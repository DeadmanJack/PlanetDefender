// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.
// This code is part of the GWIZ Generic Pooling System for Unreal Engine.

#include "SimplePoolingTests.h"
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "UObject/UObjectGlobals.h"
#include "PoolingSystem/GWIZObjectPool.h"
#include "PoolingSystem/GWIZPoolingManager.h"
#include "PoolingSystem/PoolingTypes.h"
#include "TestRunner.h"

/**
 * Simple test suite for GWIZ Pooling System.
 * Tests core functionality without complex automation framework.
 */
class FSimplePoolingTests
{
public:
    static void RunAllTests()
    {
        UE_LOG(LogTemp, Log, TEXT("=== Starting Simple Pooling Tests ==="));
        
        TestBasicPoolCreation();
        TestObjectRetrievalAndReturn();
        TestPoolConfiguration();
        TestPoolStatistics();
        TestPoolingManager();
        
        UE_LOG(LogTemp, Log, TEXT("=== Simple Pooling Tests Complete ==="));
    }

private:
    static void TestBasicPoolCreation()
    {
        UE_LOG(LogTemp, Log, TEXT("Testing Basic Pool Creation..."));
        
        // Create a test pool
        UGWIZObjectPool* TestPool = NewObject<UGWIZObjectPool>();
        if (!TestPool)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create test pool"));
            return;
        }
        
        // Set up basic configuration
        TestPool->SetPooledObjectClass(UGWIZPoolingTestRunner::StaticClass());
        TestPool->Config.MinPoolSize = 5;
        TestPool->Config.MaxPoolSize = 20;
        TestPool->Config.InitialPoolSize = 10;
        
        // Test initialization
        TestPool->InitializePool();
        
        // Verify pool state
        int32 CurrentSize = TestPool->GetCurrentPoolSize();
        int32 ObjectsInUse = TestPool->GetObjectsInUse();
        
        UE_LOG(LogTemp, Log, TEXT("Pool created successfully"));
        UE_LOG(LogTemp, Log, TEXT("Current pool size: %d"), CurrentSize);
        UE_LOG(LogTemp, Log, TEXT("Objects in use: %d"), ObjectsInUse);
        UE_LOG(LogTemp, Log, TEXT("Pool initialized: %s"), TestPool->IsInitialized() ? TEXT("Yes") : TEXT("No"));
        
