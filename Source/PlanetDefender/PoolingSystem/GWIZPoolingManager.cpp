// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.
// This code is part of the GWIZ Generic Pooling System for Unreal Engine.

#include "GWIZPoolingManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "HAL/CriticalSection.h"

// Static instance for global access
static AGWIZPoolingManager* GGlobalPoolingManager = nullptr;
static FCriticalSection GGlobalPoolingManagerMutex;

// Sets default values
AGWIZPoolingManager::AGWIZPoolingManager()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGWIZPoolingManager::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize default configuration
    DefaultConfig = FGWIZPoolConfig();
    
    // Register as global instance if not already set
    FScopeLock Lock(&GGlobalPoolingManagerMutex);
    if (GGlobalPoolingManager == nullptr)
    {
        GGlobalPoolingManager = this;
    }
}

// Called every frame
void AGWIZPoolingManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Only run expensive operations occasionally
    static float TimeAccumulator = 0.0f;
    TimeAccumulator += DeltaTime;
    
    // Update every 0.5 seconds instead of every frame
    if (TimeAccumulator >= 0.5f)
    {
        TimeAccumulator = 0.0f;
        
        if (bEnablePerformanceMonitoring)
        {
            UpdatePerformanceMetrics();
        }
        
        if (bEnableAutoCleanup) PerformAutoCleanup();
    }
    
    // Debug display updates every frame (if enabled)
    if (bEnableDebugMode)
    {
        UpdateDebugDisplay();
    }
}

void AGWIZPoolingManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    
    // Cleanup all pools when actor is destroyed
    ClearAllPools();
    
    // Unregister as global instance if this is the global manager
    FScopeLock Lock(&GGlobalPoolingManagerMutex);
    if (GGlobalPoolingManager == this)
    {
        GGlobalPoolingManager = nullptr;
    }
}

AGWIZPoolingManager* AGWIZPoolingManager::GetPoolingManager(const UObject* WorldContext)
{
    // Thread-safe access to global instance
    FScopeLock Lock(&GGlobalPoolingManagerMutex);
    
    // Return existing global instance if available
    if (GGlobalPoolingManager != nullptr && GGlobalPoolingManager->IsValidLowLevel())
    {
        return GGlobalPoolingManager;
    }
    
    // Try to find existing instance in the world
    if (WorldContext != nullptr)
    {
        UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
        if (World != nullptr)
        {
            // Find existing pooling manager in the world
            TArray<AActor*> FoundActors;
            UGameplayStatics::GetAllActorsOfClass(World, AGWIZPoolingManager::StaticClass(), FoundActors);
            
            if (FoundActors.Num() > 0)
            {
                AGWIZPoolingManager* FoundManager = Cast<AGWIZPoolingManager>(FoundActors[0]);
                if (FoundManager != nullptr)
                {
                    GGlobalPoolingManager = FoundManager;
                    return FoundManager;
                }
            }
            
            // Create new instance if none exists
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            SpawnParams.bDeferConstruction = false;
            
            AGWIZPoolingManager* NewManager = World->SpawnActor<AGWIZPoolingManager>(AGWIZPoolingManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
            if (NewManager != nullptr)
            {
                GGlobalPoolingManager = NewManager;
                return NewManager;
            }
        }
    }
    
    return nullptr;
}

UGWIZObjectPool* AGWIZPoolingManager::GetPool(TSubclassOf<UObject> ObjectClass)
{
    // Validate input
    if (!ObjectClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("GWIZPoolingManager::GetPool - Invalid object class provided"));
        return nullptr;
    }

    // Thread-safe access to pools map
    FScopeLock Lock(&PoolMutex);
    
    // Check if pool already exists
    UGWIZObjectPool** ExistingPool = Pools.Find(ObjectClass);
    if (ExistingPool != nullptr && *ExistingPool != nullptr)
    {
        return *ExistingPool;
    }
    
    // Create new pool with default configuration
    UGWIZObjectPool* NewPool = NewObject<UGWIZObjectPool>(this);
    if (NewPool == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("GWIZPoolingManager::GetPool - Failed to create pool for class %s"), *ObjectClass->GetName());
        return nullptr;
    }
    
    // Initialize pool with default configuration
    NewPool->Config = DefaultConfig;
    NewPool->PooledObjectClass = ObjectClass;
    
    // Store pool in map
    Pools.Add(ObjectClass, NewPool);
    
    if (bEnableDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::GetPool - Created new pool for class %s"), *ObjectClass->GetName());
    }
    
    return NewPool;
}

