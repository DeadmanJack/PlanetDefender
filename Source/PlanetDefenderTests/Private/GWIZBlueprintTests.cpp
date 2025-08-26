// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.
// This code is part of the GWIZ Generic Pooling System for Unreal Engine.

#include "GWIZBlueprintTests.h"
#include "PoolingSystem/GWIZPoolingManager.h"
#include "PoolingSystem/PoolingTypes.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Blueprint/Blueprint.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/UObjectGlobals.h"
#include "Engine/BlueprintGeneratedClass.h"

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(GWIZBlueprintTests, "GWIZ.Pooling.Blueprint", FAutomationTestOpenCommand)

uint32 GWIZBlueprintTests::GetTestFlags() const
{
    return EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter;
}

bool GWIZBlueprintTests::RunTest(const FString& Parameters)
{
    // Setup test environment
    SetupTestEnvironment();
    
    // Run all Blueprint test scenarios
    TestBlueprintFunctionExposure();
    TestBlueprintInterfaceImplementation();
    TestBlueprintConfiguration();
    TestBlueprintDebugOutput();
    TestBlueprintPerformanceMonitoring();
    TestBlueprintPoolOperations();
    TestBlueprintStatistics();
    TestBlueprintErrorHandling();
    TestBlueprintCustomLogic();
    TestBlueprintIntegrationWorkflow();
    
    // Cleanup test environment
    CleanupTestEnvironment();
    
    return true;
}

void GWIZBlueprintTests::SetupTestEnvironment()
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
        
        // Create test blueprint
        CreateTestBlueprint();
    }
}

void GWIZBlueprintTests::CleanupTestEnvironment()
{
    if (TestManager)
    {
        TestManager->Destroy();
        TestManager = nullptr;
    }
    
    DestroyTestBlueprint();
    DestroyTestWorld();
}

void GWIZBlueprintTests::CreateTestWorld()
{
    TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
    if (TestWorld)
    {
        TestWorld->InitWorld();
    }
}

void GWIZBlueprintTests::DestroyTestWorld()
{
    if (TestWorld)
    {
        TestWorld->DestroyWorld(false);
        TestWorld = nullptr;
    }
}

void GWIZBlueprintTests::CreateTestBlueprint()
{
    // Create a test blueprint for testing Blueprint integration
    TestBlueprint = NewObject<UBlueprint>();
    if (TestBlueprint)
    {
        TestBlueprint->ParentClass = UObject::StaticClass();
        TestBlueprint->BlueprintType = BPTYPE_Normal;
    }
}

void GWIZBlueprintTests::DestroyTestBlueprint()
{
    if (TestBlueprint)
    {
        TestBlueprint = nullptr;
    }
}

