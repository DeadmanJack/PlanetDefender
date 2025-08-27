// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.
// This code is part of the GWIZ Generic Pooling System for Unreal Engine.

#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "PoolingSystem/GWIZPoolingManager.h"
#include "PoolingSystem/PoolingTypes.h"
#include "Engine/World.h"
#include "Blueprint/Blueprint.h"
#include "Kismet/BlueprintFunctionLibrary.h"

/**
 * Test suite for GWIZ Pooling System Blueprint integration.
 * Tests all Blueprint-exposed functions and interface implementations.
 */
class GWIZBlueprintTests : public FAutomationTest
{
public:
    GWIZBlueprintTests(const FString& InName, const bool bInComplexTask)
        : FAutomationTest(InName, bInComplexTask)
    {
    }

    // FAutomationTest interface
    virtual uint32 GetTestFlags() const override;
    virtual bool RunTest(const FString& Parameters) override;

protected:
    // Blueprint test scenarios
    void TestBlueprintFunctionExposure();
    void TestBlueprintInterfaceImplementation();
    void TestBlueprintConfiguration();
    void TestBlueprintDebugOutput();
    void TestBlueprintPerformanceMonitoring();
    void TestBlueprintPoolOperations();
    void TestBlueprintStatistics();
    void TestBlueprintErrorHandling();
    void TestBlueprintCustomLogic();
    void TestBlueprintIntegrationWorkflow();

private:
    // Test data
    AGWIZPoolingManager* TestManager;
    UWorld* TestWorld;
    UBlueprint* TestBlueprint;
    
    // Helper methods
    void SetupTestEnvironment();
    void CleanupTestEnvironment();
    void CreateTestWorld();
    void DestroyTestWorld();
    void CreateTestBlueprint();
    void DestroyTestBlueprint();
    void ValidateBlueprintFunctions(const FString& TestName);
    void TestBlueprintNodeExecution();
    void SimulateBlueprintWorkflow();
};
