// Copyright (C) 2023 owoDra

#pragma once

#include "Foundation/ActivatableWidget.h"

#include "HUDLayout.generated.h"


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
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCommonActivatableWidget> EscapeMenuClass;

public:
	void NativeOnInitialized() override;

protected:
	void HandleEscapeAction();

};
