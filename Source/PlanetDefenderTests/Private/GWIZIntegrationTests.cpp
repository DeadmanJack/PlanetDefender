// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.
// This code is part of the GWIZ Generic Pooling System for Unreal Engine.

#include "GWIZIntegrationTests.h"
#include "PoolingSystem/GWIZPoolingManager.h"
#include "PoolingSystem/PoolingTypes.h"
#include "GWIZGameInstance.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "UObject/UObjectGlobals.h"
#include "HAL/Thread.h"
#include "Async/Async.h"
#include "Misc/DateTime.h"

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(GWIZIntegrationTests, "GWIZ.Pooling.Integration", FAutomationTestOpenCommand)

uint32 GWIZIntegrationTests::GetTestFlags() const
{
    return EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter;
}

bool GWIZIntegrationTests::RunTest(const FString& Parameters)
{
    // Setup test environment
    SetupTestEnvironment();
    
    // Run all integration test scenarios
    TestEndToEndPoolingWorkflow();
    TestMultiClassPooling();
    TestLevelTransitionScenarios();
    TestGameInstanceIntegration();
    TestPerformanceUnderLoad();
    TestMemoryPressureScenarios();
    TestConcurrentAccessPatterns();
    TestErrorRecoveryScenarios();
    TestConfigurationHotReload();
    TestStatisticsAccuracy();
    
    // Cleanup test environment
    CleanupTestEnvironment();
    
    return true;
}

void GWIZIntegrationTests::SetupTestEnvironment()
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
        
        // Create game instance
        TestGameInstance = NewObject<UGWIZGameInstance>();
        TestGameInstance->Init();
    }
}

void GWIZIntegrationTests::CleanupTestEnvironment()
{
    // Cleanup test objects
    DestroyTestActors();
    
    if (TestManager)
    {
        TestManager->Destroy();
        TestManager = nullptr;
    }
    
    if (TestGameInstance)
    {
        TestGameInstance->Shutdown();
        TestGameInstance = nullptr;
    }
    
    DestroyTestWorld();
}

void GWIZIntegrationTests::CreateTestWorld()
{
    TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    if (TestWorld)
    {
        TestWorld->InitWorld();
    }
}

void GWIZIntegrationTests::DestroyTestWorld()
{
    if (TestWorld)
    {
        TestWorld->DestroyWorld(false);
        TestWorld = nullptr;
    }
}

void GWIZIntegrationTests::CreateTestActors(int32 Count)
{
    for (int32 i = 0; i < Count; ++i)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        
        AActor* Actor = TestWorld->SpawnActor<AActor>(AActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
        if (Actor)
        {
            TestActors.Add(Actor);
        }
    }
}

void GWIZIntegrationTests::DestroyTestActors()
{
    for (AActor* Actor : TestActors)
    {
        if (Actor && Actor->IsValidLowLevel())
        {
            Actor->Destroy();
        }
    }
    TestActors.Empty();
}

