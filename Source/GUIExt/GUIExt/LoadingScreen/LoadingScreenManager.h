// Copyright (C) 2023 owoDra

#pragma once

#include "Framework/Application/IInputProcessor.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"

#include "UObject/WeakInterfacePtr.h"

#include "LoadingScreenManager.generated.h"

class FSubsystemCollectionBase;
class IInputProcessor;
class ILoadingProcessInterface;
class SWidget;
struct FWorldContext;


/**
 * Delegate notifies you that the load screen has been shown/hidden.
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FLoadingScreenVisibilityChangedDelegate, bool);


/**
 * Class for a processor that processes input while displaying the load screen
 */
class FLoadingScreenInputPreProcessor : public IInputProcessor
{
public:
	FLoadingScreenInputPreProcessor() {}
	virtual ~FLoadingScreenInputPreProcessor() {}

public:
	bool CanHandleInput() const { return !GIsEditor; }

	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override { }

	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override { return CanHandleInput(); }
	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override { return CanHandleInput(); }
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override { return CanHandleInput(); }
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanHandleInput(); }
	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanHandleInput(); }
	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanHandleInput(); }
	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanHandleInput(); }
	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent) override { return CanHandleInput(); }
	virtual bool HandleMotionDetectedEvent(FSlateApplication& SlateApp, const FMotionEvent& MotionEvent) override { return CanHandleInput(); }

};


/**
 * Subsystem that manages the display/hide of load screens
 */
UCLASS()
class GUIEXT_API ULoadingScreenManager 
	: public UGameInstanceSubsystem
	, public FTickableGameObject
{
	GENERATED_BODY()
public:
	ULoadingScreenManager() {}

protected:
	//
	// Slate widget for the loading screen currently displayed
	//
	TSharedPtr<SWidget> LoadingScreenWidget;

	//
	// Processor to process input while the loading screen is displayed.
	//
	TSharedPtr<IInputProcessor> InputPreProcessor;

	//
	// List of processors or actors currently loading
	//
	TArray<TWeakInterfacePtr<ILoadingProcessInterface>> LoadingProcessors;

	//
	// Reason for displaying loading screen
	//
	FString ShowLoadingScreenReason;

	//
	// Time when the loading screen is displayed
	//
	double TimeLoadingScreenShown{ 0.0 };

	//
	// Time when loading is finished and the loading screen is about to be hidden.
	// 
	// Tips:
	//	Not yet hidden depending on the value of HoldLoadingScreenAdditionalSecs in LoadingScreenSettings
	//
	double TimeLoadingScreenLastDismissed{ -1.0 };

	//
	// Whether the map is currently loaded or not (PreLoadMap to PostLoadMap)
	//
	bool bCurrentlyInLoadMap{ false };

	//
	// Whether you are currently displaying the loading screen?
	//
	bool bCurrentlyShowingLoadingScreen{ false };

public:
	//
	// Delegate notifies you that the load screen has been shown/hidden.
	//
	FLoadingScreenVisibilityChangedDelegate LoadingScreenVisibilityChanged;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LoadingScreen|Debug")
	FString GetShowLoadingScreenReason() const { return ShowLoadingScreenReason; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LoadingScreen")
	bool IsCurrentlyShowingLoadingScreen() const { return bCurrentlyShowingLoadingScreen; }

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Register a loading processor
	 */
	void RegisterLoadingProcessor(TScriptInterface<ILoadingProcessInterface> Interface);

	/**
	 * Unregister the loading processor
	 */
	void UnregisterLoadingProcessor(TScriptInterface<ILoadingProcessInterface> Interface);

public:
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;

	
protected:
	/**
	 * Notify that the map has begun loading.
	 */
	void HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName);

	/**
	 * Notify that the map has finished loading.
	 */
	void HandlePostLoadMap(UWorld* World);

	/**
	 * Update the display status of the loading screen
	 */
	void UpdateLoadingScreen();

	/**
	 * Returns whether the load screen should be displayed.
	 * Verifies if there is anything that requires a load screen and returns true if so.
	 */
	bool NeedToShowLoadingScreen();

	/**
	 * Return should the loading screen continue to be displayed.
	 * Returns true if the display should continue based on LoadingScreenSettings after the loading screen has died as required.
	 */
	bool ShouldShowLoadingScreen();

	/**
	 * Returns whether or not the initialization load screen is being displayed before the normal load screen.
	 */
	bool IsShowingInitialLoadingScreen() const;

	/**
	 * Display the load screen
	 */
	void ShowLoadingScreen();

	/**
	 * Hide and discard the loading screen.
	 */
	void HideLoadingScreen();

	/**
	 * Remove load screen widget from viewport
	 */
	void RemoveWidgetFromViewport();

	/**
	 * Enable/disable in-game input
	 */
	void SetBlockingInput(bool bBlock);

	/**
	 * Change game performance settings
	 */
	void ChangePerformanceSettings(bool bEnabingLoadingScreen);

};
