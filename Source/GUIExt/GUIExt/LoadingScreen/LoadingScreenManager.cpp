// Copyright (C) 2023 owoDra

#include "LoadingScreenManager.h"

#include "LoadingScreen/LoadingProcessInterface.h"
#include "UIDeveloperSettings.h"
#include "GUIExtLogs.h"

#include "HAL/ThreadHeartBeat.h"
#include "Misc/CommandLine.h"
#include "Misc/ConfigCacheIni.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Engine.h"
#include "GameFramework/WorldSettings.h"
#include "Framework/Application/IInputProcessor.h"
#include "Framework/Application/SlateApplication.h"
#include "PreLoadScreen.h"
#include "PreLoadScreenManager.h"
#include "ShaderPipelineCache.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/Images/SThrobber.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LoadingScreenManager)


void ULoadingScreenManager::Initialize(FSubsystemCollectionBase& Collection)
{
	FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &ThisClass::HandlePreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::HandlePostLoadMap);
}

void ULoadingScreenManager::Deinitialize()
{
	SetBlockingInput(/*bBlock =*/ false);

	RemoveWidgetFromViewport();

	FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
}

void ULoadingScreenManager::RegisterLoadingProcessor(TScriptInterface<ILoadingProcessInterface> Interface)
{
	LoadingProcessors.Add(Interface.GetObject());
}

void ULoadingScreenManager::UnregisterLoadingProcessor(TScriptInterface<ILoadingProcessInterface> Interface)
{
	LoadingProcessors.Remove(Interface.GetObject());
}


void ULoadingScreenManager::Tick(float DeltaTime)
{
	UpdateLoadingScreen();
}

ETickableTickType ULoadingScreenManager::GetTickableTickType() const
{
	return ETickableTickType::Conditional;
}

bool ULoadingScreenManager::IsTickable() const
{
	return !HasAnyFlags(RF_ClassDefaultObject);
}

TStatId ULoadingScreenManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(ULoadingScreenManager, STATGROUP_Tickables);
}

UWorld* ULoadingScreenManager::GetTickableGameObjectWorld() const
{
	return GetGameInstance()->GetWorld();
}


void ULoadingScreenManager::HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName)
{
	if (WorldContext.OwningGameInstance == GetGameInstance())
	{
		bCurrentlyInLoadMap = true;

		if (GEngine->IsInitialized())
		{
			UpdateLoadingScreen();
		}
	}
}

void ULoadingScreenManager::HandlePostLoadMap(UWorld* World)
{
	if (World && (World->GetGameInstance() == GetGameInstance()))
	{
		bCurrentlyInLoadMap = false;
	}
}

void ULoadingScreenManager::UpdateLoadingScreen()
{
	if (ShouldShowLoadingScreen())
	{
		ShowLoadingScreen();
	}
	else
	{
		HideLoadingScreen();
	}
}

bool ULoadingScreenManager::NeedToShowLoadingScreen()
{
	ShowLoadingScreenReason = FString(TEXT("Unknown"));

	const auto* LocalGameInstance{ GetGameInstance() };

	// Display load screen while WorldContext is disabled

	const auto* Context{ LocalGameInstance->GetWorldContext() };
	if (!Context)
	{
		ShowLoadingScreenReason = FString(TEXT("WorldContext owned by GameInstance is nullptr"));
		return true;
	}

	// Show load screen while World is disabled

	auto* World{ Context->World() };
	if (!World)
	{
		ShowLoadingScreenReason = FString(TEXT("No valid World"));
		return true;
	}

	// Show loading screen during map loading.

	if (bCurrentlyInLoadMap)
	{
		ShowLoadingScreenReason = FString(TEXT("The map is loading"));
		return true;
	}

	// Show loading screen during map movement.

	if (!Context->TravelURL.IsEmpty())
	{
		ShowLoadingScreenReason = FString(TEXT("Map is moving (Context->TravelURL is not empty)"));
		return true;
	}

	// The loading screen is displayed before the game starts.

	if (!World->HasBegunPlay())
	{
		ShowLoadingScreenReason = FString(TEXT("Before the game begins"));
		return true;
	}

	// Show loading screen during map seamless travel.

	if (World->IsInSeamlessTravel())
	{
		ShowLoadingScreenReason = FString(TEXT("The map is in seamless travel"));
		return true;
	}

	// Loading Displays loading screen if processor/actor is present

	for (const auto& Processor : LoadingProcessors)
	{
		if (ILoadingProcessInterface::ShouldShowLoadingScreen(Processor.GetObject(), ShowLoadingScreenReason))
		{
			return true;
		}
	}

	// ロード画面を表示する必要がない

	ShowLoadingScreenReason = TEXT("All loading is complete");
	return false;
}

