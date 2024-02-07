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
	FGameplayTag EscapeMenuInputTag;

	UPROPERTY(EditDefaultsOnly, Category = "Game Menu")
	TSoftObjectPtr<UInputAction> EscapeMenuInputAction;

public:
	void NativeOnInitialized() override;

protected:
	void HandleEscapeAction();

};
