// Copyright (C) 2024 owoDra

#include "HUDLayout.h"

#include "UIFunctionLibrary.h"
#include "GameplayTag/GUIETags_UI.h"

#include "UIFunctionLibrary.h"
#include "Input/CommonUIInputTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HUDLayout)


UHUDLayout::UHUDLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EscapeMenuInputTag = TAG_UI_Action_Escape;
}


void UHUDLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (bUseEnhancedInput)
	{
		if (!EscapeMenuInputAction.IsNull())
		{
			auto* InputAction{ EscapeMenuInputAction.IsValid() ? EscapeMenuInputAction.Get() : EscapeMenuInputAction.LoadSynchronous() };

			RegisterUIActionBinding(FBindUIActionArgs(InputAction, false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEscapeAction)));
		}
	}
	else
	{
		if (EscapeMenuInputTag.IsValid())
		{
			RegisterUIActionBinding(FBindUIActionArgs(FUIActionTag::ConvertChecked(TAG_UI_Action_Escape), false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEscapeAction)));
		}
	}
}

void UHUDLayout::HandleEscapeAction()
{
	if (ensure(!EscapeMenuClass.IsNull()))
	{
		UUIFunctionLibrary::PushStreamedContentToLayer_ForPlayer(GetOwningLocalPlayer(), TAG_UI_Layer_Menu, EscapeMenuClass);
	}
}
