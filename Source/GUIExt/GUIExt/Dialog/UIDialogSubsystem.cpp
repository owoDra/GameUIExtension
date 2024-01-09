// Copyright (C) 2024 owoDra

#include "UIDialogSubsystem.h"

#include "Dialog/UIDialog.h"
#include "UIPolicy.h"
#include "UILayout.h"
#include "UIManagerSubsystem.h"
#include "UIDeveloperSettings.h"
#include "GameplayTag/GUIETags_UI.h"

#include "Engine/LocalPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UIDialogSubsystem)


void UUIDialogSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const auto* DevSetting{ GetDefault<UUIDeveloperSettings>() };

	ConfirmationDialogClass = DevSetting->ConfirmationDialogClass.TryLoadClass<UUIDialog>();
	ErrorDialogClass = DevSetting->ErrorDialogClass.TryLoadClass<UUIDialog>();
}

bool UUIDialogSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<ULocalPlayer>(Outer)->GetGameInstance()->IsDedicatedServerInstance())
	{
		TArray<UClass*> ChildClasses;
		GetDerivedClasses(GetClass(), ChildClasses, false);

		// Only create an instance if there is no override implementation defined elsewhere

		return ChildClasses.Num() == 0;
	}

	return false;
}


void UUIDialogSubsystem::ShowConfirmation(UUIDialogDescriptor* DialogDescriptor, FDialogMessageResultDelegate ResultCallback)
{
	const auto* LocalPlayer{ GetLocalPlayer<ULocalPlayer>() };
	const auto* GameInstance{ LocalPlayer ? LocalPlayer->GetGameInstance() : nullptr};
	const auto* UIManager{ GameInstance ? GameInstance->GetSubsystem<UUIManagerSubsystem>() : nullptr };
	const auto* Policy{ UIManager ? UIManager->GetCurrentUIPolicy() : nullptr };

	if (auto* RootLayout{ Policy ? Policy->GetRootLayout(LocalPlayer) : nullptr })
	{
		RootLayout->PushWidgetToLayerStack<UUIDialog>(TAG_UI_Layer_Modal, ConfirmationDialogClass, 
			[DialogDescriptor, ResultCallback](UUIDialog& Dialog) 
			{
				Dialog.SetupDialog(DialogDescriptor, ResultCallback);
			}
		);
	}
}

void UUIDialogSubsystem::ShowError(UUIDialogDescriptor* DialogDescriptor, FDialogMessageResultDelegate ResultCallback)
{
	const auto* LocalPlayer{ GetLocalPlayer<ULocalPlayer>() };
	const auto* GameInstance{ LocalPlayer ? LocalPlayer->GetGameInstance() : nullptr };
	const auto* UIManager{ GameInstance ? GameInstance->GetSubsystem<UUIManagerSubsystem>() : nullptr };
	const auto* Policy{ UIManager ? UIManager->GetCurrentUIPolicy() : nullptr };

	if (auto * RootLayout{ Policy ? Policy->GetRootLayout(LocalPlayer) : nullptr })
	{
		RootLayout->PushWidgetToLayerStack<UUIDialog>(TAG_UI_Layer_Modal, ErrorDialogClass,
			[DialogDescriptor, ResultCallback](UUIDialog& Dialog)
			{
				Dialog.SetupDialog(DialogDescriptor, ResultCallback);
			}
		);
	}
}