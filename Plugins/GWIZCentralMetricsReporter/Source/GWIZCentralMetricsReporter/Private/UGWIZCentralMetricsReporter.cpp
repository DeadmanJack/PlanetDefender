#include "UGWIZCentralMetricsReporter.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/DateTime.h"
#include "Json/Public/Json.h"
#include "Json/Public/JsonUtilities.h"

// Static instance
UGWIZCentralMetricsReporter* UGWIZCentralMetricsReporter::Instance = nullptr;

UGWIZCentralMetricsReporter* UGWIZCentralMetricsReporter::GetMetricsReporter()
{
	if (!Instance)
	{
		Instance = NewObject<UGWIZCentralMetricsReporter>();
		Instance->AddToRoot(); // Prevent garbage collection
	}
	return Instance;
}

void UGWIZCentralMetricsReporter::Initialize(const FGWIZExportConfig& Config)
{
	if (bInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("Analytics reporter already initialized."));
		return;
	}

	// Set configuration
	ExportConfig = Config;
	
	// Initialize session manager if not already done
	UGWIZSessionManager::GetSessionManager()->InitializeSession();
	
	// Initialize statistics
	TotalEventsCollected = 0;
	AverageEventProcessingTime = 0.0f;
	TotalProcessingTime = 0.0f;
	
	// Clear any existing data
	CachedEvents.Empty();
	ActivePerformanceTests.Empty();
	
	// Set up timers if export interval is specified
	if (ExportConfig.ExportInterval > 0.0f)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				ExportTimerHandle,
				this,
				&UGWIZCentralMetricsReporter::OnExportTimer,
				ExportConfig.ExportInterval,
				true
			);
		}
	}
	
	bInitialized = true;
	bShutdownRequested = false;
	
	UE_LOG(LogTemp, Log, TEXT("Analytics reporter initialized with export interval: %.2f seconds"), ExportConfig.ExportInterval);
}

void UGWIZCentralMetricsReporter::Shutdown()
{
	if (!bInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("Analytics reporter not initialized."));
		return;
	}
	
	bShutdownRequested = true;
	
	// Stop timers
	if (UWorld* World = GetWorld())
	{
		if (ExportTimerHandle)
		{
			World->GetTimerManager().ClearTimer(*ExportTimerHandle);
		}
		if (ProcessingTimerHandle)
		{
			World->GetTimerManager().ClearTimer(*ProcessingTimerHandle);
		}
	}
	
	// Export any remaining events
	if (CachedEvents.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Exporting %d remaining events before shutdown"), CachedEvents.Num());
		ExportEvents();
	}
	
	// End session
	UGWIZSessionManager::GetSessionManager()->EndSession();
	
	bInitialized = false;
	
	UE_LOG(LogTemp, Log, TEXT("Analytics reporter shutdown complete. Total events: %d"), TotalEventsCollected);
}

void UGWIZCentralMetricsReporter::CollectEvent(const FGWIZEventData& Event)
{
	if (!bInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("Analytics reporter not initialized. Event ignored."));
		return;
	}
	
	const double StartTime = FPlatformTime::Seconds();
	
	// Process the event
	ProcessEvent(Event);
	
	// Update performance statistics
	const double EndTime = FPlatformTime::Seconds();
	const float ProcessingTime = static_cast<float>(EndTime - StartTime);
	UpdatePerformanceStatistics(ProcessingTime);
	
	TotalEventsCollected++;
	
	// Check if we need to export
	if (CachedEvents.Num() >= ExportConfig.BatchSize)
	{
		ProcessBatchExport();
	}
}

void UGWIZCentralMetricsReporter::CollectEventWithData(const FString& EventType, const FString& SystemName, const FGWIZFlexibleData& Data)
{
	FGWIZEventData Event = UGWIZSessionManager::GetSessionManager()->CreateEventWithData(EventType, SystemName, Data);
	CollectEvent(Event);
}

