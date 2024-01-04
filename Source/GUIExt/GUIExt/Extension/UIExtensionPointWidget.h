// Copyright (C) 2023 owoDra

#pragma once

#include "Components/DynamicEntryBoxBase.h"

#include "Extension/UIExtensionPointTypes.h"

#include "UIExtensionPointWidget.generated.h"

class UGFCLocalPlayer;
class APlayerState;


/**
 * A slot that defines a location in a layout, where content can be added later
 */
UCLASS()
class GUIEXT_API UUIExtensionPointWidget : public UDynamicEntryBoxBase
{
	GENERATED_BODY()
public:
	UUIExtensionPointWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
#endif

public:
	DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(TSubclassOf<UUserWidget>, FGetWidgetClassForDataDelegate, UObject*, DataItem);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FConfigureWidgetForDataDelegate, UUserWidget*, Widget, UObject*, DataItem);

protected:
	//
	// The tag that defines this extension point
	//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Extension", meta = (Categories = "UI.Extension"))
	FGameplayTag ExtensionPointTag;

	//
	// How exactly does the extension need to match the extension point tag.
	//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Extension")
	EUIExtensionPointMatch ExtensionPointTagMatch{ EUIExtensionPointMatch::ExactMatch };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Extension")
	TArray<TObjectPtr<UClass>> DataClasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Extension", meta = (IsBindableEvent = "True"))
	FGetWidgetClassForDataDelegate GetWidgetClassForData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Extension", meta = (IsBindableEvent = "True"))
	FConfigureWidgetForDataDelegate ConfigureWidgetForData;

	TArray<FUIExtensionPointHandle> ExtensionPointHandles;

	UPROPERTY(Transient)
	TMap<FUIExtensionHandle, TObjectPtr<UUserWidget>> ExtensionMapping;

public:
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	void ResetExtensionPoint();
	void RegisterExtensionPoint();
	void OnAddOrRemoveExtension(EUIExtensionAction Action, const FUIExtensionRequest& Request);

};
