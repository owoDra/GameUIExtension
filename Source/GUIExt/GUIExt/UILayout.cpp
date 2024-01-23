// Copyright (C) 2024 owoDra

#include "UILayout.h"

#include "UIManagerSubsystem.h"
#include "UIPolicy.h"
#include "GUIExtLogs.h"

#include "Player/GFCLocalPlayer.h"

#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UILayout)


UUILayout::UUILayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UUILayout::OnIsDormantChanged()
{
	//@TODO NDarnell Determine what to do with dormancy, in the past we treated dormancy as a way to shutoff rendering
	//and the view for the other local players when we force multiple players to use the player view of a single player.

	//if (UCommonLocalPlayer* LocalPlayer = GetOwningLocalPlayer<UCommonLocalPlayer>())
	//{
	//	// When the root layout is dormant, we don't want to render anything from the owner's view either
	//	LocalPlayer->SetIsPlayerViewEnabled(!bIsDormant);
	//}

	//SetVisibility(bIsDormant ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);

	//OnLayoutDormancyChanged().Broadcast(bIsDormant);
}

void UUILayout::SetIsDormant(bool InDormant)
{
	if (bIsDormant != InDormant)
	{
		const auto* LP{ GetOwningLocalPlayer() };

		UE_LOG(LogGUIE, Log, TEXT("%s UILayout Dormancy changed for [%d] from [%s] to [%s]"),
			LP && LP->IsPrimaryPlayer() ? TEXT("[Primary]") : TEXT("[Non-Primary]"),
			LP ? LP->GetControllerId() : -1,
			bIsDormant ? TEXT("Dormant") : TEXT("Not-Dormant"),
			InDormant ? TEXT("Dormant") : TEXT("Not-Dormant"));

		bIsDormant = InDormant;
		OnIsDormantChanged();
	}
}


void UUILayout::RegisterLayer(FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget)
{
	if (!IsDesignTime())
	{
		LayerWidget->OnTransitioningChanged.AddUObject(this, &UUILayout::OnWidgetStackTransitioning);

		// @TODO:	Consider allowing a transition duration, we currently set it to 0, because if it's not 0, the
		//			transition effect will cause focus to not transition properly to the new widgets when using
		//			 gamepad always.

		LayerWidget->SetTransitionDuration(0.0);

		Layers.Add(LayerTag, LayerWidget);
	}
}

void UUILayout::OnWidgetStackTransitioning(UCommonActivatableWidgetContainerBase* Widget, bool bIsTransitioning)
{
	if (bIsTransitioning)
	{
		const auto SuspendToken{ UUIFunctionLibrary::SuspendInputForPlayer(GetOwningLocalPlayer(), TEXT("GlobalStackTransion")) };
		SuspendInputTokens.Add(SuspendToken);
	}
	else
	{
		if (ensure(SuspendInputTokens.Num() > 0))
		{
			const auto SuspendToken{ SuspendInputTokens.Pop() };
			UUIFunctionLibrary::ResumeInputForPlayer(GetOwningLocalPlayer(), SuspendToken);
		}
	}
}


void UUILayout::FindAndRemoveWidgetFromLayer(UCommonActivatableWidget* ActivatableWidget)
{
	// We're not sure what layer the widget is on so go searching.

	for (const auto& KVP : Layers)
	{
		KVP.Value->RemoveWidget(*ActivatableWidget);
	}
}

UCommonActivatableWidgetContainerBase* UUILayout::GetLayerWidget(FGameplayTag LayerName)
{
	return Layers.FindRef(LayerName);
}


UUILayout* UUILayout::GetUILayoutForPrimaryPlayer(const UObject* WorldContextObject)
{
	auto* GameInstance{ UGameplayStatics::GetGameInstance(WorldContextObject) };
	auto* PlayerController{ GameInstance->GetPrimaryPlayerController(false) };

	return GetUILayout(PlayerController);
}

UUILayout* UUILayout::GetUILayout(APlayerController* PlayerController)
{
	return PlayerController ? GetUILayout(PlayerController->GetLocalPlayer()) : nullptr;
}

UUILayout* UUILayout::GetUILayout(ULocalPlayer* LocalPlayer)
{
	if (LocalPlayer)
	{
		if (const auto* GameInstance{ LocalPlayer->GetGameInstance() })
		{
			if (auto* UIManager{ GameInstance->GetSubsystem<UUIManagerSubsystem>() })
			{
				if (const auto* Policy{ UIManager->GetCurrentUIPolicy() })
				{
					if (auto* RootLayout{ Policy->GetRootLayout(LocalPlayer) })
					{
						return RootLayout;
					}
				}
			}
		}
	}

	return nullptr;
}