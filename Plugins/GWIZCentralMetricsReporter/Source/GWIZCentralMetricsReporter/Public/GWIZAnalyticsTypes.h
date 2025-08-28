#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "GWIZAnalyticsTypes.generated.h"

/**
 * Flexible data structure for storing various types of analytics data
 * Similar to JSON structure but optimized for Unreal Engine
 */
USTRUCT(BlueprintType)
struct FGWIZFlexibleData
{
	GENERATED_BODY()

public:
	// Data type enumeration
	UENUM(BlueprintType)
	enum class EDataType : uint8
	{
		None,
		String,
		Float,
		Int,
		Int64,
		Bool,
		Array,
		Nested
	};

	// Constructor
	FGWIZFlexibleData() : DataType(EDataType::None) {}

	// Data storage
	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	EDataType DataType;

	// String data
	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	FString StringValue;

	// Numeric data
	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	float FloatValue;

	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	int32 IntValue;

	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	int64 Int64Value;

	// Boolean data
	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	bool BoolValue;

	// Array data
	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	TArray<FGWIZFlexibleData> ArrayValue;

	// Nested data
	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	TMap<FString, FGWIZFlexibleData> NestedValue;

	// Setter methods
	void SetString(const FString& Value)
	{
		DataType = EDataType::String;
		StringValue = Value;
	}

	void SetFloat(float Value)
	{
		DataType = EDataType::Float;
		FloatValue = Value;
	}

	void SetInt(int32 Value)
	{
		DataType = EDataType::Int;
		IntValue = Value;
	}

	void SetInt64(int64 Value)
	{
		DataType = EDataType::Int64;
		Int64Value = Value;
	}

	void SetBool(bool Value)
	{
		DataType = EDataType::Bool;
		BoolValue = Value;
	}

	void SetArray(const TArray<FGWIZFlexibleData>& Value)
	{
		DataType = EDataType::Array;
		ArrayValue = Value;
	}

	void SetNested(const TMap<FString, FGWIZFlexibleData>& Value)
	{
		DataType = EDataType::Nested;
		NestedValue = Value;
	}

	// Getter methods
	FString GetString() const { return StringValue; }
	float GetFloat() const { return FloatValue; }
	int32 GetInt() const { return IntValue; }
	int64 GetInt64() const { return Int64Value; }
	bool GetBool() const { return BoolValue; }
	TArray<FGWIZFlexibleData> GetArray() const { return ArrayValue; }
	TMap<FString, FGWIZFlexibleData> GetNested() const { return NestedValue; }

	// Helper method to set nested data by key
	void SetNestedValue(const FString& Key, const FGWIZFlexibleData& Value)
	{
		if (DataType != EDataType::Nested)
		{
			DataType = EDataType::Nested;
			NestedValue.Empty();
		}
		NestedValue.Add(Key, Value);
	}

	// Helper method to get nested data by key
	FGWIZFlexibleData GetNestedValue(const FString& Key) const
	{
		if (DataType == EDataType::Nested && NestedValue.Contains(Key))
		{
			return NestedValue[Key];
		}
		return FGWIZFlexibleData();
	}
};

/**
 * Static session data that doesn't change during a session
 * Collected once per session for efficiency
 */
USTRUCT(BlueprintType)
struct FGWIZStaticSessionData
{
	GENERATED_BODY()

public:
	// Session identification
	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	FString SessionID;

	// Platform information
	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	FString Platform;

	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	FString DistributionPlatform;

	// Hardware information
	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	FString CPUInfo;

	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	FString GPUInfo;

	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	int32 SystemMemoryMB;

	// Engine and build information
	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	FString EngineVersion;

	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	FString BuildVersion;

	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	FString BuildConfiguration;

	// Session start time
	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	FDateTime SessionStartTime;

	// Constructor
	FGWIZStaticSessionData()
		: SystemMemoryMB(0)
		, SessionStartTime(FDateTime::Now())
	{
	}
};

/**
 * Dynamic event data for individual analytics events
 * Contains only event-specific data to avoid duplication
 */
USTRUCT(BlueprintType)
struct FGWIZEventData
{
	GENERATED_BODY()

public:
	// Event identification
	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	FString EventID;

	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	FString EventType;

	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	FString SystemName;

	// Timestamp
	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	FDateTime Timestamp;

	// Event-specific data
	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	FGWIZFlexibleData Data;

	// Constructor
	FGWIZEventData()
		: Timestamp(FDateTime::Now())
	{
		// Generate unique event ID
		EventID = FGuid::NewGuid().ToString();
	}
};

/**
 * Export configuration for external systems
 */
USTRUCT(BlueprintType)
struct FGWIZExportConfig
{
	GENERATED_BODY()

public:
	// File export settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export")
	bool bEnableFileExport = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export")
	FString FileExportPath = TEXT("Saved/Analytics/");

	// HTTP/API export settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export")
	bool bEnableHTTPExport = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export")
	FString HTTPEndpoint = TEXT("https://your-metrics-server.com/api/analytics");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export")
	TMap<FString, FString> HTTPHeaders;

	// Batch processing settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export")
	int32 BatchSize = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export")
	float ExportInterval = 5.0f; // seconds

	// Retry logic settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export")
	int32 MaxRetries = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export")
	float RetryDelay = 1.0f; // seconds

	// Offline caching settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export")
	bool bEnableOfflineCaching = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export")
	int32 MaxCachedEvents = 1000;
};
