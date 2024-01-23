// Copyright (C) 2024 owoDra

#include "UIFunctionLibrary.h"

#include "UIManagerSubsystem.h"
#include "UIPolicy.h"
#include "UILayout.h"

#include "Player/GFCLocalPlayer.h"

#include "CommonActivatableWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "CommonInputSubsystem.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UIFunctionLibrary)


int32 UUIFunctionLibrary::InputSuspensions = 0;

ECommonInputType UUIFunctionLibrary::GetOwningPlayerInputType(const UUserWidget* WidgetContextObject)
{
	if (WidgetContextObject)
	{
		if (const auto* InputSubsystem{ UCommonInputSubsystem::Get(WidgetContextObject->GetOwningLocalPlayer()) })
		{
			return InputSubsystem->GetCurrentInputType();
		}
	}

	return ECommonInputType::Count;
}

bool UUIFunctionLibrary::IsOwningPlayerUsingTouch(const UUserWidget* WidgetContextObject)
{
	if (WidgetContextObject)
	{
		if (const auto* InputSubsystem{ UCommonInputSubsystem::Get(WidgetContextObject->GetOwningLocalPlayer()) })
		{
			return InputSubsystem->GetCurrentInputType() == ECommonInputType::Touch;
		}
	}
	return false;
}

bool UUIFunctionLibrary::IsOwningPlayerUsingGamepad(const UUserWidget* WidgetContextObject)
{
	if (WidgetContextObject)
	{
		if (const auto* InputSubsystem{ UCommonInputSubsystem::Get(WidgetContextObject->GetOwningLocalPlayer()) })
		{
			return InputSubsystem->GetCurrentInputType() == ECommonInputType::Gamepad;
		}
	}
	return false;
}

UCommonActivatableWidget* UUIFunctionLibrary::PushContentToLayer_ForPlayer(const ULocalPlayer* LocalPlayer, FGameplayTag LayerName, TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (!ensure(LocalPlayer) || !ensure(WidgetClass != nullptr))
	{
		return nullptr;
	}

	if (auto* UIManager{ LocalPlayer->GetGameInstance()->GetSubsystem<UUIManagerSubsystem>() })
	{
		if (auto* Policy{ UIManager->GetCurrentUIPolicy() })
		{
			if (auto* RootLayout{ Policy->GetRootLayout(LocalPlayer) })
			{
				return RootLayout->PushWidgetToLayerStack(LayerName, WidgetClass);
			}
		}
	}

	return nullptr;
}

void UUIFunctionLibrary::PushStreamedContentToLayer_ForPlayer(const ULocalPlayer* LocalPlayer, FGameplayTag LayerName, TSoftClassPtr<UCommonActivatableWidget> WidgetClass)
{
	if (!ensure(LocalPlayer) || !ensure(!WidgetClass.IsNull()))
	{
		return;
	}

	if (auto* UIManager{ LocalPlayer->GetGameInstance()->GetSubsystem<UUIManagerSubsystem>() })
	{
		if (auto* Policy{ UIManager->GetCurrentUIPolicy() })
		{
			if (auto* RootLayout{ Policy->GetRootLayout(LocalPlayer) })
			{
				const auto bSuspendInputUntilComplete{ true };

				RootLayout->PushWidgetToLayerStackAsync(LayerName, bSuspendInputUntilComplete, WidgetClass);
			}
		}
	}
}

void UUIFunctionLibrary::PopContentFromLayer(UCommonActivatableWidget* ActivatableWidget)
{
	// Ignore request to pop an already deleted widget

	if (!ActivatableWidget)
	{
		return;
	}

	if (const auto* LocalPlayer{ ActivatableWidget->GetOwningLocalPlayer() })
	{
		if (const auto* UIManager{ LocalPlayer->GetGameInstance()->GetSubsystem<UUIManagerSubsystem>() })
		{
			if (const auto* Policy{ UIManager->GetCurrentUIPolicy() })
			{
				if (auto* RootLayout{ Policy->GetRootLayout(LocalPlayer) })
				{
					RootLayout->FindAndRemoveWidgetFromLayer(ActivatableWidget);
				}
			}
		}
	}
}

ULocalPlayer* UUIFunctionLibrary::GetLocalPlayerFromController(APlayerController* PlayerController)
{
	if (PlayerController)
	{
		return PlayerController->GetLocalPlayer();
	}

	return nullptr;
}

FName UUIFunctionLibrary::SuspendInputForPlayer(APlayerController* PlayerController, FName SuspendReason)
{
	return SuspendInputForPlayer(PlayerController ? PlayerController->GetLocalPlayer() : nullptr, SuspendReason);
}

FName UUIFunctionLibrary::SuspendInputForPlayer(ULocalPlayer* LocalPlayer, FName SuspendReason)
{
	if (auto* CommonInputSubsystem{ UCommonInputSubsystem::Get(LocalPlayer) })
	{
		InputSuspensions++;
		auto SuspendToken{ SuspendReason };
		SuspendToken.SetNumber(InputSuspensions);

		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::MouseAndKeyboard, SuspendToken, true);
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Gamepad, SuspendToken, true);
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Touch, SuspendToken, true);

		return SuspendToken;
	}

	return NAME_None;
}

void UUIFunctionLibrary::ResumeInputForPlayer(APlayerController* PlayerController, FName SuspendToken)
{
	ResumeInputForPlayer(PlayerController ? PlayerController->GetLocalPlayer() : nullptr, SuspendToken);
}

void UUIFunctionLibrary::ResumeInputForPlayer(ULocalPlayer* LocalPlayer, FName SuspendToken)
{
	if (SuspendToken == NAME_None)
	{
		return;
	}

	if (UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(LocalPlayer))
	{
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::MouseAndKeyboard, SuspendToken, false);
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Gamepad, SuspendToken, false);
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Touch, SuspendToken, false);
	}
}