        // Cleanup
        TestPool->ClearPool();
    }
    
    static void TestObjectRetrievalAndReturn()
    {
        UE_LOG(LogTemp, Log, TEXT("Testing Object Retrieval and Return..."));
        
        UGWIZObjectPool* TestPool = NewObject<UGWIZObjectPool>();
        TestPool->SetPooledObjectClass(UGWIZPoolingTestRunner::StaticClass());
        TestPool->Config.MinPoolSize = 3;
        TestPool->Config.MaxPoolSize = 10;
        TestPool->Config.InitialPoolSize = 5;
        TestPool->InitializePool();
        
        // Test object retrieval
        TArray<UObject*> RetrievedObjects;
        for (int32 i = 0; i < 3; ++i)
        {
            UObject* Obj = TestPool->GetFromPool();
            if (Obj)
            {
                RetrievedObjects.Add(Obj);
                UE_LOG(LogTemp, Log, TEXT("Retrieved object %d"), i + 1);
            }
        }
        
        UE_LOG(LogTemp, Log, TEXT("Retrieved %d objects"), RetrievedObjects.Num());
        UE_LOG(LogTemp, Log, TEXT("Pool size after retrieval: %d"), TestPool->GetCurrentPoolSize());
        UE_LOG(LogTemp, Log, TEXT("Objects in use: %d"), TestPool->GetObjectsInUse());
        
        // Test object return
        for (UObject* Obj : RetrievedObjects)
        {
            TestPool->ReturnToPool(Obj);
        }
        
        UE_LOG(LogTemp, Log, TEXT("Returned all objects"));
        UE_LOG(LogTemp, Log, TEXT("Pool size after return: %d"), TestPool->GetCurrentPoolSize());
        UE_LOG(LogTemp, Log, TEXT("Objects in use: %d"), TestPool->GetObjectsInUse());
        
        TestPool->ClearPool();
    }
    
    static void TestPoolConfiguration()
    {
        UE_LOG(LogTemp, Log, TEXT("Testing Pool Configuration..."));
        
        UGWIZObjectPool* TestPool = NewObject<UGWIZObjectPool>();
        TestPool->SetPooledObjectClass(UGWIZPoolingTestRunner::StaticClass());
        
        // Test configuration update
        FGWIZPoolConfig NewConfig;
        NewConfig.MinPoolSize = 10;
        NewConfig.MaxPoolSize = 50;
        NewConfig.InitialPoolSize = 25;
        NewConfig.Priority = 2;
        NewConfig.Category = TEXT("TestCategory");
        
        TestPool->ConfigurePool(NewConfig);
        
        UE_LOG(LogTemp, Log, TEXT("Configuration updated"));
        UE_LOG(LogTemp, Log, TEXT("Min pool size: %d"), TestPool->Config.MinPoolSize);
        UE_LOG(LogTemp, Log, TEXT("Max pool size: %d"), TestPool->Config.MaxPoolSize);
        UE_LOG(LogTemp, Log, TEXT("Initial pool size: %d"), TestPool->Config.InitialPoolSize);
        UE_LOG(LogTemp, Log, TEXT("Priority: %d"), TestPool->Config.Priority);
        UE_LOG(LogTemp, Log, TEXT("Category: %s"), *TestPool->Config.Category);
        
        TestPool->ClearPool();
    }
    
    static void TestPoolStatistics()
    {
        UE_LOG(LogTemp, Log, TEXT("Testing Pool Statistics..."));
        
        UGWIZObjectPool* TestPool = NewObject<UGWIZObjectPool>();
        TestPool->SetPooledObjectClass(UGWIZPoolingTestRunner::StaticClass());
        TestPool->Config.MinPoolSize = 2;
        TestPool->Config.MaxPoolSize = 10;
        TestPool->Config.InitialPoolSize = 5;
        TestPool->InitializePool();
        
        // Get initial statistics
        FGWIZPoolStatistics InitialStats = TestPool->GetStatistics();
        UE_LOG(LogTemp, Log, TEXT("Initial statistics:"));
        UE_LOG(LogTemp, Log, TEXT("  Current pool size: %d"), InitialStats.CurrentPoolSize);
        UE_LOG(LogTemp, Log, TEXT("  Objects in use: %d"), InitialStats.ObjectsInUse);
        UE_LOG(LogTemp, Log, TEXT("  Total objects created: %d"), InitialStats.TotalObjectsCreated);
        UE_LOG(LogTemp, Log, TEXT("  Pool hits: %d"), InitialStats.PoolHits);
        UE_LOG(LogTemp, Log, TEXT("  Pool misses: %d"), InitialStats.PoolMisses);
        UE_LOG(LogTemp, Log, TEXT("  Hit rate: %.2f"), InitialStats.HitRate);
        
        // Perform some operations
        UObject* Obj1 = TestPool->GetFromPool();
        UObject* Obj2 = TestPool->GetFromPool();
        
        FGWIZPoolStatistics MidStats = TestPool->GetStatistics();
        UE_LOG(LogTemp, Log, TEXT("Mid-operation statistics:"));
        UE_LOG(LogTemp, Log, TEXT("  Current pool size: %d"), MidStats.CurrentPoolSize);
        UE_LOG(LogTemp, Log, TEXT("  Objects in use: %d"), MidStats.ObjectsInUse);
        UE_LOG(LogTemp, Log, TEXT("  Pool hits: %d"), MidStats.PoolHits);
        UE_LOG(LogTemp, Log, TEXT("  Pool misses: %d"), MidStats.PoolMisses);
        
        // Return objects
        TestPool->ReturnToPool(Obj1);
        TestPool->ReturnToPool(Obj2);
        
        FGWIZPoolStatistics FinalStats = TestPool->GetStatistics();
        UE_LOG(LogTemp, Log, TEXT("Final statistics:"));
        UE_LOG(LogTemp, Log, TEXT("  Current pool size: %d"), FinalStats.CurrentPoolSize);
        UE_LOG(LogTemp, Log, TEXT("  Objects in use: %d"), FinalStats.ObjectsInUse);
        UE_LOG(LogTemp, Log, TEXT("  Pool hits: %d"), FinalStats.PoolHits);
        UE_LOG(LogTemp, Log, TEXT("  Pool misses: %d"), FinalStats.PoolMisses);
        
        TestPool->ClearPool();
    }
    
    static void TestPoolingManager()
    {
        UE_LOG(LogTemp, Log, TEXT("Testing Pooling Manager..."));
        
        // Create a test world
        UWorld* TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
        if (!TestWorld)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create test world"));
            return;
        }
        
        TestWorld->InitWorld();
        
        // Spawn pooling manager
        AGWIZPoolingManager* Manager = TestWorld->SpawnActor<AGWIZPoolingManager>();
        if (!Manager)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn pooling manager"));
            TestWorld->DestroyWorld(false);
            return;
        }
        
        UE_LOG(LogTemp, Log, TEXT("Pooling manager created successfully"));
        
        // Test pool retrieval
        UGWIZObjectPool* Pool = Manager->GetPool(UGWIZPoolingTestRunner::StaticClass());
        if (Pool)
        {
            UE_LOG(LogTemp, Log, TEXT("Successfully retrieved pool for UGWIZPoolingTestRunner"));
            UE_LOG(LogTemp, Log, TEXT("Pool class: %s"), *Pool->GetClass()->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to retrieve pool for UObject"));
        }
        
        // Test object lifecycle
        UObject* PooledObject = Manager->GetPooledObject(UGWIZPoolingTestRunner::StaticClass());
        if (PooledObject)
        {
            UE_LOG(LogTemp, Log, TEXT("Successfully retrieved pooled object"));
            Manager->ReturnPooledObject(PooledObject);
            UE_LOG(LogTemp, Log, TEXT("Successfully returned pooled object"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to retrieve pooled object"));
        }
        
        // Cleanup
        Manager->Destroy();
        TestWorld->DestroyWorld(false);
    }
};

// Simple test runner that can be called from anywhere
void RunSimplePoolingTests()
{
    FSimplePoolingTests::RunAllTests();
}