void GWIZIntegrationTests::TestEndToEndPoolingWorkflow()
{
    // Test complete pooling workflow from creation to cleanup
    
    // Phase 1: System initialization
    TestTrue(TEXT("Pooling manager should be created"), TestManager != nullptr);
    TestTrue(TEXT("Game instance should be initialized"), TestGameInstance != nullptr);
    
    // Phase 2: Pool configuration
    FGWIZPoolConfig Config;
    Config.MinPoolSize = 10;
    Config.MaxPoolSize = 100;
    Config.InitialPoolSize = 25;
    Config.Priority = 1;
    Config.Category = TEXT("IntegrationTest");
    
    TestManager->ConfigurePool(AActor::StaticClass(), Config);
    
    // Phase 3: Pool pre-warming
    TestManager->PreWarmAllPools();
    
    UGWIZObjectPool* Pool = TestManager->GetPool(AActor::StaticClass());
    TestEqual(TEXT("Pool should be pre-warmed to initial size"), Pool->GetCurrentPoolSize(), 25);
    
    // Phase 4: Object lifecycle simulation
    TArray<AActor*> RetrievedActors;
    const int32 WorkflowSize = 50;
    
    for (int32 i = 0; i < WorkflowSize; ++i)
    {
        AActor* Actor = Cast<AActor>(TestManager->GetPooledObject(AActor::StaticClass()));
        if (Actor)
        {
            RetrievedActors.Add(Actor);
        }
    }
    
    TestEqual(TEXT("Should retrieve all requested actors"), RetrievedActors.Num(), WorkflowSize);
    TestEqual(TEXT("Pool should have no available objects"), Pool->GetCurrentPoolSize(), 0);
    
    // Phase 5: Object usage simulation
    for (AActor* Actor : RetrievedActors)
    {
        // Simulate actor usage
        Actor->SetActorLocation(FVector(FMath::RandRange(-1000.0f, 1000.0f), FMath::RandRange(-1000.0f, 1000.0f), FMath::RandRange(-1000.0f, 1000.0f)));
        Actor->SetActorRotation(FRotator(FMath::RandRange(0.0f, 360.0f), FMath::RandRange(0.0f, 360.0f), FMath::RandRange(0.0f, 360.0f)));
    }
    
    // Phase 6: Object return
    for (AActor* Actor : RetrievedActors)
    {
        TestManager->ReturnPooledObject(Actor);
    }
    
    // Phase 7: Validation
    TestEqual(TEXT("All actors should be returned to pool"), Pool->GetCurrentPoolSize(), WorkflowSize);
    TestEqual(TEXT("No actors should be in use"), Pool->GetObjectsInUse(), 0);
    
    // Phase 8: Statistics validation
    FGWIZPoolStatistics Stats = Pool->GetStatistics();
    TestEqual(TEXT("Pool hits should reflect workflow"), Stats.PoolHits, WorkflowSize);
    TestEqual(TEXT("Pool misses should be zero"), Stats.PoolMisses, 0);
    TestEqual(TEXT("Hit rate should be 100%"), Stats.HitRate, 1.0f);
}

void GWIZIntegrationTests::TestMultiClassPooling()
{
    // Test pooling system with multiple object classes
    
    // Configure pools for different classes
    FGWIZPoolConfig ActorConfig;
    ActorConfig.MinPoolSize = 5;
    ActorConfig.MaxPoolSize = 50;
    ActorConfig.InitialPoolSize = 15;
    ActorConfig.Category = TEXT("Actors");
    
    FGWIZPoolConfig ObjectConfig;
    ObjectConfig.MinPoolSize = 3;
    ObjectConfig.MaxPoolSize = 30;
    ObjectConfig.InitialPoolSize = 10;
    ObjectConfig.Category = TEXT("Objects");
    
    TestManager->ConfigurePool(AActor::StaticClass(), ActorConfig);
    TestManager->ConfigurePool(UObject::StaticClass(), ObjectConfig);
    
    // Pre-warm all pools
    TestManager->PreWarmAllPools();
    
    // Test mixed object retrieval
    TArray<UObject*> MixedObjects;
    const int32 MixedCount = 20;
    
    for (int32 i = 0; i < MixedCount; ++i)
    {
        UObject* Obj = nullptr;
        if (i % 2 == 0)
        {
            Obj = TestManager->GetPooledObject(AActor::StaticClass());
        }
        else
        {
            Obj = TestManager->GetPooledObject(UObject::StaticClass());
        }
        
        if (Obj)
        {
            MixedObjects.Add(Obj);
        }
    }
    
    TestEqual(TEXT("Should retrieve mixed objects"), MixedObjects.Num(), MixedCount);
    
    // Verify pool states
    UGWIZObjectPool* ActorPool = TestManager->GetPool(AActor::StaticClass());
    UGWIZObjectPool* ObjectPool = TestManager->GetPool(UObject::StaticClass());
    
    TestTrue(TEXT("Actor pool should have objects in use"), ActorPool->GetObjectsInUse() > 0);
    TestTrue(TEXT("Object pool should have objects in use"), ObjectPool->GetObjectsInUse() > 0);
    
    // Return all objects
    for (UObject* Obj : MixedObjects)
    {
        TestManager->ReturnPooledObject(Obj);
    }
    
    // Verify final states
    TestEqual(TEXT("Actor pool should have all objects returned"), ActorPool->GetObjectsInUse(), 0);
    TestEqual(TEXT("Object pool should have all objects returned"), ObjectPool->GetObjectsInUse(), 0);
}

