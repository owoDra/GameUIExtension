// Copyright (C) 2024 owoDra

#pragma once

#include "Engine/CancellableAsyncAction.h"

#include "AsyncAction_CreateWidgetAsync.generated.h"

class APlayerController;
class UGameInstance;
class UUserWidget;
class UWorld;
struct FStreamableHandle;


/**
 * Delegate notifying that the widget has been created
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCreateWidgetAsyncDelegate, UUserWidget*, UserWidget);


/**
 * Load the widget class asynchronously
 */
UCLASS(BlueprintType)
class UAsyncAction_CreateWidgetAsync : public UCancellableAsyncAction
{
	GENERATED_BODY()
public:
	UAsyncAction_CreateWidgetAsync(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(BlueprintAssignable)
	FCreateWidgetAsyncDelegate OnComplete;

private:
	FName SuspendInputToken;
	TWeakObjectPtr<APlayerController> OwningPlayer;
	TWeakObjectPtr<UWorld> World;
	TWeakObjectPtr<UGameInstance> GameInstance;

	bool bSuspendInputUntilComplete{ true };

	TSoftClassPtr<UUserWidget> UserWidgetSoftClass;
	TSharedPtr<FStreamableHandle> StreamingHandle;

public:
	virtual void Activate() override;
	virtual void Cancel() override;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta=(WorldContext = "WorldContextObject", BlueprintInternalUseOnly="true"))
	static UAsyncAction_CreateWidgetAsync* CreateWidgetAsync(UObject* WorldContextObject, TSoftClassPtr<UUserWidget> UserWidgetSoftClass, APlayerController* OwningPlayer, bool bSuspendInputUntilComplete = true);

private:
	void OnWidgetLoaded();

};
