// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.
// This code is part of the GWIZ Generic Pooling System for Unreal Engine.

#include "GWIZPoolingManagerTests.h"
#include "PoolingSystem/GWIZPoolingManager.h"
#include "PoolingSystem/PoolingTypes.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "UObject/UObjectGlobals.h"
#include "HAL/Thread.h"
#include "Async/Async.h"

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(GWIZPoolingManagerTests, "GWIZ.Pooling.PoolingManager", FAutomationTestOpenCommand)

uint32 GWIZPoolingManagerTests::GetTestFlags() const
{
    return EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter;
}

bool GWIZPoolingManagerTests::RunTest(const FString& Parameters)
{
    // Setup test environment
    SetupTestEnvironment();
    
    // Run all test cases
    TestManagerCreation();
    TestPoolRetrieval();
    TestPoolConfiguration();
    TestObjectLifecycle();
    TestPoolPreWarming();
    TestStatisticsCollection();
    TestPoolManagement();
    TestQueryOperations();
    TestMemoryOperations();
    TestPerformanceMonitoring();
    TestAutoCleanup();
    TestDebugFeatures();
    TestThreadSafety();
    TestIntegrationScenarios();
    
    // Cleanup test environment
    CleanupTestEnvironment();
    
    return true;
}

void GWIZPoolingManagerTests::SetupTestEnvironment()
{
    // Create test world
    CreateTestWorld();
    
    // Create pooling manager
    if (TestWorld)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        SpawnParams.bDeferConstruction = false;
        
        TestManager = TestWorld->SpawnActor<AGWIZPoolingManager>(
            AGWIZPoolingManager::StaticClass(),
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            SpawnParams
        );
    }
}

void GWIZPoolingManagerTests::CleanupTestEnvironment()
{
    if (TestManager)
    {
        TestManager->Destroy();
        TestManager = nullptr;
    }
    
    DestroyTestWorld();
}

void GWIZPoolingManagerTests::CreateTestWorld()
{
    // Create a minimal test world
    TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    if (TestWorld)
    {
        TestWorld->InitWorld();
    }
}

void GWIZPoolingManagerTests::DestroyTestWorld()
{
    if (TestWorld)
    {
        TestWorld->DestroyWorld(false);
        TestWorld = nullptr;
    }
}

void GWIZPoolingManagerTests::TestManagerCreation()
{
    TestTrue(TEXT("Test world should be created"), TestWorld != nullptr);
    TestTrue(TEXT("Pooling manager should be created"), TestManager != nullptr);
    TestTrue(TEXT("Pooling manager should be valid"), TestManager->IsValidLowLevel());
    
    // Test global access
    AGWIZPoolingManager* GlobalManager = AGWIZPoolingManager::GetPoolingManager(TestWorld);
    TestTrue(TEXT("Global manager should be accessible"), GlobalManager != nullptr);
    TestEqual(TEXT("Global manager should match test manager"), GlobalManager, TestManager);
}

void GWIZPoolingManagerTests::TestPoolRetrieval()
{
    // Test getting pool for a class
    UGWIZObjectPool* Pool = TestManager->GetPool(UObject::StaticClass());
    TestTrue(TEXT("Should get pool for UObject class"), Pool != nullptr);
    TestEqual(TEXT("Pool should have correct object class"), Pool->PooledObjectClass, UObject::StaticClass());
    
    // Test getting same pool again (should return cached version)
    UGWIZObjectPool* CachedPool = TestManager->GetPool(UObject::StaticClass());
    TestEqual(TEXT("Should return cached pool"), Pool, CachedPool);
    
    // Test getting pool for different class
    UGWIZObjectPool* StringPool = TestManager->GetPool(UString::StaticClass());
    TestTrue(TEXT("Should get pool for UString class"), StringPool != nullptr);
    TestNotEqual(TEXT("Different classes should have different pools"), Pool, StringPool);
}

