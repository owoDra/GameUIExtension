// Copyright (C) 2024 owoDra

#pragma once

#include "CommonButtonBase.h"

#include "ButtonWidget.generated.h"


UCLASS(Abstract, BlueprintType, Blueprintable)
class UButtonWidget : public UCommonButtonBase
{
	GENERATED_BODY()
public:
	UButtonWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativePreConstruct() override;

	virtual void UpdateInputActionWidget() override;
	virtual void OnInputMethodChanged(ECommonInputType CurrentInputType) override;


protected:
	UPROPERTY(EditAnywhere, Category = "Button", meta = (InlineEditConditionToggle))
	uint8 bOverride_ButtonText : 1;

	UPROPERTY(EditAnywhere, Category = "Button", meta = (editcondition = "bOverride_ButtonText"))
	FText ButtonText;

public:
	UFUNCTION(BlueprintCallable)
	void SetButtonText(const FText& InText);
	
protected:
	void RefreshButtonText();
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonText(const FText& InText);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateButtonStyle();
	
};