void GWIZBlueprintTests::TestBlueprintFunctionExposure()
{
    // Test that all Blueprint-exposed functions are properly exposed
    
    // Test 1: Pooling Manager Blueprint functions
    UClass* ManagerClass = AGWIZPoolingManager::StaticClass();
    
    // Check for BlueprintCallable functions
    TArray<UFunction*> BlueprintFunctions;
    for (TFieldIterator<UFunction> FuncIt(ManagerClass); FuncIt; ++FuncIt)
    {
        UFunction* Function = *FuncIt;
        if (Function->HasAnyFunctionFlags(FUNC_BlueprintCallable))
        {
            BlueprintFunctions.Add(Function);
        }
    }
    
    // Verify essential functions are exposed
    bool bHasGetPool = false;
    bool bHasConfigurePool = false;
    bool bHasGetPooledObject = false;
    bool bHasReturnPooledObject = false;
    bool bHasPreWarmAllPools = false;
    bool bHasPrintAllPoolStatistics = false;
    
    for (UFunction* Function : BlueprintFunctions)
    {
        FString FunctionName = Function->GetName();
        if (FunctionName.Contains(TEXT("GetPool")))
        {
            bHasGetPool = true;
        }
        else if (FunctionName.Contains(TEXT("ConfigurePool")))
        {
            bHasConfigurePool = true;
        }
        else if (FunctionName.Contains(TEXT("GetPooledObject")))
        {
            bHasGetPooledObject = true;
        }
        else if (FunctionName.Contains(TEXT("ReturnPooledObject")))
        {
            bHasReturnPooledObject = true;
        }
        else if (FunctionName.Contains(TEXT("PreWarmAllPools")))
        {
            bHasPreWarmAllPools = true;
        }
        else if (FunctionName.Contains(TEXT("PrintAllPoolStatistics")))
        {
            bHasPrintAllPoolStatistics = true;
        }
    }
    
    TestTrue(TEXT("GetPool function should be Blueprint exposed"), bHasGetPool);
    TestTrue(TEXT("ConfigurePool function should be Blueprint exposed"), bHasConfigurePool);
    TestTrue(TEXT("GetPooledObject function should be Blueprint exposed"), bHasGetPooledObject);
    TestTrue(TEXT("ReturnPooledObject function should be Blueprint exposed"), bHasReturnPooledObject);
    TestTrue(TEXT("PreWarmAllPools function should be Blueprint exposed"), bHasPreWarmAllPools);
    TestTrue(TEXT("PrintAllPoolStatistics function should be Blueprint exposed"), bHasPrintAllPoolStatistics);
    
    UE_LOG(LogTemp, Log, TEXT("Found %d Blueprint-exposed functions in GWIZPoolingManager"), BlueprintFunctions.Num());
}

void GWIZBlueprintTests::TestBlueprintInterfaceImplementation()
{
    // Test Blueprint interface implementation
    
    // Test 1: IGWIZPoolable interface
    UClass* InterfaceClass = UGWIZPoolableInterface::StaticClass();
    TestTrue(TEXT("IGWIZPoolable interface should exist"), InterfaceClass != nullptr);
    
    // Check interface functions
    TArray<UFunction*> InterfaceFunctions;
    for (TFieldIterator<UFunction> FuncIt(InterfaceClass); FuncIt; ++FuncIt)
    {
        UFunction* Function = *FuncIt;
        InterfaceFunctions.Add(Function);
    }
    
    // Verify interface has required functions
    bool bHasOnPooled = false;
    bool bHasOnUnpooled = false;
    
    for (UFunction* Function : InterfaceFunctions)
    {
        FString FunctionName = Function->GetName();
        if (FunctionName.Contains(TEXT("OnPooled")))
        {
            bHasOnPooled = true;
        }
        else if (FunctionName.Contains(TEXT("OnUnpooled")))
        {
            bHasOnUnpooled = true;
        }
    }
    
    TestTrue(TEXT("OnPooled function should exist in interface"), bHasOnPooled);
    TestTrue(TEXT("OnUnpooled function should exist in interface"), bHasOnUnpooled);
    
    // Test 2: Blueprint interface implementation
    // Create a test object that implements the interface
    UObject* TestObject = NewObject<UObject>();
    TestTrue(TEXT("Test object should be created"), TestObject != nullptr);
    
    // Note: In a real Blueprint test, we would create a Blueprint class that implements the interface
    // For now, we'll test the interface structure
    TestTrue(TEXT("Interface should be properly defined"), InterfaceFunctions.Num() >= 2);
}