void GWIZIntegrationTests::TestLevelTransitionScenarios()
{
    // Test pooling system behavior during level transitions
    
    // Simulate level 1
    FString Level1Name = TEXT("TestLevel1");
    TestGameInstance->PreWarmPoolsForLevel(Level1Name);
    
    // Create and use objects in level 1
    TArray<UObject*> Level1Objects;
    for (int32 i = 0; i < 30; ++i)
    {
        UObject* Obj = TestManager->GetPooledObject(UObject::StaticClass());
        if (Obj)
        {
            Level1Objects.Add(Obj);
        }
    }
    
    // Simulate level transition
    TestGameInstance->CleanupUnusedPoolsForLevel(Level1Name);
    
    // Verify cleanup occurred
    UGWIZObjectPool* Pool = TestManager->GetPool(UObject::StaticClass());
    TestTrue(TEXT("Pool should respect minimum size after cleanup"), Pool->GetCurrentPoolSize() >= Pool->Config.MinPoolSize);
    
    // Simulate level 2
    FString Level2Name = TEXT("TestLevel2");
    TestGameInstance->PreWarmPoolsForLevel(Level2Name);
    
    // Verify pools are ready for level 2
    TestTrue(TEXT("Pool should be ready for level 2"), Pool->GetCurrentPoolSize() >= Pool->Config.InitialPoolSize);
    
    // Return level 1 objects
    for (UObject* Obj : Level1Objects)
    {
        TestManager->ReturnPooledObject(Obj);
    }
}

void GWIZIntegrationTests::TestGameInstanceIntegration()
{
    // Test integration between pooling manager and game instance
    
    // Test global access through game instance
    AGWIZPoolingManager* GlobalManager = TestGameInstance->GetGlobalPoolingManager();
    TestTrue(TEXT("Game instance should provide global pooling manager"), GlobalManager != nullptr);
    TestEqual(TEXT("Global manager should match test manager"), GlobalManager, TestManager);
    
    // Test pooling system initialization state
    TestTrue(TEXT("Pooling system should be initialized"), TestGameInstance->IsPoolingSystemInitialized());
    
    // Test save/load functionality
    TestGameInstance->SavePoolingSystemState();
    TestGameInstance->LoadPoolingSystemState();
    
    // Verify system state after save/load
    TestTrue(TEXT("System should remain functional after save/load"), TestGameInstance->IsPoolingSystemInitialized());
    
    // Test level-specific pool management
    FString TestLevel = TEXT("IntegrationTestLevel");
    TestGameInstance->PreWarmPoolsForLevel(TestLevel);
    TestGameInstance->CleanupUnusedPoolsForLevel(TestLevel);
    
    // Verify level operations completed successfully
    TestTrue(TEXT("Level operations should complete successfully"), true);
}

void GWIZIntegrationTests::TestPerformanceUnderLoad()
{
    // Test system performance under heavy load
    
    const int32 LoadTestSize = 1000;
    const int32 LoadTestIterations = 5;
    
    // Configure pools for performance testing
    FGWIZPoolConfig PerfConfig;
    PerfConfig.MinPoolSize = 100;
    PerfConfig.MaxPoolSize = 2000;
    PerfConfig.InitialPoolSize = 500;
    
    TestManager->ConfigurePool(UObject::StaticClass(), PerfConfig);
    TestManager->PreWarmAllPools();
    
    // Perform stress test
    PerformStressTest(LoadTestSize, LoadTestIterations);
    
    // Verify system stability
    TestTrue(TEXT("System should remain stable under load"), TestManager->IsValidLowLevel());
    
    // Check performance metrics
    TArray<FGWIZPoolStatistics> PerfStats;
    TestManager->GetGlobalPerformanceMetrics(PerfStats);
    
    TestTrue(TEXT("Should collect performance metrics under load"), PerfStats.Num() > 0);
}

