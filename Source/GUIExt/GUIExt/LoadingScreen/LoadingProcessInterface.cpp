// Copyright (C) 2024 owoDra

#include "LoadingProcessInterface.h"

#include "GUIExtLogs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LoadingProcessInterface)


bool ILoadingProcessInterface::ShouldShowLoadingScreen(UObject* TestObject, FString& OutReason)
{
	if (TestObject)
	{
		if (auto* LoadObserver{ Cast<ILoadingProcessInterface>(TestObject) })
		{
			FString ObserverReason;
			if (LoadObserver->ShouldShowLoadingScreen(ObserverReason))
			{
				if (ObserverReason.IsEmpty())
				{
					UE_LOG(LogGUIE, Warning, TEXT("[%s] The reason for displaying the loading screen was not set"), *GetPathNameSafe(TestObject));
				}

				OutReason = ObserverReason;

				return true;
			}
		}
	}

	return false;
}
