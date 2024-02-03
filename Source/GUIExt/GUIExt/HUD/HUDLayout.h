// Copyright (C) 2024 owoDra

#pragma once

#include "Foundation/ActivatableWidget.h"

#include "HUDLayout.generated.h"

class UInputAction;


/**
 * Widget used to lay out the player's HUD
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class UHUDLayout : public UActivatableWidget
{
	GENERATED_BODY()
public:
	UHUDLayout(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Game Menu")
	TSoftClassPtr<UCommonActivatableWidget> EscapeMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "Game Menu")
	bool bUseEnhancedInput{ true };

	UPROPERTY(EditDefaultsOnly, Category = "Game Menu", meta = (EditCondition = "!bUseEnhancedInput"))
	FGameplayTag EscapeMenuInputTag;

	UPROPERTY(EditDefaultsOnly, Category = "Game Menu", meta = (EditCondition = "bUseEnhancedInput"))
	TSoftObjectPtr<UInputAction> EscapeMenuInputAction;

public:
	void NativeOnInitialized() override;

protected:
	void HandleEscapeAction();

};
