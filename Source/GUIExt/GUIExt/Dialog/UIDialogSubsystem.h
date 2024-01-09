// Copyright (C) 2024 owoDra

#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"

#include "Dialog/UIDialogTypes.h"

#include "UIDialogSubsystem.generated.h"

class UUIDialog;
class UUIDialogDescriptor;


/**
 * Subsystems that manage dialog messages
 */
UCLASS()
class UUIDialogSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
public:
	UUIDialogSubsystem() {}

protected:
	UPROPERTY()
	TSubclassOf<UUIDialog> ConfirmationDialogClass;

	UPROPERTY()
	TSubclassOf<UUIDialog> ErrorDialogClass;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
	virtual void ShowConfirmation(UUIDialogDescriptor* DialogDescriptor, FDialogMessageResultDelegate ResultCallback = FDialogMessageResultDelegate());
	virtual void ShowError(UUIDialogDescriptor* DialogDescriptor, FDialogMessageResultDelegate ResultCallback = FDialogMessageResultDelegate());

};