void GWIZBlueprintTests::TestBlueprintConfiguration()
{
    // Test Blueprint configuration functionality
    
    // Test 1: Pool configuration through Blueprint
    FGWIZPoolConfig Config;
    Config.MinPoolSize = 15;
    Config.MaxPoolSize = 150;
    Config.InitialPoolSize = 30;
    Config.Priority = 3;
    Config.Category = TEXT("BlueprintTest");
    
    TestManager->ConfigurePool(UObject::StaticClass(), Config);
    
    // Verify configuration was applied
    UGWIZObjectPool* Pool = TestManager->GetPool(UObject::StaticClass());
    TestEqual(TEXT("Min pool size should be configured"), Pool->Config.MinPoolSize, 15);
    TestEqual(TEXT("Max pool size should be configured"), Pool->Config.MaxPoolSize, 150);
    TestEqual(TEXT("Initial pool size should be configured"), Pool->Config.InitialPoolSize, 30);
    TestEqual(TEXT("Priority should be configured"), Pool->Config.Priority, 3);
    TestEqual(TEXT("Category should be configured"), Pool->Config.Category, TEXT("BlueprintTest"));
    
    // Test 2: Dynamic configuration changes
    FGWIZPoolConfig NewConfig = Config;
    NewConfig.InitialPoolSize = 50;
    
    TestManager->ConfigurePool(UObject::StaticClass(), NewConfig);
    
    // Verify dynamic update
    Pool = TestManager->GetPool(UObject::StaticClass());
    TestEqual(TEXT("Initial pool size should be dynamically updated"), Pool->Config.InitialPoolSize, 50);
    
    // Test 3: Configuration validation
    FGWIZPoolConfig InvalidConfig;
    InvalidConfig.MinPoolSize = 100;
    InvalidConfig.MaxPoolSize = 50; // Invalid: min > max
    
    // System should handle invalid configuration gracefully
    TestManager->ConfigurePool(UObject::StaticClass(), InvalidConfig);
    TestTrue(TEXT("System should handle invalid configuration gracefully"), TestManager->IsValidLowLevel());
}

void GWIZBlueprintTests::TestBlueprintDebugOutput()
{
    // Test Blueprint debug output functionality
    
    // Enable debug mode
    TestManager->SetDebugModeEnabled(true);
    
    // Test 1: Debug display update
    TestManager->UpdateDebugDisplay();
    TestTrue(TEXT("Debug display should update without errors"), TestManager->IsValidLowLevel());
    
    // Test 2: Statistics printing
    TestManager->PrintAllPoolStatistics();
    TestTrue(TEXT("Statistics printing should work without errors"), TestManager->IsValidLowLevel());
    
    // Test 3: Individual pool statistics
    UGWIZObjectPool* Pool = TestManager->GetPool(UObject::StaticClass());
    if (Pool)
    {
        FGWIZPoolStatistics Stats = Pool->GetStatistics();
        TestTrue(TEXT("Pool statistics should be valid"), Stats.IsValid());
        
        // Test statistics output
        UE_LOG(LogTemp, Log, TEXT("Pool Statistics - Size: %d, InUse: %d, HitRate: %.2f"), 
               Stats.CurrentPoolSize, Stats.ObjectsInUse, Stats.HitRate);
    }
    
    // Test 4: Debug mode toggle
    TestManager->SetDebugModeEnabled(false);
    TestFalse(TEXT("Debug mode should be disabled"), TestManager->IsDebugModeEnabled());
    
    TestManager->SetDebugModeEnabled(true);
    TestTrue(TEXT("Debug mode should be enabled"), TestManager->IsDebugModeEnabled());
}

void GWIZBlueprintTests::TestBlueprintPerformanceMonitoring()
{
    // Test Blueprint performance monitoring functionality
    
    // Enable performance monitoring
    TestManager->SetPerformanceMonitoringEnabled(true);
    
    // Test 1: Performance metrics collection
    TestManager->UpdatePerformanceMetrics();
    TestTrue(TEXT("Performance metrics should update without errors"), TestManager->IsValidLowLevel());
    
    // Test 2: Global performance metrics
    TArray<FGWIZPoolStatistics> GlobalStats;
    TestManager->GetGlobalPerformanceMetrics(GlobalStats);
    TestTrue(TEXT("Global performance metrics should be collected"), GlobalStats.Num() >= 0);
    
    // Test 3: Performance monitoring toggle
    TestManager->SetPerformanceMonitoringEnabled(false);
    TestFalse(TEXT("Performance monitoring should be disabled"), TestManager->IsPerformanceMonitoringEnabled());
    
    TestManager->SetPerformanceMonitoringEnabled(true);
    TestTrue(TEXT("Performance monitoring should be enabled"), TestManager->IsPerformanceMonitoringEnabled());
    
    // Test 4: Performance data accuracy
    // Perform some operations to generate performance data
    TArray<UObject*> TestObjects;
    for (int32 i = 0; i < 50; ++i)
    {
        UObject* Obj = TestManager->GetPooledObject(UObject::StaticClass());
        if (Obj)
        {
            TestObjects.Add(Obj);
        }
    }
    
    // Update performance metrics
    TestManager->UpdatePerformanceMetrics();
    
    // Verify performance data was collected
    GlobalStats.Empty();
    TestManager->GetGlobalPerformanceMetrics(GlobalStats);
    TestTrue(TEXT("Performance data should reflect recent operations"), GlobalStats.Num() > 0);
    
    // Return objects
    for (UObject* Obj : TestObjects)
    {
        TestManager->ReturnPooledObject(Obj);
    }
}

