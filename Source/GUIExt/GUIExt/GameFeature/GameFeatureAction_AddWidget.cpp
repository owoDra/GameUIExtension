// Copyright (C) 2024 owoDra

#include "GameFeatureAction_AddWidget.h"

#include "Extension/UIExtensionPointSubsystem.h"
#include "UIFunctionLibrary.h"

#include "Actor/GFCHUD.h"

#include "Components/GameFrameworkComponentManager.h"
#include "GameFeaturesSubsystemSettings.h"
#include "CommonActivatableWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddWidget)


#define LOCTEXT_NAMESPACE "GameFeatures"

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddWidgets::IsDataValid(TArray<FText>& ValidationErrors)
{
	auto Result{ CombineDataValidationResults(Super::IsDataValid(ValidationErrors), EDataValidationResult::Valid) };

	{
		auto EntryIndex{ 0 };
		for (const auto& Entry : Layout)
		{
			if (Entry.LayoutClass.IsNull())
			{
				Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
				ValidationErrors.Add(FText::Format(LOCTEXT("LayoutHasNullClass", "Null WidgetClass at index {0} in Layout"), FText::AsNumber(EntryIndex)));
			}

			if (!Entry.LayerID.IsValid())
			{
				Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
				ValidationErrors.Add(FText::Format(LOCTEXT("LayoutHasNoTag", "LayerID is not set at index {0} in Widgets"), FText::AsNumber(EntryIndex)));
			}

			++EntryIndex;
		}
	}

	{
		auto EntryIndex{ 0 };
		for (const auto& Entry : Widgets)
		{
			if (Entry.WidgetClass.IsNull())
			{
				Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
				ValidationErrors.Add(FText::Format(LOCTEXT("EntryHasNullClass", "Null WidgetClass at index {0} in Widgets"), FText::AsNumber(EntryIndex)));
			}

			if (!Entry.SlotID.IsValid())
			{
				Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
				ValidationErrors.Add(FText::Format(LOCTEXT("EntryHasNoTag", "SlotID is not set at index {0} in Widgets"), FText::AsNumber(EntryIndex)));
			}
			++EntryIndex;
		}
	}

	return Result;
}
#endif

#if WITH_EDITORONLY_DATA
void UGameFeatureAction_AddWidgets::AddAdditionalAssetBundleData(FAssetBundleData& AssetBundleData)
{
	for (const auto& Entry : Widgets)
	{
		AssetBundleData.AddBundleAsset(UGameFeaturesSubsystemSettings::LoadStateClient, Entry.WidgetClass.ToSoftObjectPath().GetAssetPath());
	}
}
#endif


void UGameFeatureAction_AddWidgets::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	auto& ActiveData{ ContextData.FindOrAdd(Context) };

	if (!ensure(ActiveData.ComponentRequests.IsEmpty()) || !ensure(ActiveData.LayoutsAdded.IsEmpty()) || !ensure(ActiveData.ExtensionHandles.IsEmpty()))
	{
		Reset(ActiveData);
	}

	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddWidgets::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	auto* ActiveData{ ContextData.Find(Context) };

	if (ensure(ActiveData))
	{
		Reset(*ActiveData);
	}
}


void UGameFeatureAction_AddWidgets::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	auto* World{ WorldContext.World() };
	auto GameInstance{ WorldContext.OwningGameInstance };
	auto& ActiveData{ ContextData.FindOrAdd(ChangeContext) };

	if ((GameInstance != nullptr) && (World != nullptr) && World->IsGameWorld())
	{
		if (auto* Manager{ UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance) })
		{
			auto HUDActorClass{ AGFCHUD::StaticClass() };
			auto ExtensionRequestHandle{ Manager->AddExtensionHandler(HUDActorClass, UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandleActorExtension, ChangeContext)) };
			
			ActiveData.ComponentRequests.Add(ExtensionRequestHandle);
		}
	}
}


void UGameFeatureAction_AddWidgets::Reset(FPerContextData& ActiveData)
{
	ActiveData.ComponentRequests.Empty();
	ActiveData.LayoutsAdded.Empty();

	for (auto& Handle : ActiveData.ExtensionHandles)
	{
		Handle.Unregister();
	}

	ActiveData.ExtensionHandles.Reset();
}

void UGameFeatureAction_AddWidgets::HandleActorExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
{
	auto& ActiveData{ ContextData.FindOrAdd(ChangeContext) };

	if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
	{
		RemoveWidgets(Actor, ActiveData);
	}
	else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UGameFrameworkComponentManager::NAME_GameActorReady))
	{
		AddWidgets(Actor, ActiveData);
	}
}

void UGameFeatureAction_AddWidgets::AddWidgets(AActor* Actor, FPerContextData& ActiveData)
{
	if (auto* HUD{ Cast<AGFCHUD>(Actor) })
	{
		if (auto* LocalPlayer{ Cast<ULocalPlayer>(HUD->GetOwningPlayerController()->Player) })
		{
			for (const auto& Entry : Layout)
			{
				if (auto ConcreteWidgetClass{ Entry.LayoutClass.Get() })
				{
					ActiveData.LayoutsAdded.Add(UUIFunctionLibrary::PushContentToLayer_ForPlayer(LocalPlayer, Entry.LayerID, ConcreteWidgetClass));
				}
			}

			auto* ExtensionSubsystem{ HUD->GetWorld()->GetSubsystem<UUIExtensionPointSubsystem>() };

			for (const auto& Entry : Widgets)
			{
				ActiveData.ExtensionHandles.Add(ExtensionSubsystem->RegisterExtensionAsWidgetForContext(Entry.SlotID, LocalPlayer, Entry.WidgetClass.Get(), -1));
			}
		}
	}
}

void UGameFeatureAction_AddWidgets::RemoveWidgets(AActor* Actor, FPerContextData& ActiveData)
{
	if (auto* HUD{ Cast<AGFCHUD>(Actor) })
	{
		for (const auto& AddedLayout : ActiveData.LayoutsAdded)
		{
			if (AddedLayout.IsValid())
			{
				AddedLayout->DeactivateWidget();
			}
		}

		ActiveData.LayoutsAdded.Reset();

		for (auto& Handle : ActiveData.ExtensionHandles)
		{
			Handle.Unregister();
		}

		ActiveData.ExtensionHandles.Reset();
	}
}

#undef LOCTEXT_NAMESPACE
