// Copyright (C) 2024 owoDra

#include "PawnWidget.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PawnWidget)


UPawnWidget::UPawnWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UPawnWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ListenPawnChange();
}

void UPawnWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GetCachedPawn();
}

void UPawnWidget::NativeDestruct()
{
	UnlistenPawnChange();

	Super::NativeDestruct();
}


APawn* UPawnWidget::GetCachedPawn()
{
	if (!OwningPawn.IsValid())
	{
		HandlePawnChange(nullptr, GetOwningPlayerPawn());
	}

	return OwningPawn.Get();
}

void UPawnWidget::ListenPawnChange()
{
	auto* PC{ GetOwningPlayer() };
	check(PC);

	PC->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::HandlePawnChange);
}

void UPawnWidget::UnlistenPawnChange()
{
	if (auto* PC{ GetOwningPlayer() })
	{
		PC->OnPossessedPawnChanged.RemoveDynamic(this, &ThisClass::HandlePawnChange);
	}
}

void UPawnWidget::HandlePawnChange(APawn* OldPawn, APawn* NewPawn)
{
	OwningPawn = NewPawn;

	OnPawnChanged(OldPawn, NewPawn);
}