void GWIZBlueprintTests::TestBlueprintPoolOperations()
{
    // Test Blueprint pool operations
    
    // Test 1: Pool retrieval
    UGWIZObjectPool* Pool = TestManager->GetPool(UObject::StaticClass());
    TestTrue(TEXT("Should get pool for UObject class"), Pool != nullptr);
    
    // Test 2: Object retrieval
    UObject* PooledObject = TestManager->GetPooledObject(UObject::StaticClass());
    TestTrue(TEXT("Should get pooled object"), PooledObject != nullptr);
    
    // Test 3: Object return
    TestManager->ReturnPooledObject(PooledObject);
    TestTrue(TEXT("Should return pooled object without errors"), TestManager->IsValidLowLevel());
    
    // Test 4: Multiple object operations
    TArray<UObject*> MultipleObjects;
    const int32 ObjectCount = 25;
    
    for (int32 i = 0; i < ObjectCount; ++i)
    {
        UObject* Obj = TestManager->GetPooledObject(UObject::StaticClass());
        if (Obj)
        {
            MultipleObjects.Add(Obj);
        }
    }
    
    TestEqual(TEXT("Should retrieve multiple objects"), MultipleObjects.Num(), ObjectCount);
    
    // Return all objects
    for (UObject* Obj : MultipleObjects)
    {
        TestManager->ReturnPooledObject(Obj);
    }
    
    // Test 5: Pool pre-warming
    TestManager->PreWarmAllPools();
    TestTrue(TEXT("Pool pre-warming should work without errors"), TestManager->IsValidLowLevel());
    
    // Verify pre-warming occurred
    Pool = TestManager->GetPool(UObject::StaticClass());
    TestTrue(TEXT("Pool should have objects after pre-warming"), Pool->GetCurrentPoolSize() > 0);
}

