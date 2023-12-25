// Copyright (C) 2023 owoDra

#include "UIExtensionPointSubsystem.h"

#include "GUIExtLogs.h"

#include "Blueprint/UserWidget.h"
#include "UObject/Stack.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UIExtensionPointSubsystem)


void UUIExtensionPointSubsystem::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	if (auto* ExtensionSubsystem{ Cast<UUIExtensionPointSubsystem>(InThis) })
	{
		for (auto MapIt{ ExtensionSubsystem->ExtensionPointMap.CreateIterator() }; MapIt; ++MapIt)
		{
			for (const auto& ValueElement : MapIt.Value())
			{
				Collector.AddReferencedObjects(ValueElement->AllowedDataClasses);
			}
		}

		for (auto MapIt{ ExtensionSubsystem->ExtensionMap.CreateIterator() }; MapIt; ++MapIt)
		{
			for (const auto& ValueElement : MapIt.Value())
			{
				Collector.AddReferencedObject(ValueElement->Data);
			}
		}
	}
}


FUIExtensionPointHandle UUIExtensionPointSubsystem::RegisterExtensionPoint(const FGameplayTag& ExtensionPointTag, EUIExtensionPointMatch ExtensionPointTagMatchType, const TArray<UClass*>& AllowedDataClasses, FUIExtensionActionDelegate ExtensionCallback)
{
	return RegisterExtensionPointForContext(ExtensionPointTag, nullptr, ExtensionPointTagMatchType, AllowedDataClasses, ExtensionCallback);
}

FUIExtensionPointHandle UUIExtensionPointSubsystem::RegisterExtensionPointForContext(const FGameplayTag& ExtensionPointTag, UObject* ContextObject, EUIExtensionPointMatch ExtensionPointTagMatchType, const TArray<UClass*>& AllowedDataClasses, FUIExtensionActionDelegate ExtensionCallback)
{
	if (!ExtensionPointTag.IsValid())
	{
		UE_LOG(LogGUIE, Warning, TEXT("Trying to register an invalid extension point."));
		return FUIExtensionPointHandle();
	}

	if (!ExtensionCallback.IsBound())
	{
		UE_LOG(LogGUIE, Warning, TEXT("Trying to register an invalid extension point."));
		return FUIExtensionPointHandle();
	}

	if (AllowedDataClasses.Num() == 0)
	{
		UE_LOG(LogGUIE, Warning, TEXT("Trying to register an invalid extension point."));
		return FUIExtensionPointHandle();
	}

	auto& List{ ExtensionPointMap.FindOrAdd(ExtensionPointTag) };
	auto& Entry{ List.Add_GetRef(MakeShared<FUIExtensionPoint>()) };
	Entry->ExtensionPointTag			= ExtensionPointTag;
	Entry->ContextObject				= ContextObject;
	Entry->ExtensionPointTagMatchType	= ExtensionPointTagMatchType;
	Entry->AllowedDataClasses			= AllowedDataClasses;
	Entry->Callback						= MoveTemp(ExtensionCallback);

	UE_LOG(LogGUIE, Log, TEXT("Extension Point [%s] Registered"), *ExtensionPointTag.ToString());

	NotifyExtensionPointOfExtensions(Entry);

	return FUIExtensionPointHandle(this, Entry);
}


FUIExtensionHandle UUIExtensionPointSubsystem::RegisterExtensionAsWidget(const FGameplayTag& ExtensionPointTag, TSubclassOf<UUserWidget> WidgetClass, int32 Priority)
{
	return RegisterExtensionAsData(ExtensionPointTag, nullptr, WidgetClass, Priority);
}

FUIExtensionHandle UUIExtensionPointSubsystem::RegisterExtensionAsWidgetForContext(const FGameplayTag& ExtensionPointTag, UObject* ContextObject, TSubclassOf<UUserWidget> WidgetClass, int32 Priority)
{
	return RegisterExtensionAsData(ExtensionPointTag, ContextObject, WidgetClass, Priority);
}