void AGWIZPoolingManager::ConfigurePool(TSubclassOf<UObject> ObjectClass, const FGWIZPoolConfig& Config)
{
    // Validate input
    if (!ObjectClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("GWIZPoolingManager::ConfigurePool - Invalid object class provided"));
        return;
    }
    
    if (!Config.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("GWIZPoolingManager::ConfigurePool - Invalid configuration provided for class %s"), *ObjectClass->GetName());
        return;
    }
    
    // Thread-safe access to pools map
    FScopeLock Lock(&PoolMutex);
    
    // Get or create pool
    UGWIZObjectPool* Pool = GetPool(ObjectClass);
    if (Pool == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("GWIZPoolingManager::ConfigurePool - Failed to get/create pool for class %s"), *ObjectClass->GetName());
        return;
    }
    
    // Apply configuration
    Pool->Config = Config;
    
    if (bEnableDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::ConfigurePool - Configured pool for class %s (Min: %d, Max: %d, Initial: %d)"), 
               *ObjectClass->GetName(), Config.MinPoolSize, Config.MaxPoolSize, Config.InitialPoolSize);
    }
}

UObject* AGWIZPoolingManager::GetPooledObject(TSubclassOf<UObject> ObjectClass)
{
    // Validate input
    if (!ObjectClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("GWIZPoolingManager::GetPooledObject - Invalid object class provided"));
        return nullptr;
    }
    
    // Get pool for the object class
    UGWIZObjectPool* Pool = GetPool(ObjectClass);
    if (Pool == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("GWIZPoolingManager::GetPooledObject - Failed to get pool for class %s"), *ObjectClass->GetName());
        return nullptr;
    }
    
    // Get object from pool
    UObject* Object = Pool->GetObject(ObjectClass);
    if (Object == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("GWIZPoolingManager::GetPooledObject - Failed to get object from pool for class %s"), *ObjectClass->GetName());
        return nullptr;
    }
    
    // Initialize object if it implements the poolable interface
    if (Object->GetClass()->ImplementsInterface(UGWIZPoolable::StaticClass()))
    {
        IGWIZPoolable::Execute_OnPooled(Object);
    }
    
    // Handle actor-specific initialization
    if (AActor* Actor = Cast<AActor>(Object))
    {
        // Enable ticking if needed
        if (Actor->PrimaryActorTick.bCanEverTick)
        {
            Actor->SetActorTickEnabled(true);
        }
        
        // Enable rendering
        Actor->SetActorHiddenInGame(false);
        
        // Enable collision
        Actor->SetActorEnableCollision(true);
    }
    
    if (bEnableDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::GetPooledObject - Retrieved object of class %s from pool"), *ObjectClass->GetName());
    }
    
    return Object;
}

void AGWIZPoolingManager::ReturnPooledObject(UObject* Object)
{
    // Validate input
    if (!Object)
    {
        UE_LOG(LogTemp, Warning, TEXT("GWIZPoolingManager::ReturnPooledObject - Invalid object provided"));
        return;
    }
    
    // Get the object's class
    UClass* ObjectClass = Object->GetClass();
    if (!ObjectClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("GWIZPoolingManager::ReturnPooledObject - Object has no valid class"));
        return;
    }
    
    // Find the pool for this object class
    UGWIZObjectPool* Pool = GetPoolForClass(ObjectClass);
    if (Pool == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("GWIZPoolingManager::ReturnPooledObject - No pool found for class %s"), *ObjectClass->GetName());
        return;
    }
    
    // Clean up object if it implements the poolable interface
    if (Object->GetClass()->ImplementsInterface(UGWIZPoolable::StaticClass()))
    {
        IGWIZPoolable::Execute_OnUnpooled(Object);
    }
    
    // Handle actor-specific cleanup
    if (AActor* Actor = Cast<AActor>(Object))
    {
        // Disable ticking
        Actor->SetActorTickEnabled(false);
        
        // Hide from game
        Actor->SetActorHiddenInGame(true);
        
        // Disable collision
        Actor->SetActorEnableCollision(false);
        
        // Reset location to origin to avoid interference
        Actor->SetActorLocation(FVector::ZeroVector);
    }
    
    // Return object to pool
    Pool->ReturnObject(Object);
    
    if (bEnableDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::ReturnPooledObject - Returned object of class %s to pool"), *ObjectClass->GetName());
    }
}

