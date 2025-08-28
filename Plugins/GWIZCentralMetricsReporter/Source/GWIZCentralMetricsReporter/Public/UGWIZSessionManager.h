#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GWIZAnalyticsTypes.h"
#include "UGWIZSessionManager.generated.h"

/**
 * Centralized session manager for analytics data
 * Manages static session data and provides event creation functionality
 * Singleton pattern for easy access throughout the application
 */
UCLASS(BlueprintType, Blueprintable)
class GWIZCENTRALMETRICSREPORTER_API UGWIZSessionManager : public UObject
{
	GENERATED_BODY()

public:
	// Singleton access
	UFUNCTION(BlueprintCallable, Category = "Analytics", CallInEditor = true)
	static UGWIZSessionManager* GetSessionManager();

	// Session management
	UFUNCTION(BlueprintCallable, Category = "Analytics")
	void InitializeSession();

	UFUNCTION(BlueprintCallable, Category = "Analytics")
	void EndSession();

	// Event creation
	UFUNCTION(BlueprintCallable, Category = "Analytics")
	FGWIZEventData CreateEvent(const FString& EventType, const FString& SystemName);

	UFUNCTION(BlueprintCallable, Category = "Analytics")
	FGWIZEventData CreateEventWithData(const FString& EventType, const FString& SystemName, const FGWIZFlexibleData& Data);

	// Session data access
	UFUNCTION(BlueprintPure, Category = "Analytics")
	const FGWIZStaticSessionData& GetStaticSessionData() const { return StaticSessionData; }

	UFUNCTION(BlueprintPure, Category = "Analytics")
	FString GetSessionID() const { return StaticSessionData.SessionID; }

	UFUNCTION(BlueprintPure, Category = "Analytics")
	FDateTime GetSessionStartTime() const { return StaticSessionData.SessionStartTime; }

	// Session duration
	UFUNCTION(BlueprintPure, Category = "Analytics")
	float GetSessionDurationSeconds() const;

	UFUNCTION(BlueprintPure, Category = "Analytics")
	FTimespan GetSessionDuration() const;

protected:
	// Static session data (collected once per session)
	UPROPERTY(BlueprintReadOnly, Category = "Analytics")
	FGWIZStaticSessionData StaticSessionData;

	// Session end time
	UPROPERTY(BlueprintReadOnly, Category = "Analytics")
	FDateTime SessionEndTime;

	// Session state
	UPROPERTY(BlueprintReadOnly, Category = "Analytics")
	bool bSessionActive;

private:
	// Singleton instance
	static UGWIZSessionManager* Instance;

	// Platform detection helpers
	void DetectPlatformInfo();
	void DetectHardwareInfo();
	void DetectEngineInfo();

	// Session ID generation
	FString GenerateSessionID();
};
