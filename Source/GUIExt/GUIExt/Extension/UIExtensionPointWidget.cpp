// Copyright (C) 2024 owoDra

#include "UIExtensionPointWidget.h"

#include "Extension/UIExtensionPointSubsystem.h"
#include "GUIExtLogs.h"

#include "Editor/WidgetCompilerLog.h"
#include "Misc/UObjectToken.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"
#include "GameFramework/PlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UIExtensionPointWidget)


#define LOCTEXT_NAMESPACE "UIExtension"

UUIExtensionPointWidget::UUIExtensionPointWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}


#if WITH_EDITOR
void UUIExtensionPointWidget::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	// We don't care if the CDO doesn't have a specific tag.

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		if (!ExtensionPointTag.IsValid())
		{
			auto Message
			{ 
				CompileLog.Error(FText::Format(LOCTEXT("UUIExtensionPointWidget_NoTag", "{0} has no ExtensionPointTag specified - All extension points must specify a tag so they can be located."), FText::FromString(GetName()))) 
			};

			Message->AddToken(FUObjectToken::Create(this));
		}
	}
}
#endif


void UUIExtensionPointWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	ResetExtensionPoint();

	Super::ReleaseSlateResources(bReleaseChildren);
}

TSharedRef<SWidget> UUIExtensionPointWidget::RebuildWidget()
{
	if (!IsDesignTime() && ExtensionPointTag.IsValid())
	{
		ResetExtensionPoint();
		RegisterExtensionPoint();
	}

	if (IsDesignTime())
	{
		auto GetExtensionPointText
		{ 
			[this]()
			{
				return FText::Format(LOCTEXT("DesignTime_ExtensionPointLabel", "Extension Point\n{0}"), FText::FromName(ExtensionPointTag.GetTagName()));
			}
		};

		auto MessageBox{ SNew(SOverlay) };
		MessageBox->AddSlot()
			.Padding(5.0f)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Justification(ETextJustify::Center)
				.Text_Lambda(GetExtensionPointText)
			];

		return MessageBox;
	}
	else
	{
		return Super::RebuildWidget();
	}
}


void UUIExtensionPointWidget::ResetExtensionPoint()
{
	ResetInternal();

	ExtensionMapping.Reset();

	for (auto& Handle : ExtensionPointHandles)
	{
		Handle.Unregister();
	}

	ExtensionPointHandles.Reset();
}

void UUIExtensionPointWidget::RegisterExtensionPoint()
{
	if (auto* ExtensionSubsystem{ GetWorld()->GetSubsystem<UUIExtensionPointSubsystem>() })
	{
		TArray<UClass*> AllowedDataClasses;
		AllowedDataClasses.Add(UUserWidget::StaticClass());
		AllowedDataClasses.Append(DataClasses);

		ExtensionPointHandles.Add(
			ExtensionSubsystem->RegisterExtensionPoint(
				ExtensionPointTag, 
				ExtensionPointTagMatch, 
				AllowedDataClasses,
				FUIExtensionActionDelegate::CreateUObject(this, &ThisClass::OnAddOrRemoveExtension)
			)
		);

		ExtensionPointHandles.Add(
			ExtensionSubsystem->RegisterExtensionPointForContext(
				ExtensionPointTag, 
				GetOwningLocalPlayer(), 
				ExtensionPointTagMatch, 
				AllowedDataClasses,
				FUIExtensionActionDelegate::CreateUObject(this, &ThisClass::OnAddOrRemoveExtension)
			)
		);
	}
}

void UUIExtensionPointWidget::OnAddOrRemoveExtension(EUIExtensionAction Action, const FUIExtensionRequest& Request)
{
	if (Action == EUIExtensionAction::Added)
	{
		auto Data{ Request.Data };
		auto WidgetClass{ TSubclassOf<UUserWidget>(Cast<UClass>(Data)) };
		
		if (WidgetClass)
		{
			auto* Widget{ CreateEntryInternal(WidgetClass) };

			ExtensionMapping.Add(Request.ExtensionHandle, Widget);
		}
		else if (DataClasses.Num() > 0)
		{
			if (GetWidgetClassForData.IsBound())
			{
				WidgetClass = GetWidgetClassForData.Execute(Data);

				// If the data is irrelevant they can just return no widget class.

				if (WidgetClass)
				{
					if (auto* Widget{ CreateEntryInternal(WidgetClass) })
					{
						ExtensionMapping.Add(Request.ExtensionHandle, Widget);

						ConfigureWidgetForData.ExecuteIfBound(Widget, Data);
					}
				}
			}
		}
	}
	else
	{
		if (auto Extension{ ExtensionMapping.FindRef(Request.ExtensionHandle) })
		{
			RemoveEntryInternal(Extension);

			ExtensionMapping.Remove(Request.ExtensionHandle);
		}
	}
}

#undef LOCTEXT_NAMESPACE
