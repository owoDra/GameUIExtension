// Copyright (C) 2023 owoDra

#pragma once

#include "CommonUserWidget.h"

#include "UserSettings/EnhancedInputUserSettings.h"

#include "InputActionWidget.generated.h"

class UInputAction;
class UImage;


/**
 * InputAction data to be referenced
 */
USTRUCT(BlueprintType)
struct FInputActionWidgetData
{
	GENERATED_BODY()
public:
	FInputActionWidgetData() = default;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<const UInputAction> InputAction{ nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName SpecificMappingName{ NAME_None };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPlayerMappableKeySlot Slot{ EPlayerMappableKeySlot::First };
};


/**
 * Widget for displaying input action key icon
 */
UCLASS(Abstract, Blueprintable)
class GUIEXT_API UInputActionWidget : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	UInputActionWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	////////////////////////////////////////////////////////////
	// Initialization
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;


	////////////////////////////////////////////////////////////
	// Input Method
protected:
	void HandleInputMethodChanged(ECommonInputType NewInputMethod);


	////////////////////////////////////////////////////////////
	// Display Key
protected:
	UPROPERTY(EditAnywhere, Category = "Input Action")
	FInputActionWidgetData KeyboardAction;

	UPROPERTY(EditAnywhere, Category = "Input Action")
	FInputActionWidgetData GamepadAction;

	UPROPERTY(EditAnywhere, Category = "Input Action")
	FInputActionWidgetData TouchAction;

protected:
	virtual void UpdateKey(const FInputActionWidgetData& Data, ECommonInputType NewInputMethod);

	virtual FKey GetKeyOfSlot(const FInputActionWidgetData& Data) const;
	virtual FSlateBrush GetIconForKey(FKey InKey) const;


	////////////////////////////////////////////////////////////
	// Widget Bounds
private:	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, BlueprintProtected = true, AllowPrivateAccess = true))
	TObjectPtr<UImage> Image_Key;

};