void AGWIZPoolingManager::PreWarmAllPools()
{
    // Thread-safe access to pools map
    FScopeLock Lock(&PoolMutex);
    
    int32 TotalPools = Pools.Num();
    int32 ProcessedPools = 0;
    
    if (bEnableDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::PreWarmAllPools - Starting pre-warming of %d pools"), TotalPools);
    }
    
    // Pre-warm each pool
    for (auto& PoolPair : Pools)
    {
        UGWIZObjectPool* Pool = PoolPair.Value;
        if (Pool != nullptr)
        {
            // Pre-warm pool with initial size from configuration
            Pool->PreWarmPool(Pool->Config.InitialPoolSize);
            ProcessedPools++;
            
            if (bEnableDebugMode)
            {
                UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::PreWarmAllPools - Pre-warmed pool for class %s with %d objects"), 
                       *PoolPair.Key->GetName(), Pool->Config.InitialPoolSize);
            }
        }
    }
    
    if (bEnableDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::PreWarmAllPools - Completed pre-warming of %d/%d pools"), ProcessedPools, TotalPools);
    }
}

void AGWIZPoolingManager::PrintAllPoolStatistics()
{
    // Thread-safe access to pools map
    FScopeLock Lock(&PoolMutex);
    
    int32 TotalPools = Pools.Num();
    if (TotalPools == 0)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::PrintAllPoolStatistics - No pools to display"));
        return;
    }
    
    // Print header
    UE_LOG(LogTemp, Log, TEXT("=== GWIZ Pooling System Statistics ==="));
    UE_LOG(LogTemp, Log, TEXT("Total Pools: %d"), TotalPools);
    UE_LOG(LogTemp, Log, TEXT(""));
    
    // Calculate global totals
    int32 TotalObjects = 0;
    int32 TotalObjectsInUse = 0;
    int64 TotalMemoryUsage = 0;
    int32 TotalPoolHits = 0;
    int32 TotalPoolMisses = 0;
    
    // Print individual pool statistics
    for (auto& PoolPair : Pools)
    {
        UGWIZObjectPool* Pool = PoolPair.Value;
        if (Pool != nullptr)
        {
            FGWIZPoolStatistics Stats = Pool->GetStatistics();
            
            // Update global totals
            TotalObjects += Stats.GetTotalObjects();
            TotalObjectsInUse += Stats.ObjectsInUse;
            TotalMemoryUsage += Stats.MemoryUsage;
            TotalPoolHits += Stats.PoolHits;
            TotalPoolMisses += Stats.PoolMisses;
            
            // Print pool details
            UE_LOG(LogTemp, Log, TEXT("Pool: %s"), *PoolPair.Key->GetName());
            UE_LOG(LogTemp, Log, TEXT("  Configuration: Min=%d, Max=%d, Initial=%d"), 
                   Pool->Config.MinPoolSize, Pool->Config.MaxPoolSize, Pool->Config.InitialPoolSize);
            UE_LOG(LogTemp, Log, TEXT("  Current Pool Size: %d"), Stats.CurrentPoolSize);
            UE_LOG(LogTemp, Log, TEXT("  Objects In Use: %d"), Stats.ObjectsInUse);
            UE_LOG(LogTemp, Log, TEXT("  Total Objects Created: %d"), Stats.TotalObjectsCreated);
            UE_LOG(LogTemp, Log, TEXT("  Pool Hits: %d"), Stats.PoolHits);
            UE_LOG(LogTemp, Log, TEXT("  Pool Misses: %d"), Stats.PoolMisses);
            UE_LOG(LogTemp, Log, TEXT("  Hit Rate: %.2f%%"), Stats.HitRate * 100.0f);
            UE_LOG(LogTemp, Log, TEXT("  Memory Usage: %lld bytes (%.2f MB)"), 
                   Stats.MemoryUsage, Stats.MemoryUsage / (1024.0f * 1024.0f));
            UE_LOG(LogTemp, Log, TEXT("  Average Lifetime: %.2f seconds"), Stats.AverageLifetime);
            UE_LOG(LogTemp, Log, TEXT("  Peak Concurrent Usage: %d"), Stats.PeakConcurrentUsage);
            UE_LOG(LogTemp, Log, TEXT(""));
        }
    }
    
    // Print global summary
    UE_LOG(LogTemp, Log, TEXT("=== Global Summary ==="));
    UE_LOG(LogTemp, Log, TEXT("Total Objects: %d"), TotalObjects);
    UE_LOG(LogTemp, Log, TEXT("Total Objects In Use: %d"), TotalObjectsInUse);
    UE_LOG(LogTemp, Log, TEXT("Total Memory Usage: %lld bytes (%.2f MB)"), 
           TotalMemoryUsage, TotalMemoryUsage / (1024.0f * 1024.0f));
    
    const int32 TotalAccesses = TotalPoolHits + TotalPoolMisses;
    const float GlobalHitRate = TotalAccesses > 0 ? static_cast<float>(TotalPoolHits) / static_cast<float>(TotalAccesses) : 0.0f;
    UE_LOG(LogTemp, Log, TEXT("Global Hit Rate: %.2f%%"), GlobalHitRate * 100.0f);
    UE_LOG(LogTemp, Log, TEXT("Total Pool Hits: %d"), TotalPoolHits);
    UE_LOG(LogTemp, Log, TEXT("Total Pool Misses: %d"), TotalPoolMisses);
    UE_LOG(LogTemp, Log, TEXT("=== End Statistics ==="));
}

