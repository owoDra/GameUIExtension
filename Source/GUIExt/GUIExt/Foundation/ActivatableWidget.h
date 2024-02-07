// Copyright (C) 2024 owoDra

#pragma once

#include "CommonActivatableWidget.h"

#include "GameplayTagContainer.h"

#include "ActivatableWidget.generated.h"


/**
 * Input mode for widget activation
 */
UENUM(BlueprintType)
enum class EWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};


/**
 * An activatable widget that automatically drives the desired input config when activated
 */
UCLASS(Abstract, Blueprintable)
class GUIEXT_API UActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	UActivatableWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR
	virtual void ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, class IWidgetCompilerLog& CompileLog) const override;
#endif
	
protected:
	//
	// The desired input mode to use while this UI is activated, for example do you want key presses to still reach the game/player controller?
	//
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	EWidgetInputMode InputMode{ EWidgetInputMode::Default };

	//
	// The desired mouse behavior when the game gets input.
	//
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
	
public:
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

};
