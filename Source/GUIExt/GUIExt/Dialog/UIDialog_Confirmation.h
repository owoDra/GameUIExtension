// Copyright (C) 2024 owoDra

#pragma once

#include "Dialog/UIDialog.h"

#include "UIDialog_Confirmation.generated.h"

class UCommonTextBlock;
class UCommonRichTextBlock;
class UDynamicEntryBox;
class UCommonBorder;


/**
 * Base class for the confirmation dialog to be displayed
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class GUIEXT_API UUIDialog_Confirmation : public UUIDialog
{
	GENERATED_BODY()
public:
	UUIDialog_Confirmation() {}

#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
#endif

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true, BindWidget))
	TObjectPtr<UCommonTextBlock> Text_Title;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true, BindWidget))
	TObjectPtr<UCommonRichTextBlock> RichText_Description;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true, BindWidget))
	TObjectPtr<UDynamicEntryBox> EntryBox_Buttons;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true, BindWidget))
	TObjectPtr<UCommonBorder> Border_TapToCloseZone;

	UPROPERTY(EditDefaultsOnly, meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle CancelAction;


protected:
	virtual void NativeOnInitialized() override;

public:
	virtual void SetupDialog(UUIDialogDescriptor* Descriptor, FDialogMessageResultDelegate ResultCallback) override;
	virtual void KillDialog() override;

	virtual void CloseConfirmationWindow(EDialogMessageResult Result);


protected:
	FDialogMessageResultDelegate OnResultCallback;

private:
	UFUNCTION()
	FEventReply HandleTapToCloseZoneMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

};
