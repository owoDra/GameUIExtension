// Copyright (C) 2023 owoDra

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


	///////////////////////////////////////////////
	// Dialog
public:
	UPROPERTY(Config, EditAnywhere, Category = "Dialog", meta = (MetaClass = "/Script/GUIExt.UIDialog"))
	FSoftClassPath ConfirmationDialogClass;

	UPROPERTY(Config, EditAnywhere, Category = "Dialog", meta = (MetaClass = "/Script/GUIExt.UIDialog"))
	FSoftClassPath ErrorDialogClass;


	///////////////////////////////////////////////
	// LoadingScreen
public:
	//
	// Class of widget to be used for loading screen
	//
	UPROPERTY(Config, EditAnywhere, Category = "LoadingScreen", meta = (MetaClass = "/Script/UMG.UserWidget"))
	FSoftClassPath LoadingScreenWidgetClass;

	//
	// Display priority when displaying the load screen
	// 
	// Tips:
	//	The larger the size, the more it is displayed in the front
	//
	UPROPERTY(Config, EditAnywhere, Category = "LoadingScreen")
	int32 LoadingScreenZOrder{100};

	//
	// Number of additional seconds to display the loading screen after the actual loading is complete
	// 
	// Tips:
	//	By keeping the loading screen displayed for a while even after loading is complete, 
	//	you can avoid blurred textures in texture streaming, etc.
	//
	UPROPERTY(Config, EditAnywhere, Category = "LoadingScreen", meta = (ForceUnits = "s"))
	float HoldLoadingScreenAdditionalSecs{ 2.0f };

	//
	// After the actual loading is completed in the test play in the editor, do you want to show an additional loading screen?
	//
	UPROPERTY(Config, EditAnywhere, Category = "LoadingScreen|Debug")
	bool bShouldHoldLoadingScreenAdditionalSecsInEditor{ false };

	//
	// Forcing a Tick when the load screen is displayed during test play in the editor?
	//
	UPROPERTY(Config, EditAnywhere, Category= "LoadingScreen|Debug")
	bool bForceTickLoadingScreenInEditor{ true };

};