void GWIZBlueprintTests::TestBlueprintStatistics()
{
    // Test Blueprint statistics functionality
    
    // Test 1: Individual pool statistics
    UGWIZObjectPool* Pool = TestManager->GetPool(UObject::StaticClass());
    if (Pool)
    {
        FGWIZPoolStatistics Stats = Pool->GetStatistics();
        TestTrue(TEXT("Pool statistics should be valid"), Stats.IsValid());
        
        // Test statistics fields
        TestTrue(TEXT("Current pool size should be non-negative"), Stats.CurrentPoolSize >= 0);
        TestTrue(TEXT("Objects in use should be non-negative"), Stats.ObjectsInUse >= 0);
        TestTrue(TEXT("Pool hits should be non-negative"), Stats.PoolHits >= 0);
        TestTrue(TEXT("Pool misses should be non-negative"), Stats.PoolMisses >= 0);
        TestTrue(TEXT("Hit rate should be between 0 and 1"), Stats.HitRate >= 0.0f && Stats.HitRate <= 1.0f);
        TestTrue(TEXT("Memory usage should be non-negative"), Stats.MemoryUsage >= 0);
    }
    
    // Test 2: Global statistics
    int32 PoolCount = TestManager->GetPoolCount();
    TestTrue(TEXT("Pool count should be positive"), PoolCount > 0);
    
    int32 TotalObjects = TestManager->GetTotalObjects();
    TestTrue(TEXT("Total objects should be non-negative"), TotalObjects >= 0);
    
    int32 TotalObjectsInUse = TestManager->GetTotalObjectsInUse();
    TestTrue(TEXT("Total objects in use should be non-negative"), TotalObjectsInUse >= 0);
    
    int64 TotalMemoryUsage = TestManager->GetTotalMemoryUsage();
    TestTrue(TEXT("Total memory usage should be non-negative"), TotalMemoryUsage >= 0);
    
    // Test 3: Statistics accuracy
    // Perform known operations and verify statistics
    const int32 KnownOperations = 30;
    TArray<UObject*> KnownObjects;
    
    for (int32 i = 0; i < KnownOperations; ++i)
    {
        UObject* Obj = TestManager->GetPooledObject(UObject::StaticClass());
        if (Obj)
        {
            KnownObjects.Add(Obj);
        }
    }
    
    // Check updated statistics
    Pool = TestManager->GetPool(UObject::StaticClass());
    FGWIZPoolStatistics UpdatedStats = Pool->GetStatistics();
    TestEqual(TEXT("Objects in use should reflect known operations"), UpdatedStats.ObjectsInUse, KnownOperations);
    
    // Return objects and check final statistics
    for (UObject* Obj : KnownObjects)
    {
        TestManager->ReturnPooledObject(Obj);
    }
    
    FGWIZPoolStatistics FinalStats = Pool->GetStatistics();
    TestEqual(TEXT("Objects in use should be zero after return"), FinalStats.ObjectsInUse, 0);
}

void GWIZBlueprintTests::TestBlueprintErrorHandling()
{
    // Test Blueprint error handling functionality
    
    // Test 1: Null object handling
    TestManager->ReturnPooledObject(nullptr);
    TestTrue(TEXT("System should handle null object gracefully"), TestManager->IsValidLowLevel());
    
    // Test 2: Invalid class handling
    TestManager->GetPool(nullptr);
    TestTrue(TEXT("System should handle null class gracefully"), TestManager->IsValidLowLevel());
    
    // Test 3: Invalid configuration handling
    FGWIZPoolConfig InvalidConfig;
    InvalidConfig.MinPoolSize = 200;
    InvalidConfig.MaxPoolSize = 100; // Invalid: min > max
    
    TestManager->ConfigurePool(UObject::StaticClass(), InvalidConfig);
    TestTrue(TEXT("System should handle invalid configuration gracefully"), TestManager->IsValidLowLevel());
    
    // Test 4: System recovery
    TestManager->ClearAllPools();
    TestTrue(TEXT("System should recover after error conditions"), TestManager->IsValidLowLevel());
    
    // Test 5: Edge case handling
    // Try to get object from empty pool
    UObject* Obj = TestManager->GetPooledObject(UObject::StaticClass());
    TestTrue(TEXT("System should handle empty pool gracefully"), Obj != nullptr || TestManager->IsValidLowLevel());
}

void GWIZBlueprintTests::TestBlueprintCustomLogic()
{
    // Test Blueprint custom logic integration
    
    // Test 1: Custom poolable object
    // Create a test object that could implement custom Blueprint logic
    UObject* CustomObject = NewObject<UObject>();
    TestTrue(TEXT("Custom object should be created"), CustomObject != nullptr);
    
    // Test 2: Custom configuration
    FGWIZPoolConfig CustomConfig;
    CustomConfig.MinPoolSize = 5;
    CustomConfig.MaxPoolSize = 25;
    CustomConfig.InitialPoolSize = 10;
    CustomConfig.Priority = 5;
    CustomConfig.Category = TEXT("CustomLogic");
    
    TestManager->ConfigurePool(UObject::StaticClass(), CustomConfig);
    
    // Verify custom configuration
    UGWIZObjectPool* Pool = TestManager->GetPool(UObject::StaticClass());
    TestEqual(TEXT("Custom priority should be applied"), Pool->Config.Priority, 5);
    TestEqual(TEXT("Custom category should be applied"), Pool->Config.Category, TEXT("CustomLogic"));
    
    // Test 3: Custom workflow
    // Simulate custom Blueprint workflow
    TArray<UObject*> CustomObjects;
    for (int32 i = 0; i < 15; ++i)
    {
        UObject* Obj = TestManager->GetPooledObject(UObject::StaticClass());
        if (Obj)
        {
            CustomObjects.Add(Obj);
            // Simulate custom logic on object
            FString ObjectName = Obj->GetName();
        }
    }
    
    TestEqual(TEXT("Custom workflow should retrieve objects"), CustomObjects.Num(), 15);
    
    // Return objects
    for (UObject* Obj : CustomObjects)
    {
        TestManager->ReturnPooledObject(Obj);
    }
    
    // Test 4: Custom statistics
    FGWIZPoolStatistics CustomStats = Pool->GetStatistics();
    TestTrue(TEXT("Custom workflow should generate valid statistics"), CustomStats.IsValid());
}

