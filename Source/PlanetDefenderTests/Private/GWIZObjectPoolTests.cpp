// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.
// This code is part of the GWIZ Generic Pooling System for Unreal Engine.

#include "GWIZObjectPoolTests.h"
#include "PoolingSystem/GWIZObjectPool.h"
#include "PoolingSystem/PoolingTypes.h"
#include "Engine/Engine.h"
#include "UObject/UObjectGlobals.h"
#include "HAL/Thread.h"
#include "Async/Async.h"

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(GWIZObjectPoolTests, "GWIZ.Pooling.ObjectPool", FAutomationTestOpenCommand)

uint32 GWIZObjectPoolTests::GetTestFlags() const
{
    return EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter;
}

bool GWIZObjectPoolTests::RunTest(const FString& Parameters)
{
    // Setup test environment
    SetupTestEnvironment();
    
    // Run all test cases
    TestPoolCreation();
    TestPoolConfiguration();
    TestObjectRetrieval();
    TestObjectReturn();
    TestPoolResizing();
    TestPoolStatistics();
    TestEdgeCases();
    TestPerformance();
    TestMemoryManagement();
    TestThreadSafety();
    
    // Cleanup test environment
    CleanupTestEnvironment();
    
    return true;
}

void GWIZObjectPoolTests::SetupTestEnvironment()
{
    // Create a test object class (using UObject as base for testing)
    TestObjectClass = UObject::StaticClass();
    
    // Create a new pool for testing
    TestPool = NewObject<UGWIZObjectPool>();
    TestPool->PooledObjectClass = TestObjectClass;
    
    // Set default configuration
    TestPool->Config.MinPoolSize = 5;
    TestPool->Config.MaxPoolSize = 20;
    TestPool->Config.InitialPoolSize = 10;
    TestPool->Config.Priority = 1;
    TestPool->Config.Category = TEXT("Test");
}

void GWIZObjectPoolTests::CleanupTestEnvironment()
{
    if (TestPool)
    {
        TestPool->ClearPool();
        TestPool = nullptr;
    }
}

void GWIZObjectPoolTests::TestPoolCreation()
{
    // Test basic pool creation
    TestTrue(TEXT("Pool should be created"), TestPool != nullptr);
    TestTrue(TEXT("Pool should have correct object class"), TestPool->PooledObjectClass == TestObjectClass);
    TestTrue(TEXT("Pool should have default configuration"), TestPool->Config.MinPoolSize == 5);
    
    // Test pool initialization
    TestPool->InitializePool();
    TestTrue(TEXT("Pool should be initialized"), TestPool->IsInitialized());
    TestEqual(TEXT("Pool should have initial size"), TestPool->GetCurrentPoolSize(), 10);
}

void GWIZObjectPoolTests::TestPoolConfiguration()
{
    // Test configuration updates
    FGWIZPoolConfig NewConfig;
    NewConfig.MinPoolSize = 10;
    NewConfig.MaxPoolSize = 50;
    NewConfig.InitialPoolSize = 25;
    NewConfig.Priority = 2;
    NewConfig.Category = TEXT("UpdatedTest");
    
    TestPool->ConfigurePool(NewConfig);
    
    TestEqual(TEXT("Min pool size should be updated"), TestPool->Config.MinPoolSize, 10);
    TestEqual(TEXT("Max pool size should be updated"), TestPool->Config.MaxPoolSize, 50);
    TestEqual(TEXT("Initial pool size should be updated"), TestPool->Config.InitialPoolSize, 25);
    TestEqual(TEXT("Priority should be updated"), TestPool->Config.Priority, 2);
    TestEqual(TEXT("Category should be updated"), TestPool->Config.Category, TEXT("UpdatedTest"));
}

void GWIZObjectPoolTests::TestObjectRetrieval()
{
    // Test object retrieval from empty pool
    UObject* RetrievedObject = TestPool->GetFromPool();
    TestTrue(TEXT("Should retrieve object from pool"), RetrievedObject != nullptr);
    TestEqual(TEXT("Pool size should decrease"), TestPool->GetCurrentPoolSize(), 9);
    TestEqual(TEXT("Objects in use should increase"), TestPool->GetObjectsInUse(), 1);
    
    // Test multiple object retrievals
    TArray<UObject*> RetrievedObjects;
    for (int32 i = 0; i < 5; ++i)
    {
        UObject* Obj = TestPool->GetFromPool();
        if (Obj != nullptr)
        {
            RetrievedObjects.Add(Obj);
        }
    }
    
    TestEqual(TEXT("Should retrieve multiple objects"), RetrievedObjects.Num(), 5);
    TestEqual(TEXT("Pool size should reflect retrievals"), TestPool->GetCurrentPoolSize(), 4);
    TestEqual(TEXT("Objects in use should reflect retrievals"), TestPool->GetObjectsInUse(), 6);
}