void AGWIZPoolingManager::GetGlobalPerformanceMetrics(TArray<FGWIZPoolStatistics>& AllStats)
{
    // Clear output array
    AllStats.Empty();
    
    // Thread-safe access to pools map
    FScopeLock Lock(&PoolMutex);
    
    // Collect statistics from all pools
    for (auto& PoolPair : Pools)
    {
        UGWIZObjectPool* Pool = PoolPair.Value;
        if (Pool != nullptr)
        {
            FGWIZPoolStatistics Stats = Pool->GetStatistics();
            AllStats.Add(Stats);
        }
    }
    
    // Store historical data for trend analysis
    if (bEnablePerformanceMonitoring)
    {
        HistoricalStats.Append(AllStats);
        
        // Keep only last 100 entries to prevent memory bloat
        const int32 MaxHistoricalEntries = 100;
        if (HistoricalStats.Num() > MaxHistoricalEntries)
        {
            HistoricalStats.RemoveAt(0, HistoricalStats.Num() - MaxHistoricalEntries);
        }
    }
    
    if (bEnableDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::GetGlobalPerformanceMetrics - Collected metrics from %d pools"), AllStats.Num());
    }
}

TArray<UGWIZObjectPool*> AGWIZPoolingManager::GetAllPools() const
{
    TArray<UGWIZObjectPool*> Result;
    
    // Thread-safe access to pools map
    FScopeLock Lock(&PoolMutex);
    
    // Copy all pools to result array
    Pools.GenerateValueArray(Result);
    
    // Filter out null pointers
    Result.RemoveAll([](UGWIZObjectPool* Pool) { return Pool == nullptr; });
    
    if (bEnableDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::GetAllPools - Retrieved %d valid pools"), Result.Num());
    }
    
    return Result;
}

int32 AGWIZPoolingManager::GetPoolCount() const
{
    return Pools.Num();
}