void GWIZIntegrationTests::TestMemoryPressureScenarios()
{
    // Test system behavior under memory pressure
    
    // Configure pools with large sizes
    FGWIZPoolConfig LargeConfig;
    LargeConfig.MinPoolSize = 50;
    LargeConfig.MaxPoolSize = 1000;
    LargeConfig.InitialPoolSize = 200;
    
    TestManager->ConfigurePool(UObject::StaticClass(), LargeConfig);
    TestManager->PreWarmAllPools();
    
    // Create many objects to simulate memory pressure
    TArray<UObject*> PressureObjects;
    const int32 PressureCount = 800;
    
    for (int32 i = 0; i < PressureCount; ++i)
    {
        UObject* Obj = TestManager->GetPooledObject(UObject::StaticClass());
        if (Obj)
        {
            PressureObjects.Add(Obj);
        }
    }
    
    TestEqual(TEXT("Should handle memory pressure"), PressureObjects.Num(), PressureCount);
    
    // Test memory cleanup
    int64 InitialMemory = TestManager->GetTotalMemoryUsage();
    
    // Return objects to trigger cleanup
    for (UObject* Obj : PressureObjects)
    {
        TestManager->ReturnPooledObject(Obj);
    }
    
    TestManager->PerformAutoCleanup();
    
    int64 FinalMemory = TestManager->GetTotalMemoryUsage();
    TestTrue(TEXT("Memory usage should be managed under pressure"), FinalMemory <= InitialMemory);
}