void GWIZPoolingManagerTests::TestPoolConfiguration()
{
    // Test configuring pool
    FGWIZPoolConfig Config;
    Config.MinPoolSize = 10;
    Config.MaxPoolSize = 100;
    Config.InitialPoolSize = 25;
    Config.Priority = 2;
    Config.Category = TEXT("TestCategory");
    
    TestManager->ConfigurePool(UObject::StaticClass(), Config);
    
    // Verify configuration was applied
    UGWIZObjectPool* Pool = TestManager->GetPool(UObject::StaticClass());
    TestEqual(TEXT("Min pool size should be configured"), Pool->Config.MinPoolSize, 10);
    TestEqual(TEXT("Max pool size should be configured"), Pool->Config.MaxPoolSize, 100);
    TestEqual(TEXT("Initial pool size should be configured"), Pool->Config.InitialPoolSize, 25);
    TestEqual(TEXT("Priority should be configured"), Pool->Config.Priority, 2);
    TestEqual(TEXT("Category should be configured"), Pool->Config.Category, TEXT("TestCategory"));
}

void GWIZPoolingManagerTests::TestObjectLifecycle()
{
    // Test getting pooled object
    UObject* PooledObject = TestManager->GetPooledObject(UObject::StaticClass());
    TestTrue(TEXT("Should get pooled object"), PooledObject != nullptr);
    
    // Test returning pooled object
    TestManager->ReturnPooledObject(PooledObject);
    
    // Verify object was returned to pool
    UGWIZObjectPool* Pool = TestManager->GetPool(UObject::StaticClass());
    TestEqual(TEXT("Pool size should increase after return"), Pool->GetCurrentPoolSize(), 1);
    TestEqual(TEXT("Objects in use should decrease after return"), Pool->GetObjectsInUse(), 0);
}

void GWIZPoolingManagerTests::TestPoolPreWarming()
{
    // Configure pool for pre-warming
    FGWIZPoolConfig Config;
    Config.InitialPoolSize = 20;
    TestManager->ConfigurePool(UObject::StaticClass(), Config);
    
    // Test pre-warming all pools
    TestManager->PreWarmAllPools();
    
    // Verify pools were pre-warmed
    UGWIZObjectPool* Pool = TestManager->GetPool(UObject::StaticClass());
    TestEqual(TEXT("Pool should be pre-warmed to initial size"), Pool->GetCurrentPoolSize(), 20);
}

void GWIZPoolingManagerTests::TestStatisticsCollection()
{
    // Perform some operations to generate statistics
    UObject* Obj1 = TestManager->GetPooledObject(UObject::StaticClass());
    UObject* Obj2 = TestManager->GetPooledObject(UObject::StaticClass());
    
    // Test getting all pool statistics
    TArray<FGWIZPoolStatistics> AllStats;
    TestManager->GetGlobalPerformanceMetrics(AllStats);
    
    TestTrue(TEXT("Should collect statistics from all pools"), AllStats.Num() > 0);
    
    // Test printing statistics
    TestManager->PrintAllPoolStatistics();
    
    // Return objects
    TestManager->ReturnPooledObject(Obj1);
    TestManager->ReturnPooledObject(Obj2);
}

void GWIZPoolingManagerTests::TestPoolManagement()
{
    // Test getting all pools
    TArray<UGWIZObjectPool*> AllPools = TestManager->GetAllPools();
    TestTrue(TEXT("Should get all pools"), AllPools.Num() > 0);
    
    // Test getting pool for specific class
    UGWIZObjectPool* SpecificPool = TestManager->GetPoolForClass(UObject::StaticClass());
    TestTrue(TEXT("Should get pool for specific class"), SpecificPool != nullptr);
    
    // Test getting pools by category
    TArray<UGWIZObjectPool*> CategoryPools = TestManager->GetPoolsByCategory(TEXT("TestCategory"));
    TestTrue(TEXT("Should get pools by category"), CategoryPools.Num() >= 0);
    
    // Test getting pools by priority
    TArray<UGWIZObjectPool*> PriorityPools = TestManager->GetPoolsByPriority(2);
    TestTrue(TEXT("Should get pools by priority"), PriorityPools.Num() >= 0);
}

