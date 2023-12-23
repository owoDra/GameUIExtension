// Copyright (C) 2023 owoDra

#include "PreLoadScreen.h"

#include "PreLoadingScreenSlateWidget.h"

#include "Misc/App.h"


void FPreLoadScreen::Init()
{
	if (!GIsEditor && FApp::CanEverRender())
	{
		EngineLoadingWidget = SNew(SPreLoadingScreenWidget);
	}
}
