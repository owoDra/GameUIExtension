// Copyright (C) 2023 owoDra

#pragma once

#include "Input/CommonBoundActionButton.h"

#include "BoundActionButtonWidget.generated.h"

class UCommonButtonStyle;


/**
 * Button widget class with ability to displaying input action keys
 */
UCLASS(Abstract, meta = (DisableNativeTick))
class GUIEXT_API UBoundActionButtonWidget : public UCommonBoundActionButton
{
	GENERATED_BODY()
public:
	UBoundActionButtonWidget() {}

protected:
	UPROPERTY(EditAnywhere, Category = "Styles")
	TSubclassOf<UCommonButtonStyle> KeyboardStyle{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Styles")
	TSubclassOf<UCommonButtonStyle> GamepadStyle{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Styles")
	TSubclassOf<UCommonButtonStyle> TouchStyle{ nullptr };
	
protected:
	virtual void NativeConstruct() override;

private:
	void HandleInputMethodChanged(ECommonInputType NewInputMethod);

};
