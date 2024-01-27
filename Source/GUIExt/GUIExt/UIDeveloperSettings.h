// Copyright (C) 2024 owoDra

#pragma once

#include "Engine/DeveloperSettings.h"

#include "UIDeveloperSettings.generated.h"


/**
 * Settings for a loading screen system.
 */
UCLASS(Config = "Game", Defaultconfig, meta = (DisplayName = "Game UI Extension"))
class UUIDeveloperSettings : public UDeveloperSettings
{
public:
	GENERATED_BODY()
public:
	UUIDeveloperSettings();

	///////////////////////////////////////////////
	// General
public:
	UPROPERTY(Config, EditAnywhere, Category = "General", meta = (MetaClass = "/Script/GUIExt.UIPolicy"))
	FSoftClassPath DefaultUIPolicyClass;

};

