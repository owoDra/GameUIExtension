// Copyright (C) 2024 owoDra

#pragma once

#include "CommonActivatableWidget.h"

#include "Dialog/UIDialogTypes.h"

#include "UIDialog.generated.h"


/**
 * Class for describing the contents of a dialog
 */
UCLASS()
class GUIEXT_API UUIDialogDescriptor : public UObject
{
	GENERATED_BODY()
public:
	UUIDialogDescriptor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	//
	// The header of the message to display
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Header;

	//
	// The body of the message to display
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Body;

	//
	// The confirm button's input action to use.
	//
	UPROPERTY(BlueprintReadWrite)
	TArray<FDialogMessageAction> ButtonActions;
	
public:
	static UUIDialogDescriptor* CreateConfirmationOk(const FText& Header, const FText& Body);
	static UUIDialogDescriptor* CreateConfirmationOkCancel(const FText& Header, const FText& Body);
	static UUIDialogDescriptor* CreateConfirmationYesNo(const FText& Header, const FText& Body);
	static UUIDialogDescriptor* CreateConfirmationYesNoCancel(const FText& Header, const FText& Body);

};


/**
 * Base class for displayed dialogs
 */
UCLASS(Abstract)
class GUIEXT_API UUIDialog : public UCommonActivatableWidget
{
	GENERATED_BODY()	
public:
	UUIDialog() {}

public:
	virtual void SetupDialog(UUIDialogDescriptor* Descriptor, FDialogMessageResultDelegate ResultCallback) PURE_VIRTUAL(, );

	virtual void KillDialog() PURE_VIRTUAL(, );

};