FUIExtensionHandle UUIExtensionPointSubsystem::RegisterExtensionAsData(const FGameplayTag& ExtensionPointTag, UObject* ContextObject, UObject* Data, int32 Priority)
{
	if (!ExtensionPointTag.IsValid())
	{
		UE_LOG(LogGUIE, Warning, TEXT("Trying to register an invalid extension."));
		return FUIExtensionHandle();
	}

	if (!Data)
	{
		UE_LOG(LogGUIE, Warning, TEXT("Trying to register an invalid extension."));
		return FUIExtensionHandle();
	}

	auto& List{ ExtensionMap.FindOrAdd(ExtensionPointTag) };
	auto& Entry{ List.Add_GetRef(MakeShared<FUIExtension>()) };
	Entry->ExtensionPointTag	= ExtensionPointTag;
	Entry->ContextObject		= ContextObject;
	Entry->Data					= Data;
	Entry->Priority				= Priority;

	if (ContextObject)
	{
		UE_LOG(LogGUIE, Log, TEXT("Extension [%s] @ [%s] Registered"), *GetNameSafe(Data), *ExtensionPointTag.ToString());
	}
	else
	{
		UE_LOG(LogGUIE, Log, TEXT("Extension [%s] for [%s] @ [%s] Registered"), *GetNameSafe(Data), *GetNameSafe(ContextObject), *ExtensionPointTag.ToString());
	}

	NotifyExtensionPointsOfExtension(EUIExtensionAction::Added, Entry);

	return FUIExtensionHandle(this, Entry);
}


void UUIExtensionPointSubsystem::UnregisterExtension(const FUIExtensionHandle& ExtensionHandle)
{
	if (ExtensionHandle.IsValid())
	{
		checkf((ExtensionHandle.ExtensionSource == this), TEXT("Trying to unregister an extension that's not from this extension subsystem."));

		auto Extension{ ExtensionHandle.DataPtr };

		if (auto* ListPtr{ ExtensionMap.Find(Extension->ExtensionPointTag) })
		{
			if (Extension->ContextObject.IsExplicitlyNull())
			{
				UE_LOG(LogGUIE, Log, TEXT("Extension [%s] @ [%s] Unregistered"), *GetNameSafe(Extension->Data), *Extension->ExtensionPointTag.ToString());
			}
			else
			{
				UE_LOG(LogGUIE, Log, TEXT("Extension [%s] for [%s] @ [%s] Unregistered"), *GetNameSafe(Extension->Data), *GetNameSafe(Extension->ContextObject.Get()), *Extension->ExtensionPointTag.ToString());
			}

			NotifyExtensionPointsOfExtension(EUIExtensionAction::Removed, Extension);

			ListPtr->RemoveSwap(Extension);
			
			if (ListPtr->Num() == 0)
			{
				ExtensionMap.Remove(Extension->ExtensionPointTag);
			}
		}
	}
	else
	{
		UE_LOG(LogGUIE, Warning, TEXT("Trying to unregister an invalid Handle."));
	}
}

void UUIExtensionPointSubsystem::UnregisterExtensionPoint(const FUIExtensionPointHandle& ExtensionPointHandle)
{
	if (ExtensionPointHandle.IsValid())
	{
		check(ExtensionPointHandle.ExtensionSource == this);

		const auto ExtensionPoint{ ExtensionPointHandle.DataPtr };

		if (auto* ListPtr{ ExtensionPointMap.Find(ExtensionPoint->ExtensionPointTag) })
		{
			UE_LOG(LogGUIE, Log, TEXT("Extension Point [%s] Unregistered"), *ExtensionPoint->ExtensionPointTag.ToString());

			ListPtr->RemoveSwap(ExtensionPoint);

			if (ListPtr->Num() == 0)
			{
				ExtensionPointMap.Remove(ExtensionPoint->ExtensionPointTag);
			}
		}
	}
	else
	{
		UE_LOG(LogGUIE, Warning, TEXT("Trying to unregister an invalid Handle."));
	}
}


FUIExtensionPointHandle UUIExtensionPointSubsystem::K2_RegisterExtensionPoint(FGameplayTag ExtensionPointTag, EUIExtensionPointMatch ExtensionPointTagMatchType, const TArray<UClass*>& AllowedDataClasses, FUIExtensionPointActionDelegate ExtensionCallback)
{
	return RegisterExtensionPoint(
		ExtensionPointTag, 
		ExtensionPointTagMatchType, 
		AllowedDataClasses, 
		FUIExtensionActionDelegate::CreateWeakLambda(ExtensionCallback.GetUObject(),
			[this, ExtensionCallback](EUIExtensionAction Action, const FUIExtensionRequest& Request) 
			{
				ExtensionCallback.ExecuteIfBound(Action, Request);
			}
		)
	);
}

