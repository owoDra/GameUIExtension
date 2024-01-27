// Copyright (C) 2024 owoDra

#include "UIPolicy.h"

#include "UIManagerSubsystem.h"
#include "UILayout.h"
#include "GUIExtLogs.h"

#include "Player/GFCLocalPlayer.h"

#include "Framework/Application/SlateApplication.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UIPolicy)


UUIPolicy::UUIPolicy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UUIPolicy::AddLayoutToViewport(ULocalPlayer* LocalPlayer, UUILayout* Layout)
{
	UE_LOG(LogGameExt_UI, Log, TEXT("[%s] is adding player [%s]'s root layout [%s] to the viewport"), *GetName(), *GetNameSafe(LocalPlayer), *GetNameSafe(Layout));

	Layout->SetPlayerContext(FLocalPlayerContext(LocalPlayer));
	Layout->AddToPlayerScreen(1000);

	OnRootLayoutAddedToViewport(LocalPlayer, Layout);
}

void UUIPolicy::RemoveLayoutFromViewport(ULocalPlayer* LocalPlayer, UUILayout* Layout)
{
	auto LayoutSlateWidget{ Layout->GetCachedWidget() };

	if (LayoutSlateWidget.IsValid())
	{
		UE_LOG(LogGameExt_UI, Log, TEXT("[%s] is removing player [%s]'s root layout [%s] from the viewport"), *GetName(), *GetNameSafe(LocalPlayer), *GetNameSafe(Layout));

		Layout->RemoveFromParent();

		if (LayoutSlateWidget.IsValid())
		{
			UE_LOG(LogGameExt_UI, Log, TEXT("Player [%s]'s root layout [%s] has been removed from the viewport, but other references to its underlying Slate widget still exist. Noting in case we leak it."), *GetNameSafe(LocalPlayer), *GetNameSafe(Layout));
		}

		OnRootLayoutRemovedFromViewport(LocalPlayer, Layout);
	}
}


void UUIPolicy::OnRootLayoutAddedToViewport(ULocalPlayer* LocalPlayer, UUILayout* Layout)
{
#if WITH_EDITOR
	if (GIsEditor && LocalPlayer->IsPrimaryPlayer())
	{
		// So our controller will work in PIE without needing to click in the viewport

		FSlateApplication::Get().SetUserFocusToGameViewport(0);
	}
#endif
}

void UUIPolicy::OnRootLayoutRemovedFromViewport(ULocalPlayer* LocalPlayer, UUILayout* Layout)
{
}

void UUIPolicy::OnRootLayoutReleased(ULocalPlayer* LocalPlayer, UUILayout* Layout)
{
}


void UUIPolicy::CreateLayoutWidget(ULocalPlayer* LocalPlayer)
{
	if (auto* PlayerController{ LocalPlayer->GetPlayerController(GetWorld()) })
	{
		auto LayoutWidgetClass{ GetLayoutWidgetClass(LocalPlayer) };

		if (ensure(LayoutWidgetClass && !LayoutWidgetClass->HasAnyClassFlags(CLASS_Abstract)))
		{
			auto* NewLayoutObject{ CreateWidget<UUILayout>(PlayerController, LayoutWidgetClass) };

			RootViewportLayouts.Emplace(LocalPlayer, NewLayoutObject, true);

			AddLayoutToViewport(LocalPlayer, NewLayoutObject);
		}
	}
}


void UUIPolicy::NotifyPlayerAdded(ULocalPlayer* LocalPlayer)
{
	if (auto* GFCLP{ Cast<UGFCLocalPlayer>(LocalPlayer) })
	{
		GFCLP->OnPlayerControllerSet.AddWeakLambda(
			this, [this](UGFCLocalPlayer* LocalPlayer, APlayerController* PlayerController)
			{
				NotifyPlayerRemoved(LocalPlayer);

				if (auto* LayoutInfo{ RootViewportLayouts.FindByKey(LocalPlayer) })
				{
					AddLayoutToViewport(LocalPlayer, LayoutInfo->RootLayout);
					LayoutInfo->bAddedToViewport = true;
				}
				else
				{
					CreateLayoutWidget(LocalPlayer);
				}
			}
		);
	}

	if (auto* LayoutInfo{ RootViewportLayouts.FindByKey(LocalPlayer) })
	{
		AddLayoutToViewport(LocalPlayer, LayoutInfo->RootLayout);
		LayoutInfo->bAddedToViewport = true;
	}
	else
	{
		CreateLayoutWidget(LocalPlayer);
	}
}

