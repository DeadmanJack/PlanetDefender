// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.
// This code is part of the GWIZ Generic Pooling System for Unreal Engine.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PoolingSystem/GWIZPoolingManager.h"
#include "PoolingSystem/PoolingTypes.h"
#include "GWIZGameInstance.generated.h"

/**
 * Game Instance class that integrates with the GWIZ Pooling System.
 * Provides global persistence for pools across level transitions and
 * manages the global pooling manager instance.
 */
UCLASS()
class PLANETDEFENDER_API UGWIZGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UGWIZGameInstance();

    // UGameInstance interface
    virtual void Init() override;
    virtual void Shutdown() override;
    virtual void OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld) override;

    // Pooling System Integration
    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    AGWIZPoolingManager* GetGlobalPoolingManager() const;

    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    bool IsPoolingSystemInitialized() const;

    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    void SavePoolingSystemState();

    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    void LoadPoolingSystemState();

    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    void PreWarmPoolsForLevel(const FString& LevelName);

    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    void CleanupUnusedPoolsForLevel(const FString& LevelName);

protected:
    // Global pooling manager instance
    UPROPERTY()
    AGWIZPoolingManager* GlobalPoolingManager;

    // Pooling system state for persistence
    UPROPERTY()
    TMap<FString, FGWIZPoolConfig> PersistentPoolConfigs;

    // Level-specific pool configurations
    UPROPERTY()
    TMap<FString, FGWIZLevelPoolConfigs> LevelPoolConfigs;

    // Pooling system initialization state
    UPROPERTY()
    bool bPoolingSystemInitialized;

    // Initialize the pooling system
    void InitializePoolingSystem();

    // Handle level transitions
    void HandleLevelTransition(UWorld* OldWorld, UWorld* NewWorld);

    // Save/load pool configurations
    void SavePoolConfigurations();
    void LoadPoolConfigurations();

    // Level-specific pool management
    void SetupPoolsForLevel(const FString& LevelName);
    void CleanupPoolsForLevel(const FString& LevelName);

    // Pool state serialization
    void SerializePoolState();
    void DeserializePoolState();

private:
    // Critical section for thread-safe operations
    mutable FCriticalSection PoolingMutex;
};

