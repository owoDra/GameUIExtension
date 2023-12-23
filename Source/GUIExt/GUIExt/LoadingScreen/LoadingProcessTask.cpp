// Copyright (C) 2023 owoDra

#include "LoadingProcessTask.h"

#include "LoadingScreenManager.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "UObject/ScriptInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LoadingProcessTask)


ULoadingProcessTask::ULoadingProcessTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


bool ULoadingProcessTask::ShouldShowLoadingScreen(FString& OutReason) const
{
	OutReason = Reason;
	return true;
}

void ULoadingProcessTask::SetShowLoadingScreenReason(const FString& InReason)
{
	Reason = InReason;
}

void ULoadingProcessTask::Unregister()
{
	auto* Manager{ Cast<ULoadingScreenManager>(GetOuter()) };
	Manager->UnregisterLoadingProcessor(this);
}


ULoadingProcessTask* ULoadingProcessTask::CreateLoadingScreenProcessTask(UObject* WorldContextObject, const FString& ShowLoadingScreenReason)
{
	auto* World{ GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) };
	auto* GameInstance{ World ? World->GetGameInstance() : nullptr };
	auto* Manager{ GameInstance ? GameInstance->GetSubsystem<ULoadingScreenManager>() : nullptr };

	if (Manager)
	{
		auto* NewTask{ NewObject<ULoadingProcessTask>(Manager) };
		NewTask->SetShowLoadingScreenReason(ShowLoadingScreenReason);

		Manager->RegisterLoadingProcessor(NewTask);
		
		return NewTask;
	}

	return nullptr;
}