void GWIZObjectPoolTests::TestObjectReturn()
{
    // First retrieve some objects
    TArray<UObject*> RetrievedObjects;
    for (int32 i = 0; i < 3; ++i)
    {
        UObject* Obj = TestPool->GetFromPool();
        if (Obj != nullptr)
        {
            RetrievedObjects.Add(Obj);
        }
    }
    
    int32 InitialPoolSize = TestPool->GetCurrentPoolSize();
    int32 InitialInUse = TestPool->GetObjectsInUse();
    
    // Return objects to pool
    for (UObject* Obj : RetrievedObjects)
    {
        TestPool->ReturnToPool(Obj);
    }
    
    TestEqual(TEXT("Pool size should increase after return"), TestPool->GetCurrentPoolSize(), InitialPoolSize + 3);
    TestEqual(TEXT("Objects in use should decrease after return"), TestPool->GetObjectsInUse(), InitialInUse - 3);
}

void GWIZObjectPoolTests::TestPoolResizing()
{
    // Test pool expansion beyond initial size
    TArray<UObject*> RetrievedObjects;
    int32 ObjectsToRetrieve = 15; // More than initial size
    
    for (int32 i = 0; i < ObjectsToRetrieve; ++i)
    {
        UObject* Obj = TestPool->GetFromPool();
        if (Obj != nullptr)
        {
            RetrievedObjects.Add(Obj);
        }
    }
    
    TestEqual(TEXT("Should retrieve all requested objects"), RetrievedObjects.Num(), ObjectsToRetrieve);
    TestEqual(TEXT("Pool should expand to accommodate requests"), TestPool->GetCurrentPoolSize(), 0);
    TestEqual(TEXT("All objects should be in use"), TestPool->GetObjectsInUse(), ObjectsToRetrieve);
    
    // Return all objects
    for (UObject* Obj : RetrievedObjects)
    {
        TestPool->ReturnToPool(Obj);
    }
    
    // Test pool shrinking to minimum size
    TestPool->ShrinkToMinimum();
    TestEqual(TEXT("Pool should shrink to minimum size"), TestPool->GetCurrentPoolSize(), TestPool->Config.MinPoolSize);
}

void GWIZObjectPoolTests::TestPoolStatistics()
{
    // Get initial statistics
    FGWIZPoolStatistics InitialStats = TestPool->GetStatistics();
    
    // Perform some operations
    UObject* Obj1 = TestPool->GetFromPool();
    UObject* Obj2 = TestPool->GetFromPool();
    
    // Get updated statistics
    FGWIZPoolStatistics UpdatedStats = TestPool->GetStatistics();
    
    TestEqual(TEXT("Pool hits should increase"), UpdatedStats.PoolHits, InitialStats.PoolHits + 2);
    TestEqual(TEXT("Current pool size should decrease"), UpdatedStats.CurrentPoolSize, InitialStats.CurrentPoolSize - 2);
    TestEqual(TEXT("Objects in use should increase"), UpdatedStats.ObjectsInUse, InitialStats.ObjectsInUse + 2);
    
    // Return objects
    TestPool->ReturnToPool(Obj1);
    TestPool->ReturnToPool(Obj2);
    
    // Get final statistics
    FGWIZPoolStatistics FinalStats = TestPool->GetStatistics();
    TestEqual(TEXT("Objects in use should return to initial"), FinalStats.ObjectsInUse, InitialStats.ObjectsInUse);
}

void GWIZObjectPoolTests::TestEdgeCases()
{
    // Test retrieving from empty pool
    TestPool->ClearPool();
    UObject* Obj = TestPool->GetFromPool();
    TestTrue(TEXT("Should create new object when pool is empty"), Obj != nullptr);
    
    // Test returning null object
    TestPool->ReturnToPool(nullptr);
    TestEqual(TEXT("Pool size should not change when returning null"), TestPool->GetCurrentPoolSize(), 1);
    
    // Test returning object of wrong class
    UObject* WrongClassObj = NewObject<UObject>();
    TestPool->ReturnToPool(WrongClassObj);
    TestEqual(TEXT("Pool size should not change when returning wrong class"), TestPool->GetCurrentPoolSize(), 1);
    
    // Test pool with zero size limits
    FGWIZPoolConfig ZeroConfig;
    ZeroConfig.MinPoolSize = 0;
    ZeroConfig.MaxPoolSize = 0;
    ZeroConfig.InitialPoolSize = 0;
    
    TestPool->ConfigurePool(ZeroConfig);
    TestPool->InitializePool();
    TestEqual(TEXT("Pool should respect zero size limits"), TestPool->GetCurrentPoolSize(), 0);
}

