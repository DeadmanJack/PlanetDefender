// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.
// This code is part of the GWIZ Generic Pooling System for Unreal Engine.

#pragma once

#include "CoreMinimal.h"
#include "TestFramework/AutomationTest.h"
#include "PoolingSystem/GWIZPoolingManager.h"
#include "PoolingSystem/PoolingTypes.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

/**
 * Test suite for GWIZ Pooling System integration scenarios.
 * Tests end-to-end pooling workflows with real objects.
 */
class GWIZIntegrationTests : public FAutomationTest
{
public:
    GWIZIntegrationTests(const FString& InName, const bool bInComplexTask)
        : FAutomationTest(InName, bInComplexTask)
    {
    }

    // FAutomationTest interface
    virtual uint32 GetTestFlags() const override;
    virtual bool RunTest(const FString& Parameters) override;

protected:
    // Integration test scenarios
    void TestEndToEndPoolingWorkflow();
    void TestMultiClassPooling();
    void TestLevelTransitionScenarios();
    void TestGameInstanceIntegration();
    void TestPerformanceUnderLoad();
    void TestMemoryPressureScenarios();
    void TestConcurrentAccessPatterns();
    void TestErrorRecoveryScenarios();
    void TestConfigurationHotReload();
    void TestStatisticsAccuracy();

private:
    // Test data
    AGWIZPoolingManager* TestManager;
    UWorld* TestWorld;
    UGWIZGameInstance* TestGameInstance;
    
    // Test objects
    TArray<AActor*> TestActors;
    TArray<UObject*> TestObjects;
    
    // Helper methods
    void SetupTestEnvironment();
    void CleanupTestEnvironment();
    void CreateTestWorld();
    void DestroyTestWorld();
    void CreateTestActors(int32 Count);
    void DestroyTestActors();
    void ValidateSystemState(const FString& TestName);
    void SimulateGameplayScenario();
    void PerformStressTest(int32 ObjectCount, int32 Iterations);
};
