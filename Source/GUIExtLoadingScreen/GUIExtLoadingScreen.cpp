// Copyright (C) 2023 owoDra

#include "GUIExtLoadingScreen.h"

#include "PreLoadScreen.h"

#include "Misc/App.h"
#include "PreLoadScreenManager.h"

IMPLEMENT_MODULE(FGUIExtLoadingScreenModule, GUIExtLoadingScreen)


void FGUIExtLoadingScreenModule::StartupModule()
{
	PreLoadScreen = MakeShared<FPreLoadScreen>();
	PreLoadScreen->Init();

	if (!GIsEditor && FApp::CanEverRender() && FPreLoadScreenManager::Get())
	{
		FPreLoadScreenManager::Get()->RegisterPreLoadScreen(PreLoadScreen);
		FPreLoadScreenManager::Get()->OnPreLoadScreenManagerCleanUp.AddRaw(this, &ThisClass::OnPreLoadingScreenManagerCleanUp);
	}
}

void FGUIExtLoadingScreenModule::ShutdownModule()
{
}

bool FGUIExtLoadingScreenModule::IsGameModule() const
{
	return true;
}

void FGUIExtLoadingScreenModule::OnPreLoadingScreenManagerCleanUp()
{
	PreLoadScreen.Reset();
	ShutdownModule();
}
