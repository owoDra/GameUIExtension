// Copyright (C) 2024 owoDra

#pragma once

#include "Modules/ModuleManager.h"

class FPreLoadScreen;

/**
 * Module for load screen functionality
 */
class FGUIExtLoadingScreenModule : public IModuleInterface
{
protected:
	typedef FGUIExtLoadingScreenModule ThisClass;

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	bool IsGameModule() const override;

private:
	/**
	 *  Run when PreLoadingScreenManager is destroyed
	 */
	void OnPreLoadingScreenManagerCleanUp();

private:
	//
	// Reference to the class that manages the load screen
	//
	TSharedPtr<FPreLoadScreen> PreLoadScreen;

};
