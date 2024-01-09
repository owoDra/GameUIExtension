// Copyright (C) 2024 owoDra

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"

#include "Dialog/UIDialogTypes.h"

#include "AsyncAction_ShowConfirmation.generated.h"

class UUIDialogDescriptor;
class ULocalPlayer;


/**
 * Delegate to notify the result of an operation on a dialog
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDialogMessageResultMulticastDelegate, EDialogMessageResult, Result);


/**
 * Allows easily triggering an async confirmation dialog in blueprints that you can then wait on the result.
 */
UCLASS()
class UAsyncAction_ShowConfirmation : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UAsyncAction_ShowConfirmation(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(BlueprintAssignable)
	FDialogMessageResultMulticastDelegate OnResult;

private:
	UPROPERTY(Transient)
	TObjectPtr<UObject> WorldContextObject;

	UPROPERTY(Transient)
	TObjectPtr<ULocalPlayer> TargetLocalPlayer;

	UPROPERTY(Transient)
	TObjectPtr<UUIDialogDescriptor> Descriptor;

public:
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta = (BlueprintInternalUseOnly = "true", WorldContext = "InWorldContextObject"))
	static UAsyncAction_ShowConfirmation* ShowConfirmationYesNoCancel(
		UObject* InWorldContextObject, FText Title, FText Message);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta = (BlueprintInternalUseOnly = "true", WorldContext = "InWorldContextObject"))
	static UAsyncAction_ShowConfirmation* ShowConfirmationYesNo(
		UObject* InWorldContextObject, FText Title, FText Message);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta = (BlueprintInternalUseOnly = "true", WorldContext = "InWorldContextObject"))
	static UAsyncAction_ShowConfirmation* ShowConfirmationOk(
		UObject* InWorldContextObject, FText Title, FText Message);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta = (BlueprintInternalUseOnly = "true", WorldContext = "InWorldContextObject"))
	static UAsyncAction_ShowConfirmation* ShowConfirmationOkCancel(
		UObject* InWorldContextObject, FText Title, FText Message);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta = (BlueprintInternalUseOnly = "true", WorldContext = "InWorldContextObject"))
	static UAsyncAction_ShowConfirmation* ShowConfirmationCustom(
		UObject* InWorldContextObject, UUIDialogDescriptor* InDescriptor);

private:
	void HandleConfirmationResult(EDialogMessageResult ConfirmationResult);

};