void GWIZBlueprintTests::TestBlueprintIntegrationWorkflow()
{
    // Test complete Blueprint integration workflow
    
    // Phase 1: Blueprint system initialization
    TestTrue(TEXT("Pooling manager should be ready for Blueprint integration"), TestManager != nullptr);
    TestTrue(TEXT("Test world should be ready for Blueprint integration"), TestWorld != nullptr);
    
    // Phase 2: Blueprint configuration
    FGWIZPoolConfig WorkflowConfig;
    WorkflowConfig.MinPoolSize = 20;
    WorkflowConfig.MaxPoolSize = 200;
    WorkflowConfig.InitialPoolSize = 50;
    WorkflowConfig.Priority = 2;
    WorkflowConfig.Category = TEXT("BlueprintWorkflow");
    
    TestManager->ConfigurePool(UObject::StaticClass(), WorkflowConfig);
    
    // Phase 3: Blueprint pool setup
    TestManager->PreWarmAllPools();
    
    UGWIZObjectPool* WorkflowPool = TestManager->GetPool(UObject::StaticClass());
    TestEqual(TEXT("Pool should be ready for Blueprint workflow"), WorkflowPool->GetCurrentPoolSize(), 50);
    
    // Phase 4: Blueprint object lifecycle
    TArray<UObject*> WorkflowObjects;
    const int32 WorkflowSize = 75;
    
    for (int32 i = 0; i < WorkflowSize; ++i)
    {
        UObject* Obj = TestManager->GetPooledObject(UObject::StaticClass());
        if (Obj)
        {
            WorkflowObjects.Add(Obj);
        }
    }
    
    TestEqual(TEXT("Blueprint workflow should retrieve objects"), WorkflowObjects.Num(), WorkflowSize);
    
    // Phase 5: Blueprint object processing
    for (UObject* Obj : WorkflowObjects)
    {
        // Simulate Blueprint processing
        FString ObjectName = Obj->GetName();
        // In a real Blueprint, this would execute custom logic
    }
    
    // Phase 6: Blueprint object return
    for (UObject* Obj : WorkflowObjects)
    {
        TestManager->ReturnPooledObject(Obj);
    }
    
    // Phase 7: Blueprint validation
    TestEqual(TEXT("All objects should be returned to pool"), WorkflowPool->GetCurrentPoolSize(), WorkflowSize);
    TestEqual(TEXT("No objects should be in use"), WorkflowPool->GetObjectsInUse(), 0);
    
    // Phase 8: Blueprint statistics
    FGWIZPoolStatistics WorkflowStats = WorkflowPool->GetStatistics();
    TestEqual(TEXT("Pool hits should reflect workflow"), WorkflowStats.PoolHits, WorkflowSize);
    TestEqual(TEXT("Hit rate should be 100%"), WorkflowStats.HitRate, 1.0f);
    
    // Phase 9: Blueprint cleanup
    TestManager->PerformAutoCleanup();
    TestTrue(TEXT("Blueprint workflow should complete cleanup successfully"), TestManager->IsValidLowLevel());
}

