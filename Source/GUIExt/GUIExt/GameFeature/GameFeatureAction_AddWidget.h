// Copyright (C) 2023 owoDra

#pragma once

#include "GameFeature/GameFeatureAction_WorldActionBase.h"

#include "GameplayTagContainer.h"

#include "GameFeatureAction_AddWidget.generated.h"

class UCommonActivatableWidget;
struct FComponentRequestHandle;
struct FUIExtensionHandle;


/**
 * Entry data for HUD layouts to be added with GameFeatureAction_AddWidgets
 */
USTRUCT()
struct FHUDLayoutEntry
{
	GENERATED_BODY()
public:
	FHUDLayoutEntry() {}

public:
	//
	// The layout widget to spawn
	//
	UPROPERTY(EditAnywhere, meta=(AssetBundles="Client"))
	TSoftClassPtr<UCommonActivatableWidget> LayoutClass;

	//
	// The layer to insert the widget in
	//
	UPROPERTY(EditAnywhere, meta=(Categories="UI.Layer"))
	FGameplayTag LayerID;

};


/**
 * Entry data for widgets to be added to the HUD slots to be added with GameFeatureAction_AddWidgets
 */
USTRUCT()
struct FHUDElementEntry
{
	GENERATED_BODY()
public:
	FHUDElementEntry() {}

public:
	//
	// The widget to spawn
	//
	UPROPERTY(EditAnywhere, meta=(AssetBundles="Client"))
	TSoftClassPtr<UUserWidget> WidgetClass;

	//
	// The slot ID where we should place this widget
	//
	UPROPERTY(EditAnywhere)
	FGameplayTag SlotID;

};


/**
 * GameFeatureAction responsible for granting abilities (and attributes) to actors of a specified type.
 */
UCLASS(meta = (DisplayName = "Add Widgets"))
class UGameFeatureAction_AddWidgets final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()
public:
	UGameFeatureAction_AddWidgets() {}

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#endif // WITH_EDITOR

#if WITH_EDITORONLY_DATA
	virtual void AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData) override;
#endif // WITH_EDITORONLY_DATA

private:
	struct FPerContextData
	{
		TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequests;
		TArray<TWeakObjectPtr<UCommonActivatableWidget>> LayoutsAdded;
		TArray<FUIExtensionHandle> ExtensionHandles;
	};

	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

protected:
	//
	// Layout to add to the HUD
	//
	UPROPERTY(EditAnywhere, Category = UI, meta = (TitleProperty = "{LayerID} -> {LayoutClass}"))
	TArray<FHUDLayoutEntry> Layout;

	//
	// Widgets to add to the HUD
	//
	UPROPERTY(EditAnywhere, Category = UI, meta = (TitleProperty = "{SlotID} -> {WidgetClass}"))
	TArray<FHUDElementEntry> Widgets;

public:
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

private:
	void Reset(FPerContextData& ActiveData);
	void HandleActorExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);
	void AddWidgets(AActor* Actor, FPerContextData& ActiveData);
	void RemoveWidgets(AActor* Actor, FPerContextData& ActiveData);

};
