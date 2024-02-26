// Copyright (C) 2024 owoDra

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameplayTagContainer.h"

#include "UIExtensionPointTypes.generated.h"

class UUIExtensionPointSubsystem;


/**
 * Match rule for extension points
 */
UENUM(BlueprintType)
enum class EUIExtensionPointMatch : uint8
{
	// An exact match will only receive extensions with exactly the same point
	// (e.g., registering for "A.B" will match a broadcast of A.B but not A.B.C)
	ExactMatch,

	// A partial match will receive any extensions rooted in the same point
	// (e.g., registering for "A.B" will match a broadcast of A.B as well as A.B.C)
	PartialMatch
};


/**
 * Action for extension points
 */
UENUM(BlueprintType)
enum class EUIExtensionAction : uint8
{
	Added,

	Removed
};


/**
 * Delegate to notify action on UIExtensionPoint
 */
DECLARE_DELEGATE_TwoParams(FUIExtensionActionDelegate, EUIExtensionAction Action, const FUIExtensionRequest& Request);


/**
 * Data of what has been added to the UIExtensionPoint
 */
struct FUIExtension : TSharedFromThis<FUIExtension>
{
public:
	FGameplayTag ExtensionPointTag;

	int32 Priority{ INDEX_NONE };

	TWeakObjectPtr<UObject> ContextObject{ nullptr };

	TObjectPtr<UObject> Data{ nullptr };

};


/**
 * Data of the UIExtensionPoint itself
 */
struct FUIExtensionPoint : TSharedFromThis<FUIExtensionPoint>
{
public:
	FGameplayTag ExtensionPointTag;

	TWeakObjectPtr<UObject> ContextObject{ nullptr };

	EUIExtensionPointMatch ExtensionPointTagMatchType{ EUIExtensionPointMatch::ExactMatch };

	TArray<TObjectPtr<UClass>> AllowedDataClasses;

	FUIExtensionActionDelegate Callback;

public:
	/**
	 * Tests if the extension and the extension point match up, if they do then this extension point should learn about this extension.
	 */
	bool DoesExtensionPassContract(const FUIExtension* Extension) const;

};


/**
 * Handle data to manage UIExtensionPoint
 */
USTRUCT(BlueprintType)
struct GUIEXT_API FUIExtensionPointHandle
{
	GENERATED_BODY()

	friend class UUIExtensionPointSubsystem;

public:
	FUIExtensionPointHandle() {}

	FUIExtensionPointHandle(UUIExtensionPointSubsystem* InExtensionSource, const TSharedPtr<FUIExtensionPoint>& InDataPtr) 
		: ExtensionSource(InExtensionSource)
		, DataPtr(InDataPtr)
	{}

private:
	TWeakObjectPtr<UUIExtensionPointSubsystem> ExtensionSource;

	TSharedPtr<FUIExtensionPoint> DataPtr;


public:
	void Unregister();

	bool IsValid() const { return DataPtr.IsValid(); }

	bool operator==(const FUIExtensionPointHandle& Other) const { return DataPtr == Other.DataPtr; }
	bool operator!=(const FUIExtensionPointHandle& Other) const { return !operator==(Other); }

	friend uint32 GetTypeHash(const FUIExtensionPointHandle& Handle) { return PointerHash(Handle.DataPtr.Get()); }

};

template<>
struct TStructOpsTypeTraits<FUIExtensionPointHandle> : public TStructOpsTypeTraitsBase2<FUIExtensionPointHandle>
{
	enum
	{
		WithCopy = true,  // This ensures the opaque type is copied correctly in BPs
		WithIdenticalViaEquality = true,
	};
};


/**
 * Handle data to manage what is added to the UIExtensionPoint
 */
USTRUCT(BlueprintType)
struct GUIEXT_API FUIExtensionHandle
{
	GENERATED_BODY()

	friend class UUIExtensionPointSubsystem;

public:
	FUIExtensionHandle() {}

	FUIExtensionHandle(UUIExtensionPointSubsystem* InExtensionSource, const TSharedPtr<FUIExtension>& InDataPtr) 
		: ExtensionSource(InExtensionSource)
		, DataPtr(InDataPtr)
	{}

private:
	TWeakObjectPtr<UUIExtensionPointSubsystem> ExtensionSource;

	TSharedPtr<FUIExtension> DataPtr;

public:
	void Unregister();

	bool IsValid() const { return DataPtr.IsValid(); }

	bool operator==(const FUIExtensionHandle& Other) const { return DataPtr == Other.DataPtr; }
	bool operator!=(const FUIExtensionHandle& Other) const { return !operator==(Other); }

	friend uint32 GetTypeHash(FUIExtensionHandle Handle) { return PointerHash(Handle.DataPtr.Get()); }

};

template<>
struct TStructOpsTypeTraits<FUIExtensionHandle> : public TStructOpsTypeTraitsBase2<FUIExtensionHandle>
{
	enum
	{
		WithCopy = true,  // This ensures the opaque type is copied correctly in BPs
		WithIdenticalViaEquality = true,
	};
};


/**
 * Request data for UIExtensionPoint
 */
USTRUCT(BlueprintType)
struct FUIExtensionRequest
{
	GENERATED_BODY()
public:
	FUIExtensionRequest() {}

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FUIExtensionHandle ExtensionHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag ExtensionPointTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Priority{ INDEX_NONE };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UObject> Data{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UObject> ContextObject{ nullptr };
};


/**
 * Blueprint function library for UIExtensionHandle
 */
UCLASS()
class GUIEXT_API UUIExtensionHandleFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UUIExtensionHandleFunctions() {}

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
	static void Unregister( UPARAM(ref) FUIExtensionHandle& Handle);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
	static bool IsValid(const FUIExtensionHandle& Handle);

};


/**
 * Blueprint function library for UIExtensionPointHandle
 */
UCLASS()
class GUIEXT_API UUIExtensionPointHandleFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UUIExtensionPointHandleFunctions() {}

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
	static void Unregister( UPARAM(ref) FUIExtensionPointHandle& Handle);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
	static bool IsValid(const FUIExtensionPointHandle& Handle);

};