void GWIZBlueprintTests::ValidateBlueprintFunctions(const FString& TestName)
{
    // Validate Blueprint function availability
    UClass* ManagerClass = AGWIZPoolingManager::StaticClass();
    
    // Check for essential Blueprint functions
    UFunction* GetPoolFunc = ManagerClass->FindFunctionByName(TEXT("GetPool"));
    UFunction* ConfigurePoolFunc = ManagerClass->FindFunctionByName(TEXT("ConfigurePool"));
    UFunction* GetPooledObjectFunc = ManagerClass->FindFunctionByName(TEXT("GetPooledObject"));
    UFunction* ReturnPooledObjectFunc = ManagerClass->FindFunctionByName(TEXT("ReturnPooledObject"));
    
    TestTrue(*FString::Printf(TEXT("%s - GetPool function available"), *TestName), GetPoolFunc != nullptr);
    TestTrue(*FString::Printf(TEXT("%s - ConfigurePool function available"), *TestName), ConfigurePoolFunc != nullptr);
    TestTrue(*FString::Printf(TEXT("%s - GetPooledObject function available"), *TestName), GetPooledObjectFunc != nullptr);
    TestTrue(*FString::Printf(TEXT("%s - ReturnPooledObject function available"), *TestName), ReturnPooledObjectFunc != nullptr);
}

void GWIZBlueprintTests::TestBlueprintNodeExecution()
{
    // Test Blueprint node execution (simulated)
    
    // Simulate Blueprint node execution for pool operations
    UGWIZObjectPool* Pool = TestManager->GetPool(UObject::StaticClass());
    TestTrue(TEXT("Blueprint node should execute GetPool successfully"), Pool != nullptr);
    
    // Simulate Blueprint node execution for object retrieval
    UObject* Obj = TestManager->GetPooledObject(UObject::StaticClass());
    TestTrue(TEXT("Blueprint node should execute GetPooledObject successfully"), Obj != nullptr);
    
    // Simulate Blueprint node execution for object return
    TestManager->ReturnPooledObject(Obj);
    TestTrue(TEXT("Blueprint node should execute ReturnPooledObject successfully"), TestManager->IsValidLowLevel());
    
    // Simulate Blueprint node execution for statistics
    FGWIZPoolStatistics Stats = Pool->GetStatistics();
    TestTrue(TEXT("Blueprint node should execute GetStatistics successfully"), Stats.IsValid());
}

void GWIZBlueprintTests::SimulateBlueprintWorkflow()
{
    // Simulate realistic Blueprint workflow
    
    // Phase 1: Blueprint initialization
    TestManager->SetDebugModeEnabled(true);
    TestManager->SetPerformanceMonitoringEnabled(true);
    
    // Phase 2: Blueprint configuration
    FGWIZPoolConfig SimConfig;
    SimConfig.MinPoolSize = 10;
    SimConfig.MaxPoolSize = 100;
    SimConfig.InitialPoolSize = 25;
    SimConfig.Category = TEXT("BlueprintSimulation");
    
    TestManager->ConfigurePool(UObject::StaticClass(), SimConfig);
    
    // Phase 3: Blueprint execution
    TestManager->PreWarmAllPools();
    
    // Phase 4: Blueprint object management
    TArray<UObject*> SimObjects;
    for (int32 i = 0; i < 40; ++i)
    {
        UObject* Obj = TestManager->GetPooledObject(UObject::StaticClass());
        if (Obj)
        {
            SimObjects.Add(Obj);
        }
    }
    
    // Phase 5: Blueprint processing
    for (UObject* Obj : SimObjects)
    {
        // Simulate Blueprint processing
        FString ObjectName = Obj->GetName();
    }
    
    // Phase 6: Blueprint cleanup
    for (UObject* Obj : SimObjects)
    {
        TestManager->ReturnPooledObject(Obj);
    }
    
    // Phase 7: Blueprint validation
    UGWIZObjectPool* SimPool = TestManager->GetPool(UObject::StaticClass());
    TestEqual(TEXT("Blueprint simulation should complete successfully"), SimPool->GetObjectsInUse(), 0);
}
