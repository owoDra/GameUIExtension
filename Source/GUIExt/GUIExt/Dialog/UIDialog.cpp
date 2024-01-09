// Copyright (C) 2024 owoDra

#include "UIDialog.h"

#include "UIDialogSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UIDialog)


#define LOCTEXT_NAMESPACE "Dialog"

UUIDialogDescriptor::UUIDialogDescriptor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


UUIDialogDescriptor* UUIDialogDescriptor::CreateConfirmationOk(const FText& Header, const FText& Body)
{
	auto* Descriptor{ NewObject<UUIDialogDescriptor>() };
	Descriptor->Header = Header;
	Descriptor->Body = Body;

	FDialogMessageAction ConfirmAction;
	ConfirmAction.Result = EDialogMessageResult::Confirmed;
	ConfirmAction.OptionalDisplayText = LOCTEXT("Ok", "Ok");

	Descriptor->ButtonActions.Add(ConfirmAction);

	return Descriptor;
}

UUIDialogDescriptor* UUIDialogDescriptor::CreateConfirmationOkCancel(const FText& Header, const FText& Body)
{
	auto* Descriptor{ NewObject<UUIDialogDescriptor>() };
	Descriptor->Header = Header;
	Descriptor->Body = Body;

	FDialogMessageAction ConfirmAction;
	ConfirmAction.Result = EDialogMessageResult::Confirmed;
	ConfirmAction.OptionalDisplayText = LOCTEXT("Ok", "Ok");

	FDialogMessageAction CancelAction;
	CancelAction.Result = EDialogMessageResult::Cancelled;
	CancelAction.OptionalDisplayText = LOCTEXT("Cancel", "Cancel");

	Descriptor->ButtonActions.Add(ConfirmAction);
	Descriptor->ButtonActions.Add(CancelAction);

	return Descriptor;
}

UUIDialogDescriptor* UUIDialogDescriptor::CreateConfirmationYesNo(const FText& Header, const FText& Body)
{
	auto* Descriptor{ NewObject<UUIDialogDescriptor>() };
	Descriptor->Header = Header;
	Descriptor->Body = Body;

	FDialogMessageAction ConfirmAction;
	ConfirmAction.Result = EDialogMessageResult::Confirmed;
	ConfirmAction.OptionalDisplayText = LOCTEXT("Yes", "Yes");

	FDialogMessageAction DeclineAction;
	DeclineAction.Result = EDialogMessageResult::Declined;
	DeclineAction.OptionalDisplayText = LOCTEXT("No", "No");

	Descriptor->ButtonActions.Add(ConfirmAction);
	Descriptor->ButtonActions.Add(DeclineAction);

	return Descriptor;
}

UUIDialogDescriptor* UUIDialogDescriptor::CreateConfirmationYesNoCancel(const FText& Header, const FText& Body)
{
	auto* Descriptor{ NewObject<UUIDialogDescriptor>() };
	Descriptor->Header = Header;
	Descriptor->Body = Body;

	FDialogMessageAction ConfirmAction;
	ConfirmAction.Result = EDialogMessageResult::Confirmed;
	ConfirmAction.OptionalDisplayText = LOCTEXT("Yes", "Yes");

	FDialogMessageAction DeclineAction;
	DeclineAction.Result = EDialogMessageResult::Declined;
	DeclineAction.OptionalDisplayText = LOCTEXT("No", "No");

	FDialogMessageAction CancelAction;
	CancelAction.Result = EDialogMessageResult::Cancelled;
	CancelAction.OptionalDisplayText = LOCTEXT("Cancel", "Cancel");

	Descriptor->ButtonActions.Add(ConfirmAction);
	Descriptor->ButtonActions.Add(DeclineAction);
	Descriptor->ButtonActions.Add(CancelAction);

	return Descriptor;
}

#undef LOCTEXT_NAMESPACE
