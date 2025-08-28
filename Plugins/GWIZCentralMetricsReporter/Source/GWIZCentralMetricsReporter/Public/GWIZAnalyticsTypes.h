#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "GWIZAnalyticsTypes.generated.h"

// Data type enumeration for flexible data
UENUM(BlueprintType)
enum class EGWIZDataType : uint8
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

/**
 * Simple data structure for basic analytics data
 * Can be used directly in UPROPERTY without recursion issues
 */
USTRUCT(BlueprintType)
struct FGWIZSimpleData
{
	GENERATED_BODY()

public:
	// Constructor
	FGWIZSimpleData() : DataType(EGWIZDataType::None) {}

	// Data storage
	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	EGWIZDataType DataType;

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

	// Setter methods
	void SetString(const FString& Value)
	{
		DataType = EGWIZDataType::String;
		StringValue = Value;
	}

	void SetFloat(float Value)
	{
		DataType = EGWIZDataType::Float;
		FloatValue = Value;
	}

	void SetInt(int32 Value)
	{
		DataType = EGWIZDataType::Int;
		IntValue = Value;
	}

	void SetInt64(int64 Value)
	{
		DataType = EGWIZDataType::Int64;
		Int64Value = Value;
	}

	void SetBool(bool Value)
	{
		DataType = EGWIZDataType::Bool;
		BoolValue = Value;
	}

	// Getter methods
	FString GetString() const { return StringValue; }
	float GetFloat() const { return FloatValue; }
	int32 GetInt() const { return IntValue; }
	int64 GetInt64() const { return Int64Value; }
	bool GetBool() const { return BoolValue; }
};

/**
 * Complex data structure for arrays and nested objects
 * Not used in UPROPERTY to avoid recursion, but provides full functionality
 */
struct FGWIZComplexData
{
public:
	// Array data
	TArray<FGWIZSimpleData> ArrayValue;
	
	// Nested data
	TMap<FString, FGWIZSimpleData> NestedValue;

	// Setter methods
	void SetArray(const TArray<FGWIZSimpleData>& Value)
	{
		ArrayValue = Value;
	}

	void SetNested(const TMap<FString, FGWIZSimpleData>& Value)
	{
		NestedValue = Value;
	}

	// Getter methods
	TArray<FGWIZSimpleData> GetArray() const { return ArrayValue; }
	TMap<FString, FGWIZSimpleData> GetNested() const { return NestedValue; }

	// Helper method to set nested data by key
	void SetNestedValue(const FString& Key, const FGWIZSimpleData& Value)
	{
		NestedValue.Add(Key, Value);
	}

	// Helper method to get nested data by key
	FGWIZSimpleData GetNestedValue(const FString& Key) const
	{
		if (NestedValue.Contains(Key))
		{
			return NestedValue[Key];
		}
		return FGWIZSimpleData();
	}

	// Array manipulation
	void AddArrayElement(const FGWIZSimpleData& Element)
	{
		ArrayValue.Add(Element);
	}

	FGWIZSimpleData GetArrayElement(int32 Index) const
	{
		if (ArrayValue.IsValidIndex(Index))
		{
			return ArrayValue[Index];
		}
		return FGWIZSimpleData();
	}

	int32 GetArraySize() const
	{
		return ArrayValue.Num();
	}
};

/**
 * Flexible data structure that combines simple and complex data
 * Uses simple data for UPROPERTY compatibility, complex data for advanced features
 */
USTRUCT(BlueprintType)
struct FGWIZFlexibleData
{
	GENERATED_BODY()

public:
	// Constructor
	FGWIZFlexibleData() : DataType(EGWIZDataType::None) {}

	// Data storage
	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	EGWIZDataType DataType;

	// Simple data (for UPROPERTY compatibility)
	UPROPERTY(BlueprintReadWrite, Category = "Analytics")
	FGWIZSimpleData SimpleData;

	// Complex data (not exposed to Blueprint, but available in C++)
	FGWIZComplexData ComplexData;

	// Setter methods for simple data
	void SetString(const FString& Value)
	{
		DataType = EGWIZDataType::String;
		SimpleData.SetString(Value);
	}

	void SetFloat(float Value)
	{
		DataType = EGWIZDataType::Float;
		SimpleData.SetFloat(Value);
	}

	void SetInt(int32 Value)
	{
		DataType = EGWIZDataType::Int;
		SimpleData.SetInt(Value);
	}

	void SetInt64(int64 Value)
	{
		DataType = EGWIZDataType::Int64;
		SimpleData.SetInt64(Value);
	}

	void SetBool(bool Value)
	{
		DataType = EGWIZDataType::Bool;
		SimpleData.SetBool(Value);
	}

	// Setter methods for complex data
	void SetArray(const TArray<FGWIZSimpleData>& Value)
	{
		DataType = EGWIZDataType::Array;
		ComplexData.SetArray(Value);
	}

	void SetNested(const TMap<FString, FGWIZSimpleData>& Value)
	{
		DataType = EGWIZDataType::Nested;
		ComplexData.SetNested(Value);
	}

	// Getter methods for simple data
	FString GetString() const { return SimpleData.GetString(); }
	float GetFloat() const { return SimpleData.GetFloat(); }
	int32 GetInt() const { return SimpleData.GetInt(); }
	int64 GetInt64() const { return SimpleData.GetInt64(); }
	bool GetBool() const { return SimpleData.GetBool(); }

	// Getter methods for complex data
	TArray<FGWIZSimpleData> GetArray() const { return ComplexData.GetArray(); }
	TMap<FString, FGWIZSimpleData> GetNested() const { return ComplexData.GetNested(); }

	// Helper methods for complex data
	void SetNestedValue(const FString& Key, const FGWIZSimpleData& Value)
	{
		DataType = EGWIZDataType::Nested;
		ComplexData.SetNestedValue(Key, Value);
	}

	FGWIZSimpleData GetNestedValue(const FString& Key) const
	{
		return ComplexData.GetNestedValue(Key);
	}

	void AddArrayElement(const FGWIZSimpleData& Element)
	{
		DataType = EGWIZDataType::Array;
		ComplexData.AddArrayElement(Element);
	}

	FGWIZSimpleData GetArrayElement(int32 Index) const
	{
		return ComplexData.GetArrayElement(Index);
	}

	int32 GetArraySize() const
	{
		return ComplexData.GetArraySize();
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
