// Copyright (C) 2024 owoDra

#include "Actions/AsyncAction_ShowConfirmation.h"

#include "Dialog/UIDialog.h"
#include "Dialog/UIDialogSubsystem.h"

#include "Engine/GameInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_ShowConfirmation)


UAsyncAction_ShowConfirmation::UAsyncAction_ShowConfirmation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UAsyncAction_ShowConfirmation::Activate()
{
	if (WorldContextObject && !TargetLocalPlayer)
	{
		if (auto* UserWidget{ Cast<UUserWidget>(WorldContextObject) })
		{
			TargetLocalPlayer = UserWidget->GetOwningLocalPlayer<ULocalPlayer>();
		}

		else if (auto* PC{ Cast<APlayerController>(WorldContextObject) })
		{
			TargetLocalPlayer = PC->GetLocalPlayer();
		}

		else if (auto* World{ WorldContextObject->GetWorld() })
		{
			if (auto* GameInstance{ World->GetGameInstance<UGameInstance>() })
			{
				TargetLocalPlayer = GameInstance->GetPrimaryPlayerController(false)->GetLocalPlayer();
			}
		}
	}

	if (TargetLocalPlayer)
	{
		if (auto* Subsystem{ TargetLocalPlayer->GetSubsystem<UUIDialogSubsystem>() })
		{
			FDialogMessageResultDelegate ResultCallback{ FDialogMessageResultDelegate::CreateUObject(this, &UAsyncAction_ShowConfirmation::HandleConfirmationResult) };
			Subsystem->ShowConfirmation(Descriptor, ResultCallback);

			return;
		}
	}

	// If we couldn't make the confirmation, just handle an unknown result and broadcast nothing

	HandleConfirmationResult(EDialogMessageResult::Unknown);
}


UAsyncAction_ShowConfirmation* UAsyncAction_ShowConfirmation::ShowConfirmationYesNoCancel(UObject* InWorldContextObject, FText Title, FText Message)
{
	auto* Action{ NewObject<UAsyncAction_ShowConfirmation>() };
	Action->WorldContextObject = InWorldContextObject;
	Action->Descriptor = UUIDialogDescriptor::CreateConfirmationYesNoCancel(Title, Message);
	Action->RegisterWithGameInstance(InWorldContextObject);

	return Action;
}

UAsyncAction_ShowConfirmation* UAsyncAction_ShowConfirmation::ShowConfirmationYesNo(UObject* InWorldContextObject, FText Title, FText Message)
{
	auto* Action{ NewObject<UAsyncAction_ShowConfirmation>() };
	Action->WorldContextObject = InWorldContextObject;
	Action->Descriptor = UUIDialogDescriptor::CreateConfirmationYesNo(Title, Message);
	Action->RegisterWithGameInstance(InWorldContextObject);

	return Action;
}

UAsyncAction_ShowConfirmation* UAsyncAction_ShowConfirmation::ShowConfirmationOk(UObject* InWorldContextObject, FText Title, FText Message)
{
	auto* Action{ NewObject<UAsyncAction_ShowConfirmation>() };
	Action->WorldContextObject = InWorldContextObject;
	Action->Descriptor = UUIDialogDescriptor::CreateConfirmationOk(Title, Message);
	Action->RegisterWithGameInstance(InWorldContextObject);

	return Action;
}

UAsyncAction_ShowConfirmation* UAsyncAction_ShowConfirmation::ShowConfirmationOkCancel(UObject* InWorldContextObject, FText Title, FText Message)
{
	auto* Action{ NewObject<UAsyncAction_ShowConfirmation>() };
	Action->WorldContextObject = InWorldContextObject;
	Action->Descriptor = UUIDialogDescriptor::CreateConfirmationOkCancel(Title, Message);
	Action->RegisterWithGameInstance(InWorldContextObject);

	return Action;
}

UAsyncAction_ShowConfirmation* UAsyncAction_ShowConfirmation::ShowConfirmationCustom(UObject* InWorldContextObject, UUIDialogDescriptor* InDescriptor)
{
	auto* Action{ NewObject<UAsyncAction_ShowConfirmation>() };
	Action->WorldContextObject = InWorldContextObject;
	Action->Descriptor = InDescriptor;
	Action->RegisterWithGameInstance(InWorldContextObject);

	return Action;
}


void UAsyncAction_ShowConfirmation::HandleConfirmationResult(EDialogMessageResult ConfirmationResult)
{
	OnResult.Broadcast(ConfirmationResult);

	SetReadyToDestroy();
}
