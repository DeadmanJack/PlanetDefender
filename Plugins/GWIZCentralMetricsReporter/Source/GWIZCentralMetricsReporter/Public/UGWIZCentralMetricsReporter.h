#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Engine.h"
#include "GWIZAnalyticsTypes.h"
#include "UGWIZSessionManager.h"
#include "UGWIZCentralMetricsReporter.generated.h"

// Forward declarations
class FTimerHandle;

/**
 * Central analytics reporter for comprehensive game analytics
 * Handles event collection, processing, and export to various systems
 * Singleton pattern for easy access throughout the application
 */
UCLASS(BlueprintType, Blueprintable)
class GWIZCENTRALMETRICSREPORTER_API UGWIZCentralMetricsReporter : public UObject
{
	GENERATED_BODY()

public:
	// Singleton access
	UFUNCTION(BlueprintCallable, Category = "Analytics", CallInEditor = true)
	static UGWIZCentralMetricsReporter* GetMetricsReporter();

	// Initialization
	UFUNCTION(BlueprintCallable, Category = "Analytics")
	void Initialize(const FGWIZExportConfig& Config);

	UFUNCTION(BlueprintCallable, Category = "Analytics")
	void Shutdown();

	// Event collection
	UFUNCTION(BlueprintCallable, Category = "Analytics")
	void CollectEvent(const FGWIZEventData& Event);

	UFUNCTION(BlueprintCallable, Category = "Analytics")
	void CollectEventWithData(const FString& EventType, const FString& SystemName, const FGWIZFlexibleData& Data);

	// Performance testing
	UFUNCTION(BlueprintCallable, Category = "Analytics")
	void StartPerformanceTest(const FString& TestName, const FString& SystemName);

	UFUNCTION(BlueprintCallable, Category = "Analytics")
	void EndPerformanceTest(const FString& TestName, const FString& SystemName);

	// Configuration
	UFUNCTION(BlueprintCallable, Category = "Analytics")
	void UpdateExportConfig(const FGWIZExportConfig& NewConfig);

	UFUNCTION(BlueprintPure, Category = "Analytics")
	const FGWIZExportConfig& GetExportConfig() const { return ExportConfig; }

	// Statistics
	UFUNCTION(BlueprintPure, Category = "Analytics")
	int32 GetTotalEventsCollected() const { return TotalEventsCollected; }

	UFUNCTION(BlueprintPure, Category = "Analytics")
	int32 GetEventsInCache() const { return CachedEvents.Num(); }

	UFUNCTION(BlueprintPure, Category = "Analytics")
	float GetAverageEventProcessingTime() const { return AverageEventProcessingTime; }

	// Manual export
	UFUNCTION(BlueprintCallable, Category = "Analytics")
	void ForceExport();

	UFUNCTION(BlueprintCallable, Category = "Analytics")
	void ClearCache();

protected:
	// Export configuration
	UPROPERTY(BlueprintReadOnly, Category = "Analytics")
	FGWIZExportConfig ExportConfig;

	// Event storage
	UPROPERTY(BlueprintReadOnly, Category = "Analytics")
	TArray<FGWIZEventData> CachedEvents;

	// Performance tracking
	UPROPERTY(BlueprintReadOnly, Category = "Analytics")
	TMap<FString, FDateTime> ActivePerformanceTests;

	// Statistics
	UPROPERTY(BlueprintReadOnly, Category = "Analytics")
	int32 TotalEventsCollected;

	UPROPERTY(BlueprintReadOnly, Category = "Analytics")
	float AverageEventProcessingTime;

	UPROPERTY(BlueprintReadOnly, Category = "Analytics")
	float TotalProcessingTime;

	// Timers
	UPROPERTY()
	FTimerHandle* ExportTimerHandle;

	UPROPERTY()
	FTimerHandle* ProcessingTimerHandle;

	// State
	UPROPERTY(BlueprintReadOnly, Category = "Analytics")
	bool bInitialized;

	UPROPERTY(BlueprintReadOnly, Category = "Analytics")
	bool bShutdownRequested;

private:
	// Static instance
	static UGWIZCentralMetricsReporter* Instance;

	// Internal processing
	void ProcessEvent(const FGWIZEventData& Event);
	void ExportEvents();
	void ProcessBatchExport();
	void HandleExportFailure(const FString& ErrorMessage);

	// Timer callbacks
	void OnExportTimer();
	void OnProcessingTimer();

	// File export
	void ExportToFile(const TArray<FGWIZEventData>& Events);
	FString SerializeEventToJSON(const FGWIZEventData& Event);
	FString SerializeEventToCSV(const FGWIZEventData& Event);

	// HTTP export (placeholder for future implementation)
	void ExportToHTTP(const TArray<FGWIZEventData>& Events);

	// Performance tracking
	void UpdatePerformanceStatistics(float ProcessingTime);
};
