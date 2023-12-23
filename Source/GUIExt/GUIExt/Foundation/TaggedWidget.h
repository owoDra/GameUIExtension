// Copyright (C) 2023 owoDra

#pragma once

#include "CommonUserWidget.h"

#include "Components/SlateWrapperTypes.h"
#include "GameplayTagContainer.h"

#include "TaggedWidget.generated.h"


/**
 * An widget in a layout that has been tagged (can be hidden or shown via tags on the owning player)
 */
UCLASS(Abstract, Blueprintable)
class UTaggedWidget : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	UTaggedWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	//
	// If the owning player has any of these tags, this widget will be hidden (using HiddenVisibility)
	//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD")
	FGameplayTagContainer HiddenByTags;

	//
	// The visibility to use when this widget is shown (not hidden by gameplay tags).
	//
	UPROPERTY(EditAnywhere, Category = "HUD")
	ESlateVisibility ShownVisibility{ ESlateVisibility::Visible };

	//
	// The visibility to use when this widget is hidden by gameplay tags.
	//
	UPROPERTY(EditAnywhere, Category = "HUD")
	ESlateVisibility HiddenVisibility{ ESlateVisibility::Collapsed };

	//
	// Do we want to be visible (ignoring tags)?
	//
	bool bWantsToBeVisible{ true };

public:
	virtual void NativeConstruct() override;

};
