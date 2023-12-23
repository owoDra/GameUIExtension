// Copyright (C) 2023 owoDra

#include "BoundActionButtonWidget.h"

#include "CommonInputSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BoundActionButtonWidget)


void UBoundActionButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (auto* InputSubsystem{ GetInputSubsystem() })
	{
		InputSubsystem->OnInputMethodChangedNative.AddUObject(this, &ThisClass::HandleInputMethodChanged);

		HandleInputMethodChanged(InputSubsystem->GetCurrentInputType());
	}
}

void UBoundActionButtonWidget::HandleInputMethodChanged(ECommonInputType NewInputMethod)
{
	TSubclassOf<UCommonButtonStyle> NewStyle{ nullptr };

	if (NewInputMethod == ECommonInputType::Gamepad)
	{
		NewStyle = GamepadStyle;
	}
	else if (NewInputMethod == ECommonInputType::Touch)
	{
		NewStyle = TouchStyle;
	}
	else
	{
		NewStyle = KeyboardStyle;
	}

	if (NewStyle)
	{
		SetStyle(NewStyle);
	}
}