void AGWIZPoolingManager::ClearAllPools()
{
    // Thread-safe access to pools map
    FScopeLock Lock(&PoolMutex);
    
    int32 TotalPools = Pools.Num();
    int32 ClearedPools = 0;
    
    if (bEnableDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::ClearAllPools - Starting cleanup of %d pools"), TotalPools);
    }
    
    // Clear each pool
    for (auto& PoolPair : Pools)
    {
        UGWIZObjectPool* Pool = PoolPair.Value;
        if (Pool != nullptr)
        {
            Pool->ClearPool();
            ClearedPools++;
            
            if (bEnableDebugMode)
            {
                UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::ClearAllPools - Cleared pool for class %s"), *PoolPair.Key->GetName());
            }
        }
    }
    
    // Clear the pools map
    Pools.Empty();
    
    // Clear historical statistics
    HistoricalStats.Empty();
    
    if (bEnableDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::ClearAllPools - Completed cleanup of %d/%d pools"), ClearedPools, TotalPools);
    }
}

UGWIZObjectPool* AGWIZPoolingManager::GetPoolForClass(TSubclassOf<UObject> ObjectClass) const
{
    // Validate input
    if (!ObjectClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("GWIZPoolingManager::GetPoolForClass - Invalid object class provided"));
        return nullptr;
    }
    
    // Thread-safe access to pools map
    FScopeLock Lock(&PoolMutex);
    
    // Try to find exact class match first
    UGWIZObjectPool** FoundPool = Pools.Find(ObjectClass);
    if (FoundPool != nullptr && *FoundPool != nullptr)
    {
        return *FoundPool;
    }
    
    // If no exact match, try to find a pool for a parent class
    UClass* CurrentClass = ObjectClass;
    while (CurrentClass != nullptr)
    {
        UClass* ParentClass = CurrentClass->GetSuperClass();
        if (ParentClass != nullptr && ParentClass != UObject::StaticClass())
        {
            UGWIZObjectPool** ParentPool = Pools.Find(ParentClass);
            if (ParentPool != nullptr && *ParentPool != nullptr)
            {
                if (bEnableDebugMode)
                {
                    UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::GetPoolForClass - Found pool for parent class %s for requested class %s"), 
                           *ParentClass->GetName(), *ObjectClass->GetName());
                }
                return *ParentPool;
            }
        }
        CurrentClass = ParentClass;
    }
    
    if (bEnableDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::GetPoolForClass - No pool found for class %s"), *ObjectClass->GetName());
    }
    
    return nullptr;
}

TArray<UGWIZObjectPool*> AGWIZPoolingManager::GetPoolsByCategory(const FString& Category) const
{
    TArray<UGWIZObjectPool*> Result;
    
    // TODO: Implement category system when PoolingTypes.h is updated with category support
    // For now, return empty array as category system is not yet implemented
    
    if (bEnableDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::GetPoolsByCategory - Category system not yet implemented. Requested category: %s"), *Category);
    }
    
    return Result;
}

TArray<UGWIZObjectPool*> AGWIZPoolingManager::GetPoolsByPriority(int32 Priority) const
{
    TArray<UGWIZObjectPool*> Result;
    
    // TODO: Implement priority system when PoolingTypes.h is updated with priority support
    // For now, return empty array as priority system is not yet implemented
    
    if (bEnableDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::GetPoolsByPriority - Priority system not yet implemented. Requested priority: %d"), Priority);
    }
    
    return Result;
}