void GWIZObjectPoolTests::TestPerformance()
{
    // Test performance under load
    const int32 PerformanceTestSize = 1000;
    TArray<UObject*> PerformanceObjects;
    
    // Measure retrieval performance
    double StartTime = FPlatformTime::Seconds();
    
    for (int32 i = 0; i < PerformanceTestSize; ++i)
    {
        UObject* Obj = TestPool->GetFromPool();
        if (Obj != nullptr)
        {
            PerformanceObjects.Add(Obj);
        }
    }
    
    double RetrievalTime = FPlatformTime::Seconds() - StartTime;
    
    // Measure return performance
    StartTime = FPlatformTime::Seconds();
    
    for (UObject* Obj : PerformanceObjects)
    {
        TestPool->ReturnToPool(Obj);
    }
    
    double ReturnTime = FPlatformTime::Seconds() - StartTime;
    
    // Performance assertions (adjust thresholds as needed)
    TestTrue(TEXT("Retrieval performance should be reasonable"), RetrievalTime < 1.0);
    TestTrue(TEXT("Return performance should be reasonable"), ReturnTime < 1.0);
    
    UE_LOG(LogTemp, Log, TEXT("Performance Test - Retrieval: %.4f seconds, Return: %.4f seconds"), RetrievalTime, ReturnTime);
}

void GWIZObjectPoolTests::TestMemoryManagement()
{
    // Test memory usage tracking
    int64 InitialMemoryUsage = TestPool->GetMemoryUsage();
    
    // Create some objects
    TArray<UObject*> TestObjects;
    for (int32 i = 0; i < 10; ++i)
    {
        UObject* Obj = TestPool->GetFromPool();
        if (Obj != nullptr)
        {
            TestObjects.Add(Obj);
        }
    }
    
    int64 AfterRetrievalMemoryUsage = TestPool->GetMemoryUsage();
    
    // Return objects
    for (UObject* Obj : TestObjects)
    {
        TestPool->ReturnToPool(Obj);
    }
    
    int64 AfterReturnMemoryUsage = TestPool->GetMemoryUsage();
    
    // Memory usage should be consistent (objects are reused, not reallocated)
    TestTrue(TEXT("Memory usage should be tracked"), InitialMemoryUsage >= 0);
    TestTrue(TEXT("Memory usage should be consistent"), AfterReturnMemoryUsage == AfterRetrievalMemoryUsage);
}

void GWIZObjectPoolTests::TestThreadSafety()
{
    // Test concurrent access to pool
    const int32 ThreadCount = 4;
    const int32 OperationsPerThread = 100;
    
    TArray<TFuture<void>> ThreadFutures;
    
    for (int32 ThreadIndex = 0; ThreadIndex < ThreadCount; ++ThreadIndex)
    {
        TFuture<void> Future = Async(EAsyncExecution::ThreadPool, [this, OperationsPerThread, ThreadIndex]()
        {
            TArray<UObject*> ThreadObjects;
            
            for (int32 i = 0; i < OperationsPerThread; ++i)
            {
                // Randomly retrieve or return objects
                if (FMath::RandBool())
                {
                    UObject* Obj = TestPool->GetFromPool();
                    if (Obj != nullptr)
                    {
                        ThreadObjects.Add(Obj);
                    }
                }
                else if (ThreadObjects.Num() > 0)
                {
                    int32 Index = FMath::RandRange(0, ThreadObjects.Num() - 1);
                    UObject* Obj = ThreadObjects[Index];
                    ThreadObjects.RemoveAt(Index);
                    TestPool->ReturnToPool(Obj);
                }
            }
            
            // Return remaining objects
            for (UObject* Obj : ThreadObjects)
            {
                TestPool->ReturnToPool(Obj);
            }
        });
        
        ThreadFutures.Add(MoveTemp(Future));
    }
    
    // Wait for all threads to complete
    for (TFuture<void>& Future : ThreadFutures)
    {
        Future.Wait();
    }
    
    // Verify pool integrity
    TestTrue(TEXT("Pool should maintain integrity under concurrent access"), TestPool->IsValid());
    TestTrue(TEXT("Pool statistics should be consistent"), TestPool->GetStatistics().IsValid());
}

void GWIZObjectPoolTests::ValidatePoolState(const FString& TestName, int32 ExpectedSize, int32 ExpectedInUse)
{
    int32 ActualSize = TestPool->GetCurrentPoolSize();
    int32 ActualInUse = TestPool->GetObjectsInUse();
    
    TestEqual(*FString::Printf(TEXT("%s - Pool Size"), *TestName), ActualSize, ExpectedSize);
    TestEqual(*FString::Printf(TEXT("%s - Objects In Use"), *TestName), ActualInUse, ExpectedInUse);
}
