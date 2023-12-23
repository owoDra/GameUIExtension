// Copyright (C) 2023 owoDra

#include "HUDLayout.h"

#include "UIFunctionLibrary.h"
#include "GameplayTag/GUIETags_UI.h"

#include "UIFunctionLibrary.h"
#include "Input/CommonUIInputTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HUDLayout)


UHUDLayout::UHUDLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UHUDLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterUIActionBinding(FBindUIActionArgs(FUIActionTag::ConvertChecked(TAG_UI_Action_Escape), false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEscapeAction)));
}

void UHUDLayout::HandleEscapeAction()
{
	if (ensure(!EscapeMenuClass.IsNull()))
	{
		UUIFunctionLibrary::PushStreamedContentToLayer_ForPlayer(GetOwningLocalPlayer(), TAG_UI_Layer_Menu, EscapeMenuClass);
	}
}
