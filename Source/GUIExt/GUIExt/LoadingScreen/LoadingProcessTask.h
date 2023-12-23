// Copyright (C) 2023 owoDra

#pragma once

#include "UObject/Object.h"
#include "LoadingProcessInterface.h"

#include "LoadingProcessTask.generated.h"

/**
 * Class for processing to display the loading screen
 * Register to LoadingScreenManager and manage whether to display or not.
 */
UCLASS(BlueprintType)
class GUIEXT_API ULoadingProcessTask 
	: public UObject
	, public ILoadingProcessInterface
{
	GENERATED_BODY()
public:
	ULoadingProcessTask(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	//
	// Reason for displaying loading screen
	//
	UPROPERTY(Transient)
	FString Reason;

public:
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;

	/**
	 * Set the reason for displaying the loading screen
	 */
	UFUNCTION(BlueprintCallable)
	void SetShowLoadingScreenReason(const FString& InReason);

	/**
	 * Unregister from LoadingScreenManager
	 */
	UFUNCTION(BlueprintCallable)
	void Unregister();


public:
	/**
	 * Create a new LoadingProcessTask
	 */
	UFUNCTION(BlueprintCallable, Category = "LoadingScreen", meta = (WorldContext = "WorldContextObject"))
	static ULoadingProcessTask* CreateLoadingScreenProcessTask(UObject* WorldContextObject, const FString& ShowLoadingScreenReason);

};
