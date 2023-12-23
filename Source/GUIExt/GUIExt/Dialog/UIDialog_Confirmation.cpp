// Copyright (C) 2023 owoDra

#include "UIDialog_Confirmation.h"

#include "Foundation/ButtonWidget.h"
#include "GUIExtLogs.h"

#include "InputCoreTypes.h"
#include "CommonBorder.h"
#include "CommonRichTextBlock.h"
#include "CommonTextBlock.h"
#include "Components/DynamicEntryBox.h"
#include "Components/SlateWrapperTypes.h"
#include "ICommonInputModule.h"
#include "Input/Reply.h"

#if WITH_EDITOR
#include "Editor/WidgetCompilerLog.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(UIDialog_Confirmation)


#if WITH_EDITOR
void UUIDialog_Confirmation::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	if (CancelAction.IsNull())
	{
		CompileLog.Error(FText::Format(FText::FromString(TEXT("{0} has unset property: CancelAction.")), FText::FromString(GetName())));
	}
}
#endif


void UUIDialog_Confirmation::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Border_TapToCloseZone->OnMouseButtonDownEvent.BindDynamic(this, &ThisClass::HandleTapToCloseZoneMouseButtonDown);
}



void UUIDialog_Confirmation::SetupDialog(UUIDialogDescriptor* Descriptor, FDialogMessageResultDelegate ResultCallback)
{
	Super::SetupDialog(Descriptor, ResultCallback);

	Text_Title->SetText(Descriptor->Header);
	RichText_Description->SetText(Descriptor->Body);

	EntryBox_Buttons->Reset<UButtonWidget>(
		[](UButtonWidget& Button)
		{
			Button.OnClicked().Clear();
		}
	);

	for (const auto& Action : Descriptor->ButtonActions)
	{
		FDataTableRowHandle ActionRow;

		switch(Action.Result)
		{
		case EDialogMessageResult::Confirmed:
			ActionRow = ICommonInputModule::GetSettings().GetDefaultClickAction();
			break;

		case EDialogMessageResult::Declined:
			ActionRow = ICommonInputModule::GetSettings().GetDefaultBackAction();
			break;

		case EDialogMessageResult::Cancelled:
			ActionRow = CancelAction;
			break;

		default:
			UE_LOG(LogGUIE, Warning, TEXT("Invalid DialogMessageResult type has set"));
			continue;
		}

		auto* Button{ EntryBox_Buttons->CreateEntry<UButtonWidget>() };
		Button->SetTriggeringInputAction(ActionRow);
		Button->OnClicked().AddUObject(this, &ThisClass::CloseConfirmationWindow, Action.Result);
		Button->SetButtonText(Action.OptionalDisplayText);
	}

	OnResultCallback = ResultCallback;
}

void UUIDialog_Confirmation::KillDialog()
{
	Super::KillDialog();
}


void UUIDialog_Confirmation::CloseConfirmationWindow(EDialogMessageResult Result)
{
	DeactivateWidget();
	OnResultCallback.ExecuteIfBound(Result);
}


FEventReply UUIDialog_Confirmation::HandleTapToCloseZoneMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	FEventReply Reply;
	Reply.NativeReply = FReply::Unhandled();

	if (MouseEvent.IsTouchEvent() || MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		CloseConfirmationWindow(EDialogMessageResult::Declined);
		Reply.NativeReply = FReply::Handled();
	}

	return Reply;
}