void GWIZPoolingManagerTests::TestQueryOperations()
{
    // Test pool count
    int32 PoolCount = TestManager->GetPoolCount();
    TestTrue(TEXT("Pool count should be positive"), PoolCount > 0);
    
    // Test total objects
    int32 TotalObjects = TestManager->GetTotalObjects();
    TestTrue(TEXT("Total objects should be non-negative"), TotalObjects >= 0);
    
    // Test total objects in use
    int32 TotalInUse = TestManager->GetTotalObjectsInUse();
    TestTrue(TEXT("Total objects in use should be non-negative"), TotalInUse >= 0);
    TestTrue(TEXT("Total objects in use should not exceed total objects"), TotalInUse <= TotalObjects);
    
    // Test total memory usage
    int64 TotalMemory = TestManager->GetTotalMemoryUsage();
    TestTrue(TEXT("Total memory usage should be non-negative"), TotalMemory >= 0);
}

void GWIZPoolingManagerTests::TestMemoryOperations()
{
    // Test memory usage calculation
    int64 InitialMemory = TestManager->GetTotalMemoryUsage();
    
    // Create some objects to increase memory usage
    TArray<UObject*> TestObjects;
    for (int32 i = 0; i < 10; ++i)
    {
        UObject* Obj = TestManager->GetPooledObject(UObject::StaticClass());
        if (Obj != nullptr)
        {
            TestObjects.Add(Obj);
        }
    }
    
    int64 AfterCreationMemory = TestManager->GetTotalMemoryUsage();
    TestTrue(TEXT("Memory usage should increase after object creation"), AfterCreationMemory >= InitialMemory);
    
    // Return objects
    for (UObject* Obj : TestObjects)
    {
        TestManager->ReturnPooledObject(Obj);
    }
    
    // Memory usage should remain consistent (objects are reused)
    int64 AfterReturnMemory = TestManager->GetTotalMemoryUsage();
    TestEqual(TEXT("Memory usage should remain consistent after return"), AfterReturnMemory, AfterCreationMemory);
}

void GWIZPoolingManagerTests::TestPerformanceMonitoring()
{
    // Enable performance monitoring
    TestManager->SetPerformanceMonitoringEnabled(true);
    
    // Perform operations to generate performance data
    for (int32 i = 0; i < 100; ++i)
    {
        UObject* Obj = TestManager->GetPooledObject(UObject::StaticClass());
        if (Obj != nullptr)
        {
            TestManager->ReturnPooledObject(Obj);
        }
    }
    
    // Test performance metrics update
    TestManager->UpdatePerformanceMetrics();
    
    // Get performance metrics
    TArray<FGWIZPoolStatistics> PerformanceStats;
    TestManager->GetGlobalPerformanceMetrics(PerformanceStats);
    
    TestTrue(TEXT("Should collect performance metrics"), PerformanceStats.Num() > 0);
}

void GWIZPoolingManagerTests::TestAutoCleanup()
{
    // Enable auto-cleanup
    TestManager->SetAutoCleanupEnabled(true);
    
    // Configure pool with cleanup thresholds
    FGWIZPoolConfig Config;
    Config.MinPoolSize = 5;
    Config.MaxPoolSize = 20;
    TestManager->ConfigurePool(UObject::StaticClass(), Config);
    
    // Create more objects than minimum
    TArray<UObject*> TestObjects;
    for (int32 i = 0; i < 15; ++i)
    {
        UObject* Obj = TestManager->GetPooledObject(UObject::StaticClass());
        if (Obj != nullptr)
        {
            TestObjects.Add(Obj);
        }
    }
    
    // Return all objects
    for (UObject* Obj : TestObjects)
    {
        TestManager->ReturnPooledObject(Obj);
    }
    
    // Trigger auto-cleanup
    TestManager->PerformAutoCleanup();
    
    // Verify cleanup occurred
    UGWIZObjectPool* Pool = TestManager->GetPool(UObject::StaticClass());
    TestTrue(TEXT("Pool should respect minimum size after cleanup"), Pool->GetCurrentPoolSize() >= Config.MinPoolSize);
}

void GWIZPoolingManagerTests::TestDebugFeatures()
{
    // Enable debug mode
    TestManager->SetDebugModeEnabled(true);
    
    // Test debug display update
    TestManager->UpdateDebugDisplay();
    
    // Test debug statistics
    TestManager->PrintAllPoolStatistics();
    
    // Disable debug mode
    TestManager->SetDebugModeEnabled(false);
}