void UGWIZCentralMetricsReporter::StartPerformanceTest(const FString& TestName, const FString& SystemName)
{
	if (!bInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("Analytics reporter not initialized. Performance test ignored."));
		return;
	}
	
	FString TestKey = FString::Printf(TEXT("%s_%s"), *TestName, *SystemName);
	ActivePerformanceTests.Add(TestKey, FDateTime::Now());
	
	UE_LOG(LogTemp, Log, TEXT("Performance test started: %s"), *TestKey);
}

void UGWIZCentralMetricsReporter::EndPerformanceTest(const FString& TestName, const FString& SystemName)
{
	if (!bInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("Analytics reporter not initialized. Performance test ignored."));
		return;
	}
	
	FString TestKey = FString::Printf(TEXT("%s_%s"), *TestName, *SystemName);
	
	if (const FDateTime* StartTime = ActivePerformanceTests.Find(TestKey))
	{
		const FDateTime EndTime = FDateTime::Now();
		const float Duration = (EndTime - *StartTime).GetTotalSeconds();
		
		// Create performance event
		FGWIZEventData PerformanceEvent = UGWIZSessionManager::GetSessionManager()->CreateEvent("Performance", "PerformanceTest");
		PerformanceEvent.Data.SetString("TestName", TestName);
		PerformanceEvent.Data.SetString("SystemName", SystemName);
		PerformanceEvent.Data.SetFloat("Duration", Duration);
		PerformanceEvent.Data.SetString("Status", "Completed");
		
		CollectEvent(PerformanceEvent);
		
		ActivePerformanceTests.Remove(TestKey);
		
		UE_LOG(LogTemp, Log, TEXT("Performance test completed: %s (Duration: %.3f seconds)"), *TestKey, Duration);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Performance test not found: %s"), *TestKey);
	}
}

void UGWIZCentralMetricsReporter::UpdateExportConfig(const FGWIZExportConfig& NewConfig)
{
	ExportConfig = NewConfig;
	
	// Update timer if export interval changed
	if (bInitialized && UWorld* World = GetWorld())
	{
		if (ExportTimerHandle)
		{
			World->GetTimerManager().ClearTimer(*ExportTimerHandle);
		}
		
		if (ExportConfig.ExportInterval > 0.0f)
		{
			World->GetTimerManager().SetTimer(
				ExportTimerHandle,
				this,
				&UGWIZCentralMetricsReporter::OnExportTimer,
				ExportConfig.ExportInterval,
				true
			);
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("Export configuration updated"));
}

void UGWIZCentralMetricsReporter::ForceExport()
{
	if (!bInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("Analytics reporter not initialized."));
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("Force exporting %d events"), CachedEvents.Num());
	ExportEvents();
}

void UGWIZCentralMetricsReporter::ClearCache()
{
	if (!bInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("Analytics reporter not initialized."));
		return;
	}
	
	const int32 CachedCount = CachedEvents.Num();
	CachedEvents.Empty();
	
	UE_LOG(LogTemp, Log, TEXT("Cache cleared. Removed %d events"), CachedCount);
}

void UGWIZCentralMetricsReporter::ProcessEvent(const FGWIZEventData& Event)
{
	// Add event to cache
	CachedEvents.Add(Event);
	
	// Log event for debugging
	UE_LOG(LogTemp, Verbose, TEXT("Event collected: %s [%s] - %s"), 
		*Event.EventType, *Event.SystemName, *Event.EventID);
}

void UGWIZCentralMetricsReporter::ExportEvents()
{
	if (CachedEvents.Num() == 0)
	{
		return;
	}
	
	// Export to file if enabled
	if (ExportConfig.bEnableFileExport)
	{
		ExportToFile(CachedEvents);
	}
	
	// Export to HTTP if enabled
	if (ExportConfig.bEnableHTTPExport)
	{
		ExportToHTTP(CachedEvents);
	}
	
	// Clear cache after successful export
	CachedEvents.Empty();
}

void UGWIZCentralMetricsReporter::ProcessBatchExport()
{
	if (CachedEvents.Num() >= ExportConfig.BatchSize)
	{
		UE_LOG(LogTemp, Log, TEXT("Batch export triggered: %d events"), CachedEvents.Num());
		ExportEvents();
	}
}

