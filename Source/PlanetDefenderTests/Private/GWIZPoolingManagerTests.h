// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.
// This code is part of the GWIZ Generic Pooling System for Unreal Engine.

#pragma once

#include "CoreMinimal.h"
#include "TestFramework/AutomationTest.h"
#include "PoolingSystem/GWIZPoolingManager.h"
#include "PoolingSystem/PoolingTypes.h"

/**
 * Test suite for GWIZ Pooling Manager functionality.
 * Tests all manager operations and integration scenarios.
 */
class GWIZPoolingManagerTests : public FAutomationTest
{
public:
    GWIZPoolingManagerTests(const FString& InName, const bool bInComplexTask)
        : FAutomationTest(InName, bInComplexTask)
    {
    }

    // FAutomationTest interface
    virtual uint32 GetTestFlags() const override;
    virtual bool RunTest(const FString& Parameters) override;

protected:
    // Test helper functions
    void TestManagerCreation();
    void TestPoolRetrieval();
    void TestPoolConfiguration();
    void TestObjectLifecycle();
    void TestPoolPreWarming();
    void TestStatisticsCollection();
    void TestPoolManagement();
    void TestQueryOperations();
    void TestMemoryOperations();
    void TestPerformanceMonitoring();
    void TestAutoCleanup();
    void TestDebugFeatures();
    void TestThreadSafety();
    void TestIntegrationScenarios();

private:
    // Test data
    AGWIZPoolingManager* TestManager;
    UWorld* TestWorld;
    
    // Helper methods
    void SetupTestEnvironment();
    void CleanupTestEnvironment();
    void CreateTestWorld();
    void DestroyTestWorld();
    void ValidateManagerState(const FString& TestName, int32 ExpectedPoolCount);
};
