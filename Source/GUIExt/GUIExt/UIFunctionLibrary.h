// Copyright (C) 2024 owoDra

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "CommonInputTypeEnum.h"
#include "GameplayTagContainer.h"

#include "UIFunctionLibrary.generated.h"

class ULocalPlayer;
class APlayerController;
class UUserWidget;
class UCommonActivatableWidget;


UCLASS()
class GUIEXT_API UUIFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	static int32 InputSuspensions;

public:
	UUIFunctionLibrary() {}
	
public:
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "UI Function", meta = (WorldContext = "WidgetContextObject"))
	static ECommonInputType GetOwningPlayerInputType(const UUserWidget* WidgetContextObject);
	
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "UI Function", meta = (WorldContext = "WidgetContextObject"))
	static bool IsOwningPlayerUsingTouch(const UUserWidget* WidgetContextObject);

	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "UI Function", meta = (WorldContext = "WidgetContextObject"))
	static bool IsOwningPlayerUsingGamepad(const UUserWidget* WidgetContextObject);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Function", meta = (GameplayTagFilter = "UI.Layer"))
	static UCommonActivatableWidget* PushContentToLayer_ForPlayer(const ULocalPlayer* LocalPlayer, FGameplayTag LayerName, TSubclassOf<UCommonActivatableWidget> WidgetClass);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Function", meta = (GameplayTagFilter = "UI.Layer"))
	static void PushStreamedContentToLayer_ForPlayer(const ULocalPlayer* LocalPlayer, FGameplayTag LayerName, TSoftClassPtr<UCommonActivatableWidget> WidgetClass);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Function")
	static void PopContentFromLayer(UCommonActivatableWidget* ActivatableWidget);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Function")
	static ULocalPlayer* GetLocalPlayerFromController(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Function")
	static FName SuspendInputForPlayer(APlayerController* PlayerController, FName SuspendReason);
	static FName SuspendInputForPlayer(ULocalPlayer* LocalPlayer, FName SuspendReason);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Function")
	static void ResumeInputForPlayer(APlayerController* PlayerController, FName SuspendToken);
	static void ResumeInputForPlayer(ULocalPlayer* LocalPlayer, FName SuspendToken);

};
