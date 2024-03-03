// Copyright (C) 2023 owoDra

#include "InputActionWidget.h"

#include "Components/Image.h"
#include "InputAction.h"
#include "PlayerMappableKeySettings.h"
#include "CommonUITypes.h"
#include "CommonInputSubsystem.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InputActionWidget)

/////////////////////////////////////////////////////////////////////////

UInputActionWidget::UInputActionWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


// Initialization

void UInputActionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (auto* InputSubsystem{ GetInputSubsystem() })
	{
		InputSubsystem->OnInputMethodChangedNative.AddUObject(this, &ThisClass::HandleInputMethodChanged);

		HandleInputMethodChanged(InputSubsystem->GetCurrentInputType());
	}
}

void UInputActionWidget::NativeDestruct()
{
	if (auto* InputSubsystem{ GetInputSubsystem() })
	{
		InputSubsystem->OnInputMethodChangedNative.RemoveAll(this);
	}

	Super::NativeDestruct();
}


// Input Method

void UInputActionWidget::HandleInputMethodChanged(ECommonInputType NewInputMethod)
{
	if (NewInputMethod == ECommonInputType::Gamepad)
	{
		UpdateKey(GamepadAction, NewInputMethod);
	}
	else if (NewInputMethod == ECommonInputType::Touch)
	{
		UpdateKey(TouchAction, NewInputMethod);
	}
	else
	{
		UpdateKey(KeyboardAction, NewInputMethod);
	}
}


// Display Key

void UInputActionWidget::UpdateKey(const FInputActionWidgetData& Data, ECommonInputType NewInputMethod)
{
	if (!CommonUI::IsEnhancedInputSupportEnabled())
	{
		return;
	}

	auto NewKey{ GetKeyOfSlot(Data) };
	NewKey = NewKey.IsValid() ? NewKey : CommonUI::GetFirstKeyForInputType(GetOwningLocalPlayer(), NewInputMethod, Data.InputAction);

	if (NewKey.IsValid())
	{
		if (auto* Resource{ GetIconForKey(NewKey).GetResourceObject() })
		{
			Image_Key->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			Image_Key->SetBrushResourceObject(Resource);
			return;
		}
	}

	Image_Key->SetVisibility(ESlateVisibility::Collapsed);
}


FKey UInputActionWidget::GetKeyOfSlot(const FInputActionWidgetData& Data) const
{
	auto MappingName{ Data.SpecificMappingName };
	if (MappingName.IsNone())
	{
		auto& InputAction{ Data.InputAction };
		auto Mappable{ InputAction ? InputAction->GetPlayerMappableKeySettings() : nullptr };

		MappingName = Mappable ? Mappable->GetMappingName() : MappingName;
	}

	const auto* LocalPlayer{ GetOwningLocalPlayer() };
	const auto* EISubsystem{ ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer) };
	auto* UserSettings{ EISubsystem ? EISubsystem->GetUserSettings() : nullptr };
	const auto* PrimaryMaping{ UserSettings->FindCurrentMappingForSlot(MappingName, Data.Slot) };

	return PrimaryMaping ? PrimaryMaping->GetCurrentKey() : FKey();
}

FSlateBrush UInputActionWidget::GetIconForKey(FKey InKey) const
{
	FSlateBrush SlateBrush;

	auto* CommonInputSubsystem{ ULocalPlayer::GetSubsystem<UCommonInputSubsystem>(GetOwningLocalPlayer()) };
	auto* InputPlatformSettings{ UCommonInputPlatformSettings::Get() };

	if (CommonInputSubsystem && InputPlatformSettings)
	{
		auto GamepadName{ CommonInputSubsystem->GetCurrentGamepadName() };
		GamepadName = GamepadName.IsNone() ? InputPlatformSettings->GetDefaultGamepadName() : GamepadName;

		InputPlatformSettings->TryGetInputBrush(
			SlateBrush,
			InKey,
			InKey.IsGamepadKey() ? ECommonInputType::Gamepad : ECommonInputType::MouseAndKeyboard,
			GamepadName);
	}

	return SlateBrush;
}