FUIExtensionHandle UUIExtensionPointSubsystem::K2_RegisterExtensionAsWidget(FGameplayTag ExtensionPointTag, TSubclassOf<UUserWidget> WidgetClass, int32 Priority)
{
	return RegisterExtensionAsWidget(ExtensionPointTag, WidgetClass, Priority);
}


FUIExtensionHandle UUIExtensionPointSubsystem::K2_RegisterExtensionAsWidgetForContext(FGameplayTag ExtensionPointTag, TSubclassOf<UUserWidget> WidgetClass, UObject* ContextObject, int32 Priority)
{
	if (ContextObject)
	{
		return RegisterExtensionAsWidgetForContext(ExtensionPointTag, ContextObject, WidgetClass, Priority);
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("A null ContextObject was passed to Register Extension (Widget For Context)"), ELogVerbosity::Error);

		return FUIExtensionHandle();
	}
}

FUIExtensionHandle UUIExtensionPointSubsystem::K2_RegisterExtensionAsData(FGameplayTag ExtensionPointTag, UObject* Data, int32 Priority)
{
	return RegisterExtensionAsData(ExtensionPointTag, nullptr, Data, Priority);
}

FUIExtensionHandle UUIExtensionPointSubsystem::K2_RegisterExtensionAsDataForContext(FGameplayTag ExtensionPointTag, UObject* ContextObject, UObject* Data, int32 Priority)
{
	if (ContextObject)
	{
		return RegisterExtensionAsData(ExtensionPointTag, ContextObject, Data, Priority);
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("A null ContextObject was passed to Register Extension (Data For Context)"), ELogVerbosity::Error);
		
		return FUIExtensionHandle();
	}
}


void UUIExtensionPointSubsystem::NotifyExtensionPointOfExtensions(TSharedPtr<FUIExtensionPoint>& ExtensionPoint)
{
	for (auto Tag{ ExtensionPoint->ExtensionPointTag }; Tag.IsValid(); Tag = Tag.RequestDirectParent())
	{
		if (const auto* ListPtr{ ExtensionMap.Find(Tag) })
		{
			// Copy in case there are removals while handling callbacks

			auto ExtensionArray{ FExtensionList(*ListPtr) };

			for (const auto& Extension : ExtensionArray)
			{
				if (ExtensionPoint->DoesExtensionPassContract(Extension.Get()))
				{
					auto Request{ CreateExtensionRequest(Extension) };

					ExtensionPoint->Callback.ExecuteIfBound(EUIExtensionAction::Added, Request);
				}
			}
		}

		if (ExtensionPoint->ExtensionPointTagMatchType == EUIExtensionPointMatch::ExactMatch)
		{
			break;
		}
	}
}

void UUIExtensionPointSubsystem::NotifyExtensionPointsOfExtension(EUIExtensionAction Action, TSharedPtr<FUIExtension>& Extension)
{
	auto bOnInitialTag{ true };

	for (auto Tag{ Extension->ExtensionPointTag }; Tag.IsValid(); Tag = Tag.RequestDirectParent())
	{
		if (const auto* ListPtr{ ExtensionPointMap.Find(Tag) })
		{
			// Copy in case there are removals while handling callbacks

			auto ExtensionPointArray{ FExtensionPointList(*ListPtr) };

			for (const auto& ExtensionPoint : ExtensionPointArray)
			{
				if (bOnInitialTag || (ExtensionPoint->ExtensionPointTagMatchType == EUIExtensionPointMatch::PartialMatch))
				{
					if (ExtensionPoint->DoesExtensionPassContract(Extension.Get()))
					{
						auto Request{ CreateExtensionRequest(Extension) };

						ExtensionPoint->Callback.ExecuteIfBound(Action, Request);
					}
				}
			}
		}

		bOnInitialTag = false;
	}
}


FUIExtensionRequest UUIExtensionPointSubsystem::CreateExtensionRequest(const TSharedPtr<FUIExtension>& Extension)
{
	FUIExtensionRequest Request;
	Request.ExtensionHandle = FUIExtensionHandle(this, Extension);
	Request.ExtensionPointTag = Extension->ExtensionPointTag;
	Request.Priority = Extension->Priority;
	Request.Data = Extension->Data;
	Request.ContextObject = Extension->ContextObject.Get();

	return Request;
}