bool ULoadingScreenManager::ShouldShowLoadingScreen()
{
	// Only when non-shipping: load screen is forced to be hidden if NoLoadingScreen is present in the startup argument

#if !UE_BUILD_SHIPPING
	static auto bCmdLineNoLoadingScreen{ FParse::Param(FCommandLine::Get(), TEXT("NoLoadingScreen")) };
	if (bCmdLineNoLoadingScreen)
	{
		ShowLoadingScreenReason = FString(TEXT("'NoLoadingScreen' as startup argument"));
		return false;
	}
#endif

	// If a load screen is required, display the load screen.

	if (NeedToShowLoadingScreen())
	{
		TimeLoadingScreenLastDismissed = -1.0;
		return true;
	}

	// If the load screen is no longer needed, but only the display of the load screen is to be extended

	const auto* Settings{ GetDefault<UUIDeveloperSettings>() };	
	const auto CurrentTime{ FPlatformTime::Seconds() };
	const auto bCanHoldLoadingScreen{ !GIsEditor || Settings->bShouldHoldLoadingScreenAdditionalSecsInEditor };
	const auto HoldLoadingScreenAdditionalSecs{ bCanHoldLoadingScreen ? Settings->HoldLoadingScreenAdditionalSecs : 0.0 };

	if (TimeLoadingScreenLastDismissed < 0.0)
	{
		TimeLoadingScreenLastDismissed = CurrentTime;
	}
	const auto TimeSinceScreenDismissed{ CurrentTime - TimeLoadingScreenLastDismissed };

	if ((HoldLoadingScreenAdditionalSecs > 0.0) && (TimeSinceScreenDismissed < HoldLoadingScreenAdditionalSecs))
	{
		// Disable world rendering 

		auto* GameViewportClient{ GetGameInstance()->GetGameViewportClient() };
		GameViewportClient->bDisableWorldRendering = false;

		ShowLoadingScreenReason = FString::Printf(TEXT("Additional display for texture streaming after loading is complete (%.2f sec)"), HoldLoadingScreenAdditionalSecs);
		return true;
	}

	return false;
}

bool ULoadingScreenManager::IsShowingInitialLoadingScreen() const
{
	auto* PreLoadScreenManager{ FPreLoadScreenManager::Get() };
	return PreLoadScreenManager && PreLoadScreenManager->HasValidActivePreLoadScreen();
}

void ULoadingScreenManager::ShowLoadingScreen()
{
	// If the loading screen is already displayed, do nothing.

	if (bCurrentlyShowingLoadingScreen)
	{
		return;
	}

	// Normal loading screen cannot be displayed during the loading phase of the game engine

	auto* PreLoadScreenManager{ FPreLoadScreenManager::Get() };
	if (PreLoadScreenManager && PreLoadScreenManager->HasActivePreLoadScreenType(EPreLoadScreenTypes::EngineLoadingScreen))
	{
		return;
	}

	UE_LOG(LogGUIE, Log, TEXT("Load screen displayed (Reason: %s)"), *GetShowLoadingScreenReason());

	// Record load screen display start time and set displayed flag to true

	TimeLoadingScreenShown = FPlatformTime::Seconds();
	bCurrentlyShowingLoadingScreen = true;

	if (!IsShowingInitialLoadingScreen())
	{
		auto* LocalGameInstance{ GetGameInstance() };

		// Disable Input

		SetBlockingInput(/*bBlock =*/ true);

		// Notify the display of the loading screen.

		LoadingScreenVisibilityChanged.Broadcast(/*bIsVisible =*/ true);

		// Creating Widgets

		const auto* Settings{ GetDefault<UUIDeveloperSettings>() };
		auto WidgetClass{ Settings->LoadingScreenWidgetClass.TryLoadClass<UUserWidget>() };
		if (auto* Widget{ UUserWidget::CreateWidgetInstance(*LocalGameInstance, WidgetClass, NAME_None) })
		{
			LoadingScreenWidget = Widget->TakeWidget();
		}
		else
		{
			UE_LOG(LogGUIE, Error, TEXT("Failed to create loading screen widget, please check UIDeveloperSettings->LoadingScreenWidgetClass"));
			
			// Set alternative slate widget
			
			LoadingScreenWidget = SNew(SThrobber);
		}

		// Add widget to viewport

		auto* GameViewportClient{ LocalGameInstance->GetGameViewportClient() };
		GameViewportClient->AddViewportWidgetContent(LoadingScreenWidget.ToSharedRef(), Settings->LoadingScreenZOrder);

		// Change performance settings

		ChangePerformanceSettings(/*bEnableLoadingScreen =*/ true);

		// Perform Tick processing of slate

		if (!GIsEditor || Settings->bForceTickLoadingScreenInEditor)
		{
			FSlateApplication::Get().Tick();
		}
	}
}

