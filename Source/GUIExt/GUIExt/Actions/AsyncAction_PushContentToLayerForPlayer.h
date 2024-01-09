// Copyright (C) 2024 owoDra

#pragma once

#include "Engine/CancellableAsyncAction.h"

#include "GameplayTagContainer.h"

#include "AsyncAction_PushContentToLayerForPlayer.generated.h"

class APlayerController;
class UCommonActivatableWidget;
struct FStreamableHandle;


/**
 * Delegate notifying that the widget has been pushed to the layer
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPushContentToLayerForPlayerAsyncDelegate, UCommonActivatableWidget*, UserWidget);


/**
 * Asynchronously push widgets into layers
 */
UCLASS(BlueprintType)
class UAsyncAction_PushContentToLayerForPlayer : public UCancellableAsyncAction
{
	GENERATED_BODY()
public:
	UAsyncAction_PushContentToLayerForPlayer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(BlueprintAssignable)
	FPushContentToLayerForPlayerAsyncDelegate BeforePush;

	UPROPERTY(BlueprintAssignable)
	FPushContentToLayerForPlayerAsyncDelegate AfterPush;

private:
	FGameplayTag LayerName;

	bool bSuspendInputUntilComplete{ false };

	TWeakObjectPtr<APlayerController> OwningPlayerPtr;
	TSoftClassPtr<UCommonActivatableWidget> WidgetClass;

	TSharedPtr<FStreamableHandle> StreamingHandle;

public:
	virtual void Activate() override;
	virtual void Cancel() override;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta=(WorldContext = "WorldContextObject", BlueprintInternalUseOnly="true"), meta = (GameplayTagFilter = "UI.Layer"))
	static UAsyncAction_PushContentToLayerForPlayer* PushContentToLayerForPlayer(APlayerController* OwningPlayer, TSoftClassPtr<UCommonActivatableWidget> WidgetClass, FGameplayTag LayerName, bool bSuspendInputUntilComplete = true);

};