void AGWIZPoolingManager::CleanupUnusedPools()
{
    // Thread-safe access to pools map
    FScopeLock Lock(&PoolMutex);
    
    int32 TotalPools = Pools.Num();
    int32 CleanedPools = 0;
    
    if (bEnableDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::CleanupUnusedPools - Starting cleanup of %d pools"), TotalPools);
    }
    
    // Find pools to clean up (pools with no objects in use and excess objects)
    TArray<TSubclassOf<UObject>> PoolsToCleanup;
    
    for (auto& PoolPair : Pools)
    {
        UGWIZObjectPool* Pool = PoolPair.Value;
        if (Pool != nullptr)
        {
            FGWIZPoolStatistics Stats = Pool->GetStatistics();
            
            // Clean up if no objects are in use and pool size exceeds minimum
            if (Stats.ObjectsInUse == 0 && Stats.CurrentPoolSize > Pool->Config.MinPoolSize)
            {
                PoolsToCleanup.Add(PoolPair.Key);
            }
        }
    }
    
    // Clean up identified pools
    for (TSubclassOf<UObject> ClassToCleanup : PoolsToCleanup)
    {
        UGWIZObjectPool** PoolPtr = Pools.Find(ClassToCleanup);
        if (PoolPtr != nullptr && *PoolPtr != nullptr)
        {
            UGWIZObjectPool* Pool = *PoolPtr;
            FGWIZPoolStatistics Stats = Pool->GetStatistics();
            
            // Reduce pool size to minimum
            int32 ObjectsToRemove = Stats.CurrentPoolSize - Pool->Config.MinPoolSize;
            if (ObjectsToRemove > 0)
            {
                // Remove excess objects from pool
                for (int32 i = 0; i < ObjectsToRemove; ++i)
                {
                    Pool->RemoveFromPool(nullptr); // Remove from available pool
                }
                
                CleanedPools++;
                
                if (bEnableDebugMode)
                {
                    UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::CleanupUnusedPools - Cleaned up %d objects from pool %s"), 
                           ObjectsToRemove, *ClassToCleanup->GetName());
                }
            }
        }
    }
    
    if (bEnableDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::CleanupUnusedPools - Completed cleanup of %d pools"), CleanedPools);
    }
}

int64 AGWIZPoolingManager::GetTotalMemoryUsage() const
{
    int64 TotalMemoryUsage = 0;
    
    // Thread-safe access to pools map
    FScopeLock Lock(&PoolMutex);
    
    // Calculate memory usage from all pools
    for (auto& PoolPair : Pools)
    {
        UGWIZObjectPool* Pool = PoolPair.Value;
        if (Pool != nullptr)
        {
            TotalMemoryUsage += Pool->GetMemoryUsage();
        }
    }
    
    if (bEnableDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::GetTotalMemoryUsage - Total memory usage: %lld bytes (%.2f MB)"), 
               TotalMemoryUsage, TotalMemoryUsage / (1024.0f * 1024.0f));
    }
    
    return TotalMemoryUsage;
}

int32 AGWIZPoolingManager::GetTotalObjects() const
{
    int32 TotalObjects = 0;
    
    // Thread-safe access to pools map
    FScopeLock Lock(&PoolMutex);
    
    // Calculate total objects from all pools
    for (auto& PoolPair : Pools)
    {
        UGWIZObjectPool* Pool = PoolPair.Value;
        if (Pool != nullptr)
        {
            FGWIZPoolStatistics Stats = Pool->GetStatistics();
            TotalObjects += Stats.GetTotalObjects();
        }
    }
    
    if (bEnableDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::GetTotalObjects - Total objects across all pools: %d"), TotalObjects);
    }
    
    return TotalObjects;
}

int32 AGWIZPoolingManager::GetTotalObjectsInUse() const
{
    int32 TotalObjectsInUse = 0;
    
    // Thread-safe access to pools map
    FScopeLock Lock(&PoolMutex);
    
    // Calculate total objects in use from all pools
    for (auto& PoolPair : Pools)
    {
        UGWIZObjectPool* Pool = PoolPair.Value;
        if (Pool != nullptr)
        {
            FGWIZPoolStatistics Stats = Pool->GetStatistics();
            TotalObjectsInUse += Stats.ObjectsInUse;
        }
    }
    
    if (bEnableDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::GetTotalObjectsInUse - Total objects in use across all pools: %d"), TotalObjectsInUse);
    }
    
    return TotalObjectsInUse;
}

