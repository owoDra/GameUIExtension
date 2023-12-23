// Copyright (C) 2023 owoDra

#include "TaggedWidget.h"

#include "CommonUISettings.h"
#include "ICommonUIModule.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TaggedWidget)


UTaggedWidget::UTaggedWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UTaggedWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!IsDesignTime())
	{
		if(!bWantsToBeVisible)
		{
			const auto& PlatformTraits{ ICommonUIModule::GetSettings().GetPlatformTraits() };
			const auto bShouldBeHidden{ PlatformTraits.HasAny(HiddenByTags) };

			SetVisibility(bShouldBeHidden ? HiddenVisibility : ShownVisibility);
		}
		else
		{
			SetVisibility(ShownVisibility);
		}
	}
}
