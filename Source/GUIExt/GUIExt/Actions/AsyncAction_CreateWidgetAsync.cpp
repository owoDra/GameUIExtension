// Copyright (C) 2024 owoDra

#include "Actions/AsyncAction_CreateWidgetAsync.h"

#include "UIFunctionLibrary.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/AssetManager.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "UObject/Stack.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_CreateWidgetAsync)


UAsyncAction_CreateWidgetAsync::UAsyncAction_CreateWidgetAsync(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UAsyncAction_CreateWidgetAsync::Activate()
{
	static const auto NAME_InputFilterReason{ FName(TEXT("CreatingWidgetAsync")) };

	SuspendInputToken = bSuspendInputUntilComplete ? UUIFunctionLibrary::SuspendInputForPlayer(OwningPlayer.Get(), NAME_InputFilterReason) : NAME_None;

	StreamingHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		UserWidgetSoftClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject(this, &ThisClass::OnWidgetLoaded),
		FStreamableManager::AsyncLoadHighPriority
	);

	// Setup a cancel delegate so that we can resume input if this handler is canceled.

	StreamingHandle->BindCancelDelegate(FStreamableDelegate::CreateWeakLambda(this,
			[this]()
			{
				UUIFunctionLibrary::ResumeInputForPlayer(OwningPlayer.Get(), SuspendInputToken);
			}
		)
	);
}

void UAsyncAction_CreateWidgetAsync::Cancel()
{
	Super::Cancel();

	if (StreamingHandle.IsValid())
	{
		StreamingHandle->CancelHandle();
		StreamingHandle.Reset();
	}
}


UAsyncAction_CreateWidgetAsync* UAsyncAction_CreateWidgetAsync::CreateWidgetAsync(UObject* InWorldContextObject, TSoftClassPtr<UUserWidget> InUserWidgetSoftClass, APlayerController* InOwningPlayer, bool bSuspendInputUntilComplete)
{
	if (InUserWidgetSoftClass.IsNull())
	{
		FFrame::KismetExecutionMessage(TEXT("CreateWidgetAsync was passed a null UserWidgetSoftClass"), ELogVerbosity::Error);
		return nullptr;
	}

	auto* World{ GEngine->GetWorldFromContextObject(InWorldContextObject, EGetWorldErrorMode::LogAndReturnNull) };

	auto* Action{ NewObject<UAsyncAction_CreateWidgetAsync>() };
	Action->UserWidgetSoftClass = InUserWidgetSoftClass;
	Action->OwningPlayer = InOwningPlayer;
	Action->World = World;
	Action->GameInstance = World->GetGameInstance();
	Action->bSuspendInputUntilComplete = bSuspendInputUntilComplete;
	Action->RegisterWithGameInstance(World);

	return Action;
}


void UAsyncAction_CreateWidgetAsync::OnWidgetLoaded()
{
	if (bSuspendInputUntilComplete)
	{
		UUIFunctionLibrary::ResumeInputForPlayer(OwningPlayer.Get(), SuspendInputToken);
	}

	// If the load as successful, create it, otherwise don't complete this.

	if (auto UserWidgetClass{ UserWidgetSoftClass.Get() })
	{
		auto* UserWidget{ UWidgetBlueprintLibrary::Create(World.Get(), UserWidgetClass, OwningPlayer.Get()) };

		OnComplete.Broadcast(UserWidget);
	}

	StreamingHandle.Reset();

	SetReadyToDestroy();
}
