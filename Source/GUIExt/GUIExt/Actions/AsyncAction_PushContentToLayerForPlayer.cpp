// Copyright (C) 2024 owoDra

#include "Actions/AsyncAction_PushContentToLayerForPlayer.h"

#include "UILayout.h"

#include "Engine/Engine.h"
#include "UObject/Stack.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_PushContentToLayerForPlayer)


UAsyncAction_PushContentToLayerForPlayer::UAsyncAction_PushContentToLayerForPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UAsyncAction_PushContentToLayerForPlayer::Activate()
{
	if (auto* RootLayout{ UUILayout::GetUILayout(OwningPlayerPtr.Get()) })
	{
		auto WeakThis{ TWeakObjectPtr<UAsyncAction_PushContentToLayerForPlayer>(this) };

		StreamingHandle = RootLayout->PushWidgetToLayerStackAsync<UCommonActivatableWidget>(LayerName, bSuspendInputUntilComplete, WidgetClass, 
			[this, WeakThis](EAsyncWidgetLayerState State, UCommonActivatableWidget* Widget) 
			{
				if (WeakThis.IsValid())
				{
					switch (State)
					{
					case EAsyncWidgetLayerState::Initialize:
						BeforePush.Broadcast(Widget);
						break;

					case EAsyncWidgetLayerState::AfterPush:
						AfterPush.Broadcast(Widget);
						SetReadyToDestroy();
						break;

					case EAsyncWidgetLayerState::Canceled:
						SetReadyToDestroy();
						break;
					}
				}

				SetReadyToDestroy();
			}
		);
	}
	else
	{
		SetReadyToDestroy();
	}
}

void UAsyncAction_PushContentToLayerForPlayer::Cancel()
{
	Super::Cancel();

	if (StreamingHandle.IsValid())
	{
		StreamingHandle->CancelHandle();
		StreamingHandle.Reset();
	}
}


UAsyncAction_PushContentToLayerForPlayer* UAsyncAction_PushContentToLayerForPlayer::PushContentToLayerForPlayer(APlayerController* InOwningPlayer, TSoftClassPtr<UCommonActivatableWidget> InWidgetClass, FGameplayTag InLayerName, bool bSuspendInputUntilComplete)
{
	if (InWidgetClass.IsNull())
	{
		FFrame::KismetExecutionMessage(TEXT("PushContentToLayerForPlayer was passed a null WidgetClass"), ELogVerbosity::Error);
		return nullptr;
	}

	if (auto* World{ GEngine->GetWorldFromContextObject(InOwningPlayer, EGetWorldErrorMode::LogAndReturnNull) })
	{
		auto* Action{ NewObject<UAsyncAction_PushContentToLayerForPlayer>() };
		Action->WidgetClass = InWidgetClass;
		Action->OwningPlayerPtr = InOwningPlayer;
		Action->LayerName = InLayerName;
		Action->bSuspendInputUntilComplete = bSuspendInputUntilComplete;
		Action->RegisterWithGameInstance(World);

		return Action;
	}

	return nullptr;
}