void UUIPolicy::NotifyPlayerRemoved(ULocalPlayer* LocalPlayer)
{
	if (auto* LayoutInfo{ RootViewportLayouts.FindByKey(LocalPlayer) })
	{
		RemoveLayoutFromViewport(LocalPlayer, LayoutInfo->RootLayout);
		LayoutInfo->bAddedToViewport = false;

		if (MultiplayerInteractionMode == EUIMultiplayerInteractionMode::SingleToggle && !LocalPlayer->IsPrimaryPlayer())
		{
			auto RootLayout{ LayoutInfo->RootLayout };

			if (RootLayout && !RootLayout->IsDormant())
			{
				// We're removing a secondary player's root while it's in control - transfer control back to the primary player's root

				RootLayout->SetIsDormant(true);
				for (const auto& RootLayoutInfo : RootViewportLayouts)
				{
					if (RootLayoutInfo.LocalPlayer->IsPrimaryPlayer())
					{
						if (auto PrimaryRootLayout{ RootLayoutInfo.RootLayout })
						{
							PrimaryRootLayout->SetIsDormant(false);
						}
					}
				}
			}
		}
	}
}

void UUIPolicy::NotifyPlayerDestroyed(ULocalPlayer* LocalPlayer)
{
	NotifyPlayerRemoved(LocalPlayer);

	if (auto* GFCLP{ Cast<UGFCLocalPlayer>(LocalPlayer) })
	{
		GFCLP->OnPlayerControllerSet.RemoveAll(this);
	}

	const auto LayoutInfoIdx{ RootViewportLayouts.IndexOfByKey(LocalPlayer) };
	if (LayoutInfoIdx != INDEX_NONE)
	{
		auto Layout{ RootViewportLayouts[LayoutInfoIdx].RootLayout };

		RootViewportLayouts.RemoveAt(LayoutInfoIdx);

		RemoveLayoutFromViewport(LocalPlayer, Layout);

		OnRootLayoutReleased(LocalPlayer, Layout);
	}
}


void UUIPolicy::RequestPrimaryControl(UUILayout* Layout)
{
	if (MultiplayerInteractionMode == EUIMultiplayerInteractionMode::SingleToggle && Layout->IsDormant())
	{
		for (const auto& LayoutInfo : RootViewportLayouts)
		{
			auto RootLayout{ LayoutInfo.RootLayout };
			if (RootLayout && !RootLayout->IsDormant())
			{
				RootLayout->SetIsDormant(true);
				break;
			}
		}

		Layout->SetIsDormant(false);
	}
}


TSubclassOf<UUILayout> UUIPolicy::GetLayoutWidgetClass(ULocalPlayer* LocalPlayer)
{
	return LayoutClass.LoadSynchronous();
}

UWorld* UUIPolicy::GetWorld() const
{
	return GetOwningUIManager()->GetGameInstance()->GetWorld();
}

UUIManagerSubsystem* UUIPolicy::GetOwningUIManager() const
{
	return CastChecked<UUIManagerSubsystem>(GetOuter());
}

UUILayout* UUIPolicy::GetRootLayout(const ULocalPlayer* LocalPlayer) const
{
	const auto* LayoutInfo{ RootViewportLayouts.FindByKey(LocalPlayer) };

	return LayoutInfo ? LayoutInfo->RootLayout : nullptr;
}


UUIPolicy* UUIPolicy::GetUIPolicy(const UObject* WorldContextObject)
{
	if (auto* World{ GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) })
	{
		if (auto* GameInstance{ World->GetGameInstance() })
		{
			if (auto* UIManager{ UGameInstance::GetSubsystem<UUIManagerSubsystem>(GameInstance) })
			{
				return UIManager->GetCurrentUIPolicy();
			}
		}
	}

	return nullptr;
}
