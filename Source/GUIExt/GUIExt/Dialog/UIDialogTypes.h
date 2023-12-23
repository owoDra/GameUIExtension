// Copyright (C) 2023 owoDra

#pragma once

#include "UIDialogTypes.generated.h"


/** 
 * Possible results from a dialog 
 */
UENUM(BlueprintType)
enum class EDialogMessageResult : uint8
{
	Confirmed,	// The "yes" button was pressed

	Declined,	// The "no" button was pressed

	Cancelled,	// The "ignore/cancel" button was pressed

	Killed,		// The dialog was explicitly killed (no user input)

	Unknown UMETA(Hidden)
};


/**
 * Data that defines actions such as buttons on dialogs
 */
USTRUCT(BlueprintType)
struct FDialogMessageAction
{
	GENERATED_BODY()
public:
	FDialogMessageAction() {}

public:
	//
	// The dialog option to provide
	// 
	// Note:
	//	This value must be set
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDialogMessageResult Result{ EDialogMessageResult::Unknown };

	//
	// Display Text to use instead of the action name associated with the result.
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText OptionalDisplayText;

public:
	bool operator==(const FDialogMessageAction& Other) const
	{
		return (Result == Other.Result) && OptionalDisplayText.EqualTo(Other.OptionalDisplayText);
	}

};


/**
 * Delegate to notify the result of an operation on a dialog
 */
DECLARE_DELEGATE_OneParam(FDialogMessageResultDelegate, EDialogMessageResult /* Result */);
