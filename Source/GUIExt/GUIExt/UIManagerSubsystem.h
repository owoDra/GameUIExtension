// Copyright (C) 2024 owoDra

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"

#include "UIManagerSubsystem.generated.h"

class ULocalPlayer;
class UUIPolicy;


/**
 * Subsystems for managing the UI displayed to the user
 */
UCLASS()
class GUIEXT_API UUIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UUIManagerSubsystem() {}

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;


protected:
	FTSTicker::FDelegateHandle TickHandle;

protected:
	bool Tick(float DeltaTime);
	void SyncRootLayoutVisibilityToShowHUD();


protected:
	UPROPERTY(Transient)
	TObjectPtr<UUIPolicy> CurrentPolicy{ nullptr };

protected:
	void SwitchToPolicy(UUIPolicy* InPolicy);

public:
	UUIPolicy* GetCurrentUIPolicy() const { return CurrentPolicy; }

	virtual void NotifyPlayerAdded(ULocalPlayer* LocalPlayer);
	virtual void NotifyPlayerRemoved(ULocalPlayer* LocalPlayer);
	virtual void NotifyPlayerDestroyed(ULocalPlayer* LocalPlayer);

private:
	void HandleAddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId);
	void HanldeRemoveLocalPlayer(ULocalPlayer* ExistingPlayer);

};