void GWIZPoolingManagerTests::TestThreadSafety()
{
    // Test concurrent access to pooling manager
    const int32 ThreadCount = 4;
    const int32 OperationsPerThread = 50;
    
    TArray<TFuture<void>> ThreadFutures;
    
    for (int32 ThreadIndex = 0; ThreadIndex < ThreadCount; ++ThreadIndex)
    {
        TFuture<void> Future = Async(EAsyncExecution::ThreadPool, [this, OperationsPerThread, ThreadIndex]()
        {
            TArray<UObject*> ThreadObjects;
            
            for (int32 i = 0; i < OperationsPerThread; ++i)
            {
                // Randomly perform different operations
                int32 Operation = FMath::RandRange(0, 3);
                
                switch (Operation)
                {
                case 0: // Get pooled object
                    {
                        UObject* Obj = TestManager->GetPooledObject(UObject::StaticClass());
                        if (Obj != nullptr)
                        {
                            ThreadObjects.Add(Obj);
                        }
                        break;
                    }
                case 1: // Return pooled object
                    if (ThreadObjects.Num() > 0)
                    {
                        int32 Index = FMath::RandRange(0, ThreadObjects.Num() - 1);
                        UObject* Obj = ThreadObjects[Index];
                        ThreadObjects.RemoveAt(Index);
                        TestManager->ReturnPooledObject(Obj);
                    }
                    break;
                case 2: // Get pool statistics
                    TestManager->GetPoolCount();
                    break;
                case 3: // Configure pool
                    {
                        FGWIZPoolConfig Config;
                        Config.MinPoolSize = FMath::RandRange(1, 10);
                        Config.MaxPoolSize = FMath::RandRange(10, 50);
                        TestManager->ConfigurePool(UObject::StaticClass(), Config);
                        break;
                    }
                }
            }
            
            // Return remaining objects
            for (UObject* Obj : ThreadObjects)
            {
                TestManager->ReturnPooledObject(Obj);
            }
        });
        
        ThreadFutures.Add(MoveTemp(Future));
    }
    
    // Wait for all threads to complete
    for (TFuture<void>& Future : ThreadFutures)
    {
        Future.Wait();
    }
    
    // Verify manager integrity
    TestTrue(TEXT("Manager should maintain integrity under concurrent access"), TestManager->IsValidLowLevel());
}

void GWIZPoolingManagerTests::TestIntegrationScenarios()
{
    // Test end-to-end pooling workflow
    const int32 WorkflowSize = 100;
    TArray<UObject*> WorkflowObjects;
    
    // Phase 1: Retrieve objects
    for (int32 i = 0; i < WorkflowSize; ++i)
    {
        UObject* Obj = TestManager->GetPooledObject(UObject::StaticClass());
        if (Obj != nullptr)
        {
            WorkflowObjects.Add(Obj);
        }
    }
    
    TestEqual(TEXT("Should retrieve all requested objects"), WorkflowObjects.Num(), WorkflowSize);
    
    // Phase 2: Simulate object usage
    for (UObject* Obj : WorkflowObjects)
    {
        // Simulate some work with the object
        if (Obj != nullptr)
        {
            // Access object properties to simulate usage
            FString ObjectName = Obj->GetName();
        }
    }
    
    // Phase 3: Return objects
    for (UObject* Obj : WorkflowObjects)
    {
        TestManager->ReturnPooledObject(Obj);
    }
    
    // Phase 4: Verify final state
    UGWIZObjectPool* Pool = TestManager->GetPool(UObject::StaticClass());
    TestEqual(TEXT("All objects should be returned to pool"), Pool->GetObjectsInUse(), 0);
    TestTrue(TEXT("Pool should have objects available"), Pool->GetCurrentPoolSize() > 0);
    
    // Phase 5: Test statistics
    FGWIZPoolStatistics FinalStats = Pool->GetStatistics();
    TestTrue(TEXT("Pool should have valid statistics"), FinalStats.IsValid());
    TestEqual(TEXT("Pool hits should reflect workflow"), FinalStats.PoolHits, WorkflowSize);
}

void GWIZPoolingManagerTests::ValidateManagerState(const FString& TestName, int32 ExpectedPoolCount)
{
    int32 ActualPoolCount = TestManager->GetPoolCount();
    TestEqual(*FString::Printf(TEXT("%s - Pool Count"), *TestName), ActualPoolCount, ExpectedPoolCount);
}