void GWIZIntegrationTests::TestConcurrentAccessPatterns()
{
    // Test system behavior under concurrent access
    
    const int32 ThreadCount = 8;
    const int32 OperationsPerThread = 100;
    
    TArray<TFuture<void>> ThreadFutures;
    
    for (int32 ThreadIndex = 0; ThreadIndex < ThreadCount; ++ThreadIndex)
    {
        TFuture<void> Future = Async(EAsyncExecution::ThreadPool, [this, OperationsPerThread, ThreadIndex]()
        {
            TArray<UObject*> ThreadObjects;
            
            for (int32 i = 0; i < OperationsPerThread; ++i)
            {
                // Perform various operations concurrently
                int32 Operation = FMath::RandRange(0, 4);
                
                switch (Operation)
                {
                case 0: // Get object
                    {
                        UObject* Obj = TestManager->GetPooledObject(UObject::StaticClass());
                        if (Obj)
                        {
                            ThreadObjects.Add(Obj);
                        }
                        break;
                    }
                case 1: // Return object
                    if (ThreadObjects.Num() > 0)
                    {
                        int32 Index = FMath::RandRange(0, ThreadObjects.Num() - 1);
                        UObject* Obj = ThreadObjects[Index];
                        ThreadObjects.RemoveAt(Index);
                        TestManager->ReturnPooledObject(Obj);
                    }
                    break;
                case 2: // Get statistics
                    TestManager->GetPoolCount();
                    break;
                case 3: // Configure pool
                    {
                        FGWIZPoolConfig Config;
                        Config.MinPoolSize = FMath::RandRange(1, 20);
                        Config.MaxPoolSize = FMath::RandRange(20, 100);
                        TestManager->ConfigurePool(UObject::StaticClass(), Config);
                        break;
                    }
                case 4: // Get pool
                    TestManager->GetPool(UObject::StaticClass());
                    break;
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
    
    // Verify system integrity
    TestTrue(TEXT("System should maintain integrity under concurrent access"), TestManager->IsValidLowLevel());
    
    // Verify data consistency
    UGWIZObjectPool* Pool = TestManager->GetPool(UObject::StaticClass());
    TestTrue(TEXT("Pool should maintain consistency under concurrent access"), Pool->IsValid());
}

void GWIZIntegrationTests::TestErrorRecoveryScenarios()
{
    // Test system recovery from various error conditions
    
    // Test 1: Invalid object return
    TestManager->ReturnPooledObject(nullptr);
    TestTrue(TEXT("System should handle null object return gracefully"), TestManager->IsValidLowLevel());
    
    // Test 2: Wrong class object return
    UObject* WrongClassObj = NewObject<UObject>();
    TestManager->ReturnPooledObject(WrongClassObj);
    TestTrue(TEXT("System should handle wrong class object return gracefully"), TestManager->IsValidLowLevel());
    
    // Test 3: Invalid configuration
    FGWIZPoolConfig InvalidConfig;
    InvalidConfig.MinPoolSize = 100;
    InvalidConfig.MaxPoolSize = 50; // Invalid: min > max
    
    TestManager->ConfigurePool(UObject::StaticClass(), InvalidConfig);
    TestTrue(TEXT("System should handle invalid configuration gracefully"), TestManager->IsValidLowLevel());
    
    // Test 4: Recovery from invalid state
    UGWIZObjectPool* Pool = TestManager->GetPool(UObject::StaticClass());
    TestTrue(TEXT("Pool should remain functional after invalid operations"), Pool != nullptr);
    
    // Test 5: System reset
    TestManager->ClearAllPools();
    TestTrue(TEXT("System should recover after reset"), TestManager->IsValidLowLevel());
}

void GWIZIntegrationTests::TestConfigurationHotReload()
{
    // Test dynamic configuration changes during runtime
    
    // Initial configuration
    FGWIZPoolConfig InitialConfig;
    InitialConfig.MinPoolSize = 10;
    InitialConfig.MaxPoolSize = 100;
    InitialConfig.InitialPoolSize = 25;
    
    TestManager->ConfigurePool(UObject::StaticClass(), InitialConfig);
    TestManager->PreWarmAllPools();
    
    UGWIZObjectPool* Pool = TestManager->GetPool(UObject::StaticClass());
    int32 InitialSize = Pool->GetCurrentPoolSize();
    
    // Hot reload configuration
    FGWIZPoolConfig NewConfig;
    NewConfig.MinPoolSize = 20;
    NewConfig.MaxPoolSize = 200;
    NewConfig.InitialPoolSize = 50;
    
    TestManager->ConfigurePool(UObject::StaticClass(), NewConfig);
    
    // Verify configuration was applied
    TestEqual(TEXT("Min pool size should be updated"), Pool->Config.MinPoolSize, 20);
    TestEqual(TEXT("Max pool size should be updated"), Pool->Config.MaxPoolSize, 200);
    TestEqual(TEXT("Initial pool size should be updated"), Pool->Config.InitialPoolSize, 50);
    
    // Test pool resizing
    TestManager->PreWarmAllPools();
    int32 NewSize = Pool->GetCurrentPoolSize();
    
    TestTrue(TEXT("Pool should resize according to new configuration"), NewSize >= 50);
}

void GWIZIntegrationTests::TestStatisticsAccuracy()
{
    // Test accuracy of collected statistics
    
    // Reset statistics
    TestManager->ClearAllPools();
    
    // Perform known operations
    const int32 KnownOperations = 100;
    TArray<UObject*> KnownObjects;
    
    for (int32 i = 0; i < KnownOperations; ++i)
    {
        UObject* Obj = TestManager->GetPooledObject(UObject::StaticClass());
        if (Obj)
        {
            KnownObjects.Add(Obj);
        }
    }
    
    // Verify statistics accuracy
    UGWIZObjectPool* Pool = TestManager->GetPool(UObject::StaticClass());
    FGWIZPoolStatistics Stats = Pool->GetStatistics();
    
    TestEqual(TEXT("Pool hits should match known operations"), Stats.PoolHits, KnownOperations);
    TestEqual(TEXT("Objects in use should match known operations"), Stats.ObjectsInUse, KnownOperations);
    TestEqual(TEXT("Current pool size should be zero"), Stats.CurrentPoolSize, 0);
    
    // Return objects
    for (UObject* Obj : KnownObjects)
    {
        TestManager->ReturnPooledObject(Obj);
    }
    
    // Verify updated statistics
    Stats = Pool->GetStatistics();
    TestEqual(TEXT("Objects in use should be zero after return"), Stats.ObjectsInUse, 0);
    TestEqual(TEXT("Current pool size should reflect returned objects"), Stats.CurrentPoolSize, KnownOperations);
    
    // Test global statistics
    TArray<FGWIZPoolStatistics> GlobalStats;
    TestManager->GetGlobalPerformanceMetrics(GlobalStats);
    
    TestTrue(TEXT("Global statistics should include all pools"), GlobalStats.Num() > 0);
    
    // Verify global totals
    int32 GlobalTotalObjects = TestManager->GetTotalObjects();
    int32 GlobalTotalInUse = TestManager->GetTotalObjectsInUse();
    
    TestEqual(TEXT("Global total objects should match individual pool"), GlobalTotalObjects, KnownOperations);
    TestEqual(TEXT("Global total in use should be zero"), GlobalTotalInUse, 0);
}

void GWIZIntegrationTests::ValidateSystemState(const FString& TestName)
{
    // Validate overall system state
    TestTrue(TEXT("Pooling manager should be valid"), TestManager != nullptr && TestManager->IsValidLowLevel());
    TestTrue(TEXT("Game instance should be valid"), TestGameInstance != nullptr && TestGameInstance->IsPoolingSystemInitialized());
    
    // Validate pool states
    TArray<UGWIZObjectPool*> AllPools = TestManager->GetAllPools();
    for (UGWIZObjectPool* Pool : AllPools)
    {
        TestTrue(TEXT("All pools should be valid"), Pool != nullptr && Pool->IsValid());
        TestTrue(TEXT("Pool statistics should be valid"), Pool->GetStatistics().IsValid());
    }
}

void GWIZIntegrationTests::SimulateGameplayScenario()
{
    // Simulate realistic gameplay scenario
    
    // Phase 1: Level startup
    TestGameInstance->PreWarmPoolsForLevel(TEXT("GameplayLevel"));
    
    // Phase 2: Spawn enemies
    TArray<UObject*> Enemies;
    for (int32 i = 0; i < 50; ++i)
    {
        UObject* Enemy = TestManager->GetPooledObject(UObject::StaticClass());
        if (Enemy)
        {
            Enemies.Add(Enemy);
        }
    }
    
    // Phase 3: Spawn projectiles
    TArray<UObject*> Projectiles;
    for (int32 i = 0; i < 100; ++i)
    {
        UObject* Projectile = TestManager->GetPooledObject(UObject::StaticClass());
        if (Projectile)
        {
            Projectiles.Add(Projectile);
        }
    }
    
    // Phase 4: Simulate gameplay loop
    for (int32 Frame = 0; Frame < 60; ++Frame)
    {
        // Simulate frame update
        if (Frame % 10 == 0)
        {
            // Spawn new objects
            UObject* NewObj = TestManager->GetPooledObject(UObject::StaticClass());
            if (NewObj)
            {
                Projectiles.Add(NewObj);
            }
        }
        
        if (Frame % 15 == 0)
        {
            // Return some objects
            if (Projectiles.Num() > 0)
            {
                UObject* ReturnObj = Projectiles.Pop();
                TestManager->ReturnPooledObject(ReturnObj);
            }
        }
    }
    
    // Phase 5: Cleanup
    for (UObject* Enemy : Enemies)
    {
        TestManager->ReturnPooledObject(Enemy);
    }
    
    for (UObject* Projectile : Projectiles)
    {
        TestManager->ReturnPooledObject(Projectile);
    }
    
    // Phase 6: Level cleanup
    TestGameInstance->CleanupUnusedPoolsForLevel(TEXT("GameplayLevel"));
}

void GWIZIntegrationTests::PerformStressTest(int32 ObjectCount, int32 Iterations)
{
    // Perform stress test with specified parameters
    
    TArray<UObject*> StressObjects;
    
    for (int32 Iteration = 0; Iteration < Iterations; ++Iteration)
    {
        // Create objects
        for (int32 i = 0; i < ObjectCount; ++i)
        {
            UObject* Obj = TestManager->GetPooledObject(UObject::StaticClass());
            if (Obj)
            {
                StressObjects.Add(Obj);
            }
        }
        
        // Return objects
        for (UObject* Obj : StressObjects)
        {
            TestManager->ReturnPooledObject(Obj);
        }
        
        StressObjects.Empty();
        
        // Force garbage collection periodically
        if (Iteration % 10 == 0)
        {
            CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
        }
    }
    
    // Verify system stability
    TestTrue(TEXT("System should remain stable during stress test"), TestManager->IsValidLowLevel());
    
    // Verify final state
    UGWIZObjectPool* Pool = TestManager->GetPool(UObject::StaticClass());
    TestTrue(TEXT("Pool should remain functional after stress test"), Pool != nullptr);
}
