// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.
// This code is part of the GWIZ Generic Pooling System for Unreal Engine.

#include "TestRunner.h"
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "PoolingSystem/GWIZObjectPool.h"
#include "SimplePoolingTests.h"

/**
 * Simple test runner for GWIZ Pooling System.
 * Can be called from game instance or console.
 */

// UCLASS implementation
void UGWIZPoolingTestRunner::RunQuickPoolingTest()
{
    ::RunQuickPoolingTest();
}

void UGWIZPoolingTestRunner::RunAllPoolingTests()
{
    ::RunAllPoolingTests();
}

// Simple test function that can be called from anywhere
void RunQuickPoolingTest()
{
    UE_LOG(LogTemp, Log, TEXT("=== Quick Pooling Test ==="));
    
    // Quick test of basic functionality
    UGWIZObjectPool* TestPool = NewObject<UGWIZObjectPool>();
    if (TestPool)
    {
        TestPool->SetPooledObjectClass(UGWIZPoolingTestRunner::StaticClass());
        TestPool->Config.MinPoolSize = 2;
        TestPool->Config.MaxPoolSize = 5;
        TestPool->Config.InitialPoolSize = 3;
        TestPool->InitializePool();
        
        int32 CurrentSize = TestPool->GetCurrentPoolSize();
        int32 ObjectsInUse = TestPool->GetObjectsInUse();
        UE_LOG(LogTemp, Log, TEXT("Pool created - Size: %d, In Use: %d"), CurrentSize, ObjectsInUse);
        
        // Test object retrieval
        UObject* Obj = TestPool->GetFromPool();
        if (Obj)
        {
            UE_LOG(LogTemp, Log, TEXT("Object retrieved successfully"));
            TestPool->ReturnToPool(Obj);
            UE_LOG(LogTemp, Log, TEXT("Object returned successfully"));
        }
        
        TestPool->ClearPool();
        UE_LOG(LogTemp, Log, TEXT("Quick test completed successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create test pool"));
    }
}

// Main test runner function
void RunAllPoolingTests()
{
    UE_LOG(LogTemp, Log, TEXT("=== Running GWIZ Pooling Tests ==="));
    RunSimplePoolingTests();
    UE_LOG(LogTemp, Log, TEXT("=== GWIZ Pooling Tests Complete ==="));
}