void UGWIZCentralMetricsReporter::HandleExportFailure(const FString& ErrorMessage)
{
	UE_LOG(LogTemp, Error, TEXT("Export failure: %s"), *ErrorMessage);
	
	// If offline caching is enabled, keep events in cache
	if (!ExportConfig.bEnableOfflineCaching)
	{
		// Remove events that failed to export
		const int32 MaxCached = FMath::Min(ExportConfig.MaxCachedEvents, CachedEvents.Num());
		CachedEvents.RemoveAt(0, CachedEvents.Num() - MaxCached);
	}
}

void UGWIZCentralMetricsReporter::OnExportTimer()
{
	if (bShutdownRequested)
	{
		return;
	}
	
	ExportEvents();
}

void UGWIZCentralMetricsReporter::OnProcessingTimer()
{
	// This can be used for periodic processing tasks
	// Currently not implemented
}

void UGWIZCentralMetricsReporter::ExportToFile(const TArray<FGWIZEventData>& Events)
{
	if (Events.Num() == 0)
	{
		return;
	}
	
	// Create directory if it doesn't exist
	FString ExportPath = FPaths::ProjectSavedDir() + ExportConfig.FileExportPath;
	IFileManager::Get().MakeDirectory(*ExportPath, true);
	
	// Generate filename with timestamp
	FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));
	FString Filename = FString::Printf(TEXT("Analytics_%s.json"), *Timestamp);
	FString FullPath = ExportPath + Filename;
	
	// Serialize events to JSON
	FString JsonContent;
	for (const FGWIZEventData& Event : Events)
	{
		JsonContent += SerializeEventToJSON(Event) + TEXT("\n");
	}
	
	// Write to file
	if (FFileHelper::SaveStringToFile(JsonContent, *FullPath))
	{
		UE_LOG(LogTemp, Log, TEXT("Exported %d events to file: %s"), Events.Num(), *FullPath);
	}
	else
	{
		HandleExportFailure(FString::Printf(TEXT("Failed to write to file: %s"), *FullPath));
	}
}

FString UGWIZCentralMetricsReporter::SerializeEventToJSON(const FGWIZEventData& Event)
{
	// Create JSON object
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	
	// Add event data
	JsonObject->SetStringField("EventID", Event.EventID);
	JsonObject->SetStringField("EventType", Event.EventType);
	JsonObject->SetStringField("SystemName", Event.SystemName);
	JsonObject->SetStringField("Timestamp", Event.Timestamp.ToString());
	
	// Add session data
	const FGWIZStaticSessionData& SessionData = UGWIZSessionManager::GetSessionManager()->GetStaticSessionData();
	JsonObject->SetStringField("SessionID", SessionData.SessionID);
	JsonObject->SetStringField("Platform", SessionData.Platform);
	JsonObject->SetStringField("EngineVersion", SessionData.EngineVersion);
	
	// Add event-specific data (simplified for now)
	JsonObject->SetStringField("Data", TEXT("Event data serialization not yet implemented"));
	
	// Serialize to string
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	
	return OutputString;
}

FString UGWIZCentralMetricsReporter::SerializeEventToCSV(const FGWIZEventData& Event)
{
	// CSV serialization (placeholder)
	return FString::Printf(TEXT("%s,%s,%s,%s"), 
		*Event.EventID, *Event.EventType, *Event.SystemName, *Event.Timestamp.ToString());
}

void UGWIZCentralMetricsReporter::ExportToHTTP(const TArray<FGWIZEventData>& Events)
{
	// HTTP export implementation will be added in Phase 5.3
	UE_LOG(LogTemp, Log, TEXT("HTTP export not yet implemented. %d events skipped."), Events.Num());
}

void UGWIZCentralMetricsReporter::UpdatePerformanceStatistics(float ProcessingTime)
{
	TotalProcessingTime += ProcessingTime;
	AverageEventProcessingTime = TotalProcessingTime / TotalEventsCollected;
}
