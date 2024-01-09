// Copyright (C) 2024 owoDra

#include "UIManagerSubsystem.h"

#include "UIPolicy.h"
#include "UIDeveloperSettings.h"

#include "System/GFCGameInstance.h"

#include "Engine/LocalPlayer.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "Components/SlateWrapperTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UIManagerSubsystem)


void UUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (auto* GI{ Cast<UGFCGameInstance>(GetGameInstance()) })
	{
		GI->Register_OnLocalPlayerAdded(UGFCGameInstance::FLocalPlayerAddedDelegate::FDelegate::CreateUObject(this, &ThisClass::HandleAddLocalPlayer));
		GI->Register_OnLocalPlayerRemoved(UGFCGameInstance::FLocalPlayerRemovedDelegate::FDelegate::CreateUObject(this, &ThisClass::HanldeRemoveLocalPlayer));
	}

	if (!CurrentPolicy)
	{
		const auto* DevSettings{ GetDefault<UUIDeveloperSettings>() };
		const auto UIPolicySoftClass{ DevSettings->DefaultUIPolicyClass };

		if (!UIPolicySoftClass.IsNull())
		{
			auto* PolicyClass{ UIPolicySoftClass.TryLoadClass<UUIPolicy>() };

			SwitchToPolicy(NewObject<UUIPolicy>(this, PolicyClass));
		}
	}

	TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::Tick), 0.0f);
}

void UUIManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();

	SwitchToPolicy(nullptr);

	FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
}

bool UUIManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> ChildClasses;
		GetDerivedClasses(GetClass(), ChildClasses, false);

		// Only create an instance if there is no override implementation defined elsewhere

		return ChildClasses.Num() == 0;
	}

	return false;
}


bool UUIManagerSubsystem::Tick(float DeltaTime)
{
	SyncRootLayoutVisibilityToShowHUD();

	return true;
}

void UUIManagerSubsystem::SyncRootLayoutVisibilityToShowHUD()
{
	if (const auto* Policy{ GetCurrentUIPolicy() })
	{
		for (const auto& LocalPlayer : GetGameInstance()->GetLocalPlayers())
		{
			auto bShouldShowUI{ true };

			if (const auto* PC{ LocalPlayer->GetPlayerController(GetWorld()) })
			{
				const auto* HUD{ PC->GetHUD() };

				if (HUD && !HUD->bShowHUD)
				{
					bShouldShowUI = false;
				}
			}

			if (auto* RootLayout{ Policy->GetRootLayout(LocalPlayer) })
			{
				const auto DesiredVisibility{ bShouldShowUI ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed };
				if (DesiredVisibility != RootLayout->GetVisibility())
				{
					RootLayout->SetVisibility(DesiredVisibility);
				}
			}
		}
	}
}


void UUIManagerSubsystem::SwitchToPolicy(UUIPolicy* InPolicy)
{
	if (CurrentPolicy != InPolicy)
	{
		CurrentPolicy = InPolicy;
	}
}


void UUIManagerSubsystem::NotifyPlayerAdded(ULocalPlayer* LocalPlayer)
{
	if (ensure(LocalPlayer) && CurrentPolicy)
	{
		CurrentPolicy->NotifyPlayerAdded(LocalPlayer);
	}
}

void UUIManagerSubsystem::NotifyPlayerRemoved(ULocalPlayer* LocalPlayer)
{
	if (LocalPlayer && CurrentPolicy)
	{
		CurrentPolicy->NotifyPlayerRemoved(LocalPlayer);
	}
}

void UUIManagerSubsystem::NotifyPlayerDestroyed(ULocalPlayer* LocalPlayer)
{
	if (LocalPlayer && CurrentPolicy)
	{
		CurrentPolicy->NotifyPlayerDestroyed(LocalPlayer);
	}
}


void UUIManagerSubsystem::HandleAddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId)
{
	NotifyPlayerAdded(NewPlayer);
}

void UUIManagerSubsystem::HanldeRemoveLocalPlayer(ULocalPlayer* ExistingPlayer)
{
	NotifyPlayerDestroyed(ExistingPlayer);
}
