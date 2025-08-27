// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.
// This code is part of the GWIZ Generic Pooling System for Unreal Engine.

#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "PoolingSystem/GWIZObjectPool.h"
#include "PoolingSystem/PoolingTypes.h"

/**
 * Test suite for GWIZ Object Pool functionality.
 * Tests all pool operations individually with edge cases.
 */
class GWIZObjectPoolTests : public FAutomationTest
{
public:
    GWIZObjectPoolTests(const FString& InName, const bool bInComplexTask)
        : FAutomationTest(InName, bInComplexTask)
    {
    }

    // FAutomationTest interface
    virtual uint32 GetTestFlags() const override;
    virtual bool RunTest(const FString& Parameters) override;

protected:
    // Test helper functions
    void TestPoolCreation();
    void TestPoolConfiguration();
    void TestObjectRetrieval();
    void TestObjectReturn();
    void TestPoolResizing();
    void TestPoolStatistics();
    void TestEdgeCases();
    void TestPerformance();
    void TestMemoryManagement();
    void TestThreadSafety();

private:
    // Test data
    UGWIZObjectPool* TestPool;
    TSubclassOf<UObject> TestObjectClass;
    
    // Helper methods
    void SetupTestEnvironment();
    void CleanupTestEnvironment();
    void ValidatePoolState(const FString& TestName, int32 ExpectedSize, int32 ExpectedInUse);
};
