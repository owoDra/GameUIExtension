// Copyright (C) 2024 owoDra

#pragma once

#include "UObject/Interface.h"

#include "LoadingProcessInterface.generated.h"


/** 
 * Interface for classes that perform processing that requires displaying the load screen
 */
UINTERFACE(BlueprintType)
class GUIEXT_API ULoadingProcessInterface : public UInterface
{
public:
	GENERATED_BODY()

};

class GUIEXT_API ILoadingProcessInterface
{
public:
	GENERATED_BODY()

public:
	/**
	 * Checks if ILoadingProcessInterface is implemented and returns whether the loading screen needs to be displayed
	 */
	static bool ShouldShowLoadingScreen(UObject* TestObject, FString& OutReason);

	/**
	 * Returns whether the load screen should be displayed.
	 */
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const { return false; }

};