void AGWIZPoolingManager::UpdatePerformanceMetrics()
{
    if (!bEnablePerformanceMonitoring)
    {
        return;
    }
    
    // Collect current performance data
    TArray<FGWIZPoolStatistics> CurrentStats;
    GetGlobalPerformanceMetrics(CurrentStats);
    
    // Calculate performance trends
    if (HistoricalStats.Num() > 0)
    {
        // Calculate memory usage trend
        int64 CurrentMemoryUsage = GetTotalMemoryUsage();
        int64 PreviousMemoryUsage = 0;
        
        if (HistoricalStats.Num() >= 2)
        {
            // Get memory usage from previous frame
            for (const FGWIZPoolStatistics& Stat : HistoricalStats)
            {
                PreviousMemoryUsage += Stat.MemoryUsage;
            }
            PreviousMemoryUsage /= HistoricalStats.Num();
        }
        
        // Calculate memory growth rate
        float MemoryGrowthRate = 0.0f;
        if (PreviousMemoryUsage > 0)
        {
            MemoryGrowthRate = static_cast<float>(CurrentMemoryUsage - PreviousMemoryUsage) / static_cast<float>(PreviousMemoryUsage);
        }
        
        // Log performance alerts if thresholds are exceeded
        const float MemoryGrowthThreshold = 0.1f; // 10% growth
        if (MemoryGrowthRate > MemoryGrowthThreshold)
        {
            UE_LOG(LogTemp, Warning, TEXT("GWIZPoolingManager::UpdatePerformanceMetrics - High memory growth detected: %.2f%%"), MemoryGrowthRate * 100.0f);
        }
        
        // Calculate hit rate trends
        float AverageHitRate = 0.0f;
        int32 TotalAccesses = 0;
        
        for (const FGWIZPoolStatistics& Stat : CurrentStats)
        {
            const int32 Accesses = Stat.PoolHits + Stat.PoolMisses;
            if (Accesses > 0)
            {
                AverageHitRate += Stat.HitRate * Accesses;
                TotalAccesses += Accesses;
            }
        }
        
        if (TotalAccesses > 0)
        {
            AverageHitRate /= TotalAccesses;
            
            // Log low hit rate warnings
            const float LowHitRateThreshold = 0.5f; // 50%
            if (AverageHitRate < LowHitRateThreshold)
            {
                UE_LOG(LogTemp, Warning, TEXT("GWIZPoolingManager::UpdatePerformanceMetrics - Low hit rate detected: %.2f%%"), AverageHitRate * 100.0f);
            }
        }
    }
    
    if (bEnableDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::UpdatePerformanceMetrics - Updated performance metrics for %d pools"), CurrentStats.Num());
    }
}

void AGWIZPoolingManager::PerformAutoCleanup()
{
    if (!bEnableAutoCleanup)
    {
        return;
    }
    
    // Thread-safe access to pools map
    FScopeLock Lock(&PoolMutex);
    
    int32 CleanedPools = 0;
    int32 TotalObjectsRemoved = 0;
    
    // Check each pool for cleanup conditions
    for (auto& PoolPair : Pools)
    {
        UGWIZObjectPool* Pool = PoolPair.Value;
        if (Pool != nullptr)
        {
            FGWIZPoolStatistics Stats = Pool->GetStatistics();
            
            // Cleanup conditions:
            // 1. No objects in use
            // 2. Pool size exceeds minimum
            // 3. Pool has been unused for a while (based on hit rate)
            bool ShouldCleanup = false;
            int32 ObjectsToRemove = 0;
            
            if (Stats.ObjectsInUse == 0 && Stats.CurrentPoolSize > Pool->Config.MinPoolSize)
            {
                // Calculate how many objects to remove based on usage patterns
                const float HitRate = Stats.HitRate;
                const int32 ExcessObjects = Stats.CurrentPoolSize - Pool->Config.MinPoolSize;
                
                if (HitRate < 0.3f) // Low hit rate - remove more objects
                {
                    ObjectsToRemove = FMath::Min(ExcessObjects, ExcessObjects / 2);
                }
                else if (HitRate < 0.7f) // Medium hit rate - remove some objects
                {
                    ObjectsToRemove = FMath::Min(ExcessObjects, ExcessObjects / 4);
                }
                else // High hit rate - keep most objects
                {
                    ObjectsToRemove = FMath::Min(ExcessObjects, ExcessObjects / 8);
                }
                
                if (ObjectsToRemove > 0)
                {
                    ShouldCleanup = true;
                }
            }
            
            // Perform cleanup if conditions are met
            if (ShouldCleanup)
            {
                for (int32 i = 0; i < ObjectsToRemove; ++i)
                {
                    Pool->RemoveFromPool(nullptr);
                }
                
                CleanedPools++;
                TotalObjectsRemoved += ObjectsToRemove;
                
                if (bEnableDebugMode)
                {
                    UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::PerformAutoCleanup - Cleaned up %d objects from pool %s"), 
                           ObjectsToRemove, *PoolPair.Key->GetName());
                }
            }
        }
    }
    
    if (bEnableDebugMode && CleanedPools > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("GWIZPoolingManager::PerformAutoCleanup - Cleaned up %d objects from %d pools"), 
               TotalObjectsRemoved, CleanedPools);
    }
}

