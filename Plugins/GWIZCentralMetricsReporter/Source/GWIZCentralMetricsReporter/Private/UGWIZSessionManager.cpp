#include "UGWIZSessionManager.h"
#include "Engine/Engine.h"
#include "HAL/PlatformProperties.h"
#include "HAL/PlatformMisc.h"
#include "Misc/App.h"
#include "Misc/EngineVersion.h"

// Static instance
UGWIZSessionManager* UGWIZSessionManager::Instance = nullptr;

UGWIZSessionManager* UGWIZSessionManager::GetSessionManager()
{
	if (!Instance)
	{
		Instance = NewObject<UGWIZSessionManager>();
		Instance->AddToRoot(); // Prevent garbage collection
	}
	return Instance;
}

void UGWIZSessionManager::InitializeSession()
{
	if (bSessionActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session already active. Ending previous session first."));
		EndSession();
	}

	// Generate session ID
	StaticSessionData.SessionID = GenerateSessionID();
	
	// Set session start time
	StaticSessionData.SessionStartTime = FDateTime::Now();
	
	// Detect platform and hardware information
	DetectPlatformInfo();
	DetectHardwareInfo();
	DetectEngineInfo();
	
	// Mark session as active
	bSessionActive = true;
	
	UE_LOG(LogTemp, Log, TEXT("Analytics session initialized: %s"), *StaticSessionData.SessionID);
}

void UGWIZSessionManager::EndSession()
{
	if (!bSessionActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("No active session to end."));
		return;
	}
	
	SessionEndTime = FDateTime::Now();
	bSessionActive = false;
	
	UE_LOG(LogTemp, Log, TEXT("Analytics session ended: %s (Duration: %.2f seconds)"), 
		*StaticSessionData.SessionID, GetSessionDurationSeconds());
}

FGWIZEventData UGWIZSessionManager::CreateEvent(const FString& EventType, const FString& SystemName)
{
	FGWIZEventData Event;
	Event.EventType = EventType;
	Event.SystemName = SystemName;
	Event.Timestamp = FDateTime::Now();
	
	// Generate unique event ID
	Event.EventID = FGuid::NewGuid().ToString();
	
	return Event;
}

FGWIZEventData UGWIZSessionManager::CreateEventWithData(const FString& EventType, const FString& SystemName, const FGWIZFlexibleData& Data)
{
	FGWIZEventData Event = CreateEvent(EventType, SystemName);
	Event.Data = Data;
	return Event;
}

float UGWIZSessionManager::GetSessionDurationSeconds() const
{
	if (!bSessionActive)
	{
		return (SessionEndTime - StaticSessionData.SessionStartTime).GetTotalSeconds();
	}
	else
	{
		return (FDateTime::Now() - StaticSessionData.SessionStartTime).GetTotalSeconds();
	}
}

FTimespan UGWIZSessionManager::GetSessionDuration() const
{
	if (!bSessionActive)
	{
		return SessionEndTime - StaticSessionData.SessionStartTime;
	}
	else
	{
		return FDateTime::Now() - StaticSessionData.SessionStartTime;
	}
}

void UGWIZSessionManager::DetectPlatformInfo()
{
	// Platform detection
	StaticSessionData.Platform = FPlatformProperties::GetPlatformInfo().IniPlatformName;
	
	// Distribution platform detection (simplified)
	StaticSessionData.DistributionPlatform = TEXT("Unknown");
	
	// You can expand this with Steam, Epic, etc. detection
	// For now, we'll use a placeholder
	if (FPlatformMisc::IsDebuggerPresent())
	{
		StaticSessionData.DistributionPlatform = TEXT("Development");
	}
	else
	{
		StaticSessionData.DistributionPlatform = TEXT("Standalone");
	}
}

void UGWIZSessionManager::DetectHardwareInfo()
{
	// CPU information
	StaticSessionData.CPUInfo = FPlatformMisc::GetCPUBrand();
	
	// GPU information (simplified)
	StaticSessionData.GPUInfo = FPlatformMisc::GetPrimaryGPUBrand();
	
	// System memory
	StaticSessionData.SystemMemoryMB = FPlatformMisc::GetTotalPhysicalGB() * 1024;
}

void UGWIZSessionManager::DetectEngineInfo()
{
	// Engine version
	StaticSessionData.EngineVersion = FEngineVersion::Current().ToString();
	
	// Build version (from app settings)
	StaticSessionData.BuildVersion = FApp::GetBuildVersion();
	
	// Build configuration
#if UE_BUILD_DEBUG
	StaticSessionData.BuildConfiguration = TEXT("Debug");
#elif UE_BUILD_DEVELOPMENT
	StaticSessionData.BuildConfiguration = TEXT("Development");
#elif UE_BUILD_SHIPPING
	StaticSessionData.BuildConfiguration = TEXT("Shipping");
#else
	StaticSessionData.BuildConfiguration = TEXT("Unknown");
#endif
}

FString UGWIZSessionManager::GenerateSessionID()
{
	// Generate a unique session ID using timestamp and random component
	FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));
	FString RandomComponent = FGuid::NewGuid().ToString().Left(8);
	return FString::Printf(TEXT("Session_%s_%s"), *Timestamp, *RandomComponent);
}
