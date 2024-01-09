// Copyright (C) 2024 owoDra

#pragma once

#include "CommonUserWidget.h"

#include "UIFunctionLibrary.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

#include "UILayout.generated.h"


/**
 * The state of an async load operation for the UI.
 */
enum class EAsyncWidgetLayerState : uint8
{
	Canceled,

	Initialize,

	AfterPush
};


/**
 * The primary game UI layout of your game.  This widget class represents how to layout, push and display all layers
 * of the UI for a single player.  Each player in a split-screen game will receive their own primary game layout.
 */
UCLASS(Abstract, meta = (DisableNativeTick))
class GUIEXT_API UUILayout : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	UUILayout(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	bool bIsDormant{ false };

protected:
	virtual void OnIsDormantChanged();

public:
	/** 
	 * A dormant root layout is collapsed and responds only to persistent actions registered by the owning player 
	 */
	void SetIsDormant(bool Dormant);

	/**
	 * Returns bIsDormant value
	 */
	bool IsDormant() const { return bIsDormant; }


private:
	//
	// Lets us keep track of all suspended input tokens so that multiple async UIs can be loading and we correctly suspend
	// for the duration of all of them.
	//
	TArray<FName> SuspendInputTokens;

	//
	// The registered layers for the primary layout.
	//
	UPROPERTY(Transient, meta = (Categories = "UI.Layer"))
	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> Layers;

protected:
	/** 
	 * Register a layer that widgets can be pushed onto. 
	 */
	UFUNCTION(BlueprintCallable, Category = "Layer", meta = (GameplayTagFilter = "UI.Layer"))
	void RegisterLayer(FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget);

	void OnWidgetStackTransitioning(UCommonActivatableWidgetContainerBase* Widget, bool bIsTransitioning);

public:
	/**
	 * Find the widget if it exists on any of the layers and remove it from the layer.
	 */
	void FindAndRemoveWidgetFromLayer(UCommonActivatableWidget* ActivatableWidget);

	/**
	 * Get the layer widget for the given layer tag.
	 */
	UCommonActivatableWidgetContainerBase* GetLayerWidget(FGameplayTag LayerName);

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	TSharedPtr<FStreamableHandle> PushWidgetToLayerStackAsync(FGameplayTag LayerName, bool bSuspendInputUntilComplete, TSoftClassPtr<UCommonActivatableWidget> ActivatableWidgetClass)
	{
		return PushWidgetToLayerStackAsync<ActivatableWidgetT>(LayerName, bSuspendInputUntilComplete, ActivatableWidgetClass, [](EAsyncWidgetLayerState, ActivatableWidgetT*) {});
	}

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	TSharedPtr<FStreamableHandle> PushWidgetToLayerStackAsync(FGameplayTag LayerName, bool bSuspendInputUntilComplete, TSoftClassPtr<UCommonActivatableWidget> ActivatableWidgetClass, TFunction<void(EAsyncWidgetLayerState, ActivatableWidgetT*)> StateFunc)
	{
		static_assert(TIsDerivedFrom<ActivatableWidgetT, UCommonActivatableWidget>::IsDerived, "Only CommonActivatableWidgets can be used here");

		static const auto NAME_PushingWidgetToLayer{ FName(TEXT("PushingWidgetToLayer")) };
		const auto SuspendInputToken{ bSuspendInputUntilComplete ? UUIFunctionLibrary::SuspendInputForPlayer(GetOwningPlayer(), NAME_PushingWidgetToLayer) : NAME_None };

		auto& StreamableManager{ UAssetManager::Get().GetStreamableManager() };
		auto StreamingHandle
		{
			StreamableManager.RequestAsyncLoad(
				ActivatableWidgetClass.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(
					this, [this, LayerName, ActivatableWidgetClass, StateFunc, SuspendInputToken]()
					{
						UUIFunctionLibrary::ResumeInputForPlayer(GetOwningPlayer(), SuspendInputToken);

						auto* Widget
						{
							PushWidgetToLayerStack<ActivatableWidgetT>(
								LayerName, ActivatableWidgetClass.Get(), [StateFunc](ActivatableWidgetT& WidgetToInit)
								{
									StateFunc(EAsyncWidgetLayerState::Initialize, &WidgetToInit);
								}
							)
						};

						StateFunc(EAsyncWidgetLayerState::AfterPush, Widget);
					}
				)
			)
		};

		// Setup a cancel delegate so that we can resume input if this handler is canceled.

		StreamingHandle->BindCancelDelegate(
			FStreamableDelegate::CreateWeakLambda(
				this, [this, StateFunc, SuspendInputToken]()
				{
					UUIFunctionLibrary::ResumeInputForPlayer(GetOwningPlayer(), SuspendInputToken);
					StateFunc(EAsyncWidgetLayerState::Canceled, nullptr);
				}
			)
		);

		return StreamingHandle;
	}

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayerStack(FGameplayTag LayerName, UClass* ActivatableWidgetClass)
	{
		return PushWidgetToLayerStack<ActivatableWidgetT>(LayerName, ActivatableWidgetClass, [](ActivatableWidgetT&) {});
	}

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayerStack(FGameplayTag LayerName, UClass* ActivatableWidgetClass, TFunctionRef<void(ActivatableWidgetT&)> InitInstanceFunc)
	{
		static_assert(TIsDerivedFrom<ActivatableWidgetT, UCommonActivatableWidget>::IsDerived, "Only CommonActivatableWidgets can be used here");

		if (auto* Layer{ GetLayerWidget(LayerName) })
		{
			return Layer->AddWidget<ActivatableWidgetT>(ActivatableWidgetClass, InitInstanceFunc);
		}

		return nullptr;
	}

public:
	static UUILayout* GetUILayoutForPrimaryPlayer(const UObject* WorldContextObject);
	static UUILayout* GetUILayout(APlayerController* PlayerController);
	static UUILayout* GetUILayout(ULocalPlayer* LocalPlayer);

};