void AGWIZPoolingManager::UpdateDebugDisplay()
{
    if (!bEnableDebugMode)
    {
        return;
    }
    
    // Get current statistics
    int32 TotalPools = GetPoolCount();
    int32 TotalObjects = GetTotalObjects();
    int32 TotalObjectsInUse = GetTotalObjectsInUse();
    int64 TotalMemoryUsage = GetTotalMemoryUsage();
    
    // Calculate global hit rate
    TArray<FGWIZPoolStatistics> AllStats;
    GetGlobalPerformanceMetrics(AllStats);
    
    float GlobalHitRate = 0.0f;
    int32 TotalAccesses = 0;
    
    for (const FGWIZPoolStatistics& Stat : AllStats)
    {
        const int32 Accesses = Stat.PoolHits + Stat.PoolMisses;
        if (Accesses > 0)
        {
            GlobalHitRate += Stat.HitRate * Accesses;
            TotalAccesses += Accesses;
        }
    }
    
    if (TotalAccesses > 0)
    {
        GlobalHitRate /= TotalAccesses;
    }
    
    // Display debug information on screen (if we have access to HUD)
    if (GEngine && GEngine->GetWorld())
    {
        // Create debug text
        FString DebugText = FString::Printf(TEXT("GWIZ Pooling System Debug Info:\n"));
        DebugText += FString::Printf(TEXT("Total Pools: %d\n"), TotalPools);
        DebugText += FString::Printf(TEXT("Total Objects: %d\n"), TotalObjects);
        DebugText += FString::Printf(TEXT("Objects In Use: %d\n"), TotalObjectsInUse);
        DebugText += FString::Printf(TEXT("Memory Usage: %.2f MB\n"), TotalMemoryUsage / (1024.0f * 1024.0f));
        DebugText += FString::Printf(TEXT("Global Hit Rate: %.1f%%\n"), GlobalHitRate * 100.0f);
        
        // Display on screen (this would need to be integrated with the game's HUD system)
        // For now, we'll just log the information
        UE_LOG(LogTemp, Log, TEXT("=== GWIZ Pooling Debug Display ==="));
        UE_LOG(LogTemp, Log, TEXT("%s"), *DebugText);
        UE_LOG(LogTemp, Log, TEXT("=== End Debug Display ==="));
    }
    
    // Log detailed pool information if verbose debug is enabled
    if (bEnableDebugMode)
    {
        for (auto& PoolPair : Pools)
        {
            UGWIZObjectPool* Pool = PoolPair.Value;
            if (Pool != nullptr)
            {
                FGWIZPoolStatistics Stats = Pool->GetStatistics();
                UE_LOG(LogTemp, Log, TEXT("Pool %s: Size=%d, InUse=%d, HitRate=%.1f%%, Memory=%.2f MB"), 
                       *PoolPair.Key->GetName(), 
                       Stats.CurrentPoolSize, 
                       Stats.ObjectsInUse, 
                       Stats.HitRate * 100.0f,
                       Stats.MemoryUsage / (1024.0f * 1024.0f));
            }
        }
    }
}

