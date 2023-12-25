// Copyright (C) 2023 owoDra

#include "UIExtensionPointTypes.h"

#include "Extension/UIExtensionPointSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UIExtensionPointTypes)


///////////////////////////////////////////////////////////
// FUIExtensionPointHandle

void FUIExtensionPointHandle::Unregister()
{
	if (auto* ExtensionSourcePtr{ ExtensionSource.Get() })
	{
		ExtensionSourcePtr->UnregisterExtensionPoint(*this);
	}
}


///////////////////////////////////////////////////////////
// FUIExtensionHandle

void FUIExtensionHandle::Unregister()
{
	if (auto* ExtensionSourcePtr{ ExtensionSource.Get() })
	{
		ExtensionSourcePtr->UnregisterExtension(*this);
	}
}


///////////////////////////////////////////////////////////
// FUIExtensionPoint

bool FUIExtensionPoint::DoesExtensionPassContract(const FUIExtension* Extension) const
{
	if (auto DataPtr{ Extension->Data })
	{
		const auto bMatchesContext{ (ContextObject.IsExplicitlyNull() && Extension->ContextObject.IsExplicitlyNull()) ||
									(ContextObject == Extension->ContextObject) };

		// Make sure the contexts match.

		if (bMatchesContext)
		{
			// The data can either be the literal class of the data type, or a instance of the class type.

			const auto* DataClass{ DataPtr->IsA(UClass::StaticClass()) ? Cast<UClass>(DataPtr) : DataPtr->GetClass() };

			for (const auto& AllowedDataClass : AllowedDataClasses)
			{
				if (DataClass->IsChildOf(AllowedDataClass) || DataClass->ImplementsInterface(AllowedDataClass))
				{
					return true;
				}
			}
		}
	}

	return false;
}


///////////////////////////////////////////////////////////
// UUIExtensionHandleFunctions

void UUIExtensionHandleFunctions::Unregister(FUIExtensionHandle& Handle)
{
	Handle.Unregister();
}

bool UUIExtensionHandleFunctions::IsValid(const FUIExtensionHandle& Handle)
{
	return Handle.IsValid();
}


///////////////////////////////////////////////////////////
// UUIExtensionPointHandleFunctions

void UUIExtensionPointHandleFunctions::Unregister(FUIExtensionPointHandle& Handle)
{
	Handle.Unregister();
}

bool UUIExtensionPointHandleFunctions::IsValid(const FUIExtensionPointHandle& Handle)
{
	return Handle.IsValid();
}
