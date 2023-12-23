// Copyright (C) 2023 owoDra

#pragma once

#include "UIPolicy.generated.h"

class ULocalPlayer;
class UUIManagerSubsystem;
class UUILayout;


/**
 * How the UI behaves towards other players in multiplayer.
 */
UENUM()
enum class EUIMultiplayerInteractionMode : uint8
{
	PrimaryOnly,	// Fullscreen viewport for the primary player only, regardless of the other player's existence
	
	SingleToggle,	// Fullscreen viewport for one player, but players can swap control over who's is displayed and who's is dormant
	
	Simultaneous	// Viewports displayed simultaneously for both players
};


/**
 * Display information for the root portion of the UI layout
 */
USTRUCT()
struct FRootViewportLayoutInfo
{
	GENERATED_BODY()
public:
	FRootViewportLayoutInfo() {}

	FRootViewportLayoutInfo(ULocalPlayer* InLocalPlayer, UUILayout* InRootLayout, bool bIsInViewport)
		: LocalPlayer(InLocalPlayer)
		, RootLayout(InRootLayout)
		, bAddedToViewport(bIsInViewport)
	{}

public:
	UPROPERTY(Transient)
	TObjectPtr<ULocalPlayer> LocalPlayer{ nullptr };

	UPROPERTY(Transient)
	TObjectPtr<UUILayout> RootLayout{ nullptr };

	UPROPERTY(Transient)
	bool bAddedToViewport{ false };

public:
	bool operator==(const ULocalPlayer* OtherLocalPlayer) const { return LocalPlayer == OtherLocalPlayer; }

};


/**
 * Display information for the root portion of the UI layout
 */
UCLASS(Abstract, Blueprintable, Within = "UIManagerSubsystem")
class GUIEXT_API UUIPolicy : public UObject
{
	GENERATED_BODY()

	friend class UUIManagerSubsystem;

public:
	UUIPolicy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UPROPERTY(EditAnywhere)
	EUIMultiplayerInteractionMode MultiplayerInteractionMode{ EUIMultiplayerInteractionMode::PrimaryOnly };

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UUILayout> LayoutClass{ nullptr };

	UPROPERTY(Transient)
	TArray<FRootViewportLayoutInfo> RootViewportLayouts;

protected:
	void AddLayoutToViewport(ULocalPlayer* LocalPlayer, UUILayout* Layout);
	void RemoveLayoutFromViewport(ULocalPlayer* LocalPlayer, UUILayout* Layout);

	virtual void OnRootLayoutAddedToViewport(ULocalPlayer* LocalPlayer, UUILayout* Layout);
	virtual void OnRootLayoutRemovedFromViewport(ULocalPlayer* LocalPlayer, UUILayout* Layout);
	virtual void OnRootLayoutReleased(ULocalPlayer* LocalPlayer, UUILayout* Layout);

	void CreateLayoutWidget(ULocalPlayer* LocalPlayer);

	void NotifyPlayerAdded(ULocalPlayer* LocalPlayer);
	void NotifyPlayerRemoved(ULocalPlayer* LocalPlayer);
	void NotifyPlayerDestroyed(ULocalPlayer* LocalPlayer);

public:
	void RequestPrimaryControl(UUILayout* Layout);


protected:
	TSubclassOf<UUILayout> GetLayoutWidgetClass(ULocalPlayer* LocalPlayer);

public:
	virtual UWorld* GetWorld() const override;
	UUIManagerSubsystem* GetOwningUIManager() const;
	UUILayout* GetRootLayout(const ULocalPlayer* LocalPlayer) const;

	EUIMultiplayerInteractionMode GetLocalMultiplayerInteractionMode() const { return MultiplayerInteractionMode; }

public:
	static UUIPolicy* GetUIPolicy(const UObject* WorldContextObject);

	template <typename T = UUIPolicy>
	static T* GetUIPolicyAs(const UObject* WorldContextObject)
	{
		return Cast<T>(GetUIPolicy(WorldContextObject));
	}

};