void ULoadingScreenManager::HideLoadingScreen()
{
	// If the load screen is already hidden, do nothing.

	if (!bCurrentlyShowingLoadingScreen)
	{
		return;
	}

	UE_LOG(LogGUIE, Log, TEXT("Load screen is hidden (Reason: %s)"), *GetShowLoadingScreenReason());

	// Enable Input

	SetBlockingInput(/*bBlock =*/ false);

	if (!IsShowingInitialLoadingScreen())
	{
		// Remove load screen widget

		GEngine->ForceGarbageCollection(true);
		RemoveWidgetFromViewport();

		// Change performance settings
	
		ChangePerformanceSettings(/*bEnableLoadingScreen　=*/ false);

		// Notify the user to hide the loading screen.

		LoadingScreenVisibilityChanged.Broadcast(/*bIsVisible　=*/ false);
	}

	UE_LOG(LogGUIE, Log, TEXT("Loading screen display time: %.2f sec"), FPlatformTime::Seconds() - TimeLoadingScreenShown);

	// Set the displayed flag to false

	bCurrentlyShowingLoadingScreen = false;
}

void ULoadingScreenManager::RemoveWidgetFromViewport()
{
	if (LoadingScreenWidget.IsValid())
	{
		if (auto* GameViewportClient{ GetGameInstance()->GetGameViewportClient() })
		{
			GameViewportClient->RemoveViewportWidgetContent(LoadingScreenWidget.ToSharedRef());
		}
		LoadingScreenWidget.Reset();
	}
}

void ULoadingScreenManager::SetBlockingInput(bool bBlock)
{
	if (bBlock)
	{
		if (!InputPreProcessor.IsValid())
		{
			InputPreProcessor = MakeShareable<FLoadingScreenInputPreProcessor>(new FLoadingScreenInputPreProcessor());
			FSlateApplication::Get().RegisterInputPreProcessor(InputPreProcessor, 0);
		}
	}
	else
	{
		if (InputPreProcessor.IsValid())
		{
			FSlateApplication::Get().UnregisterInputPreProcessor(InputPreProcessor);
			InputPreProcessor.Reset();
		}
	}
}

void ULoadingScreenManager::ChangePerformanceSettings(bool bEnabingLoadingScreen)
{
	// Change shader batch mode

	FShaderPipelineCache::SetBatchMode(bEnabingLoadingScreen ? FShaderPipelineCache::BatchMode::Fast : FShaderPipelineCache::BatchMode::Background);

	// Disable world rendering

	auto* GameViewportClient{ GetGameInstance()->GetGameViewportClient() };
	GameViewportClient->bDisableWorldRendering = bEnabingLoadingScreen;

	// Change streaming priority within a level

	if (auto* ViewportWorld{ GameViewportClient->GetWorld() })
	{
		if (auto* WorldSettings{ ViewportWorld->GetWorldSettings(false, false) })
		{
			WorldSettings->bHighPriorityLoadingLocal = bEnabingLoadingScreen;
		}
	}

	// Obtain and apply HangDurationMultiplier from GConfig on the load screen

	if (bEnabingLoadingScreen)
	{
		auto HangDurationMultiplier{ 0.0 };
		if (!GConfig || !GConfig->GetDouble(TEXT("Core.System"), TEXT("LoadingScreenHangDurationMultiplier"), HangDurationMultiplier, GEngineIni))
		{
			HangDurationMultiplier = 1.0;
		}

		FThreadHeartBeat::Get().SetDurationMultiplier(HangDurationMultiplier);
		FGameThreadHitchHeartBeat::Get().SuspendHeartBeat();
	}

	// Restore the original settings

	else
	{
		FThreadHeartBeat::Get().SetDurationMultiplier(1.0);
		FGameThreadHitchHeartBeat::Get().ResumeHeartBeat();
	}
}
