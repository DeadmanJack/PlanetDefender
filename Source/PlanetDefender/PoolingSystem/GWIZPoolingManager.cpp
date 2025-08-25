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
            // TODO: UpdatePerformanceMetrics();
        }
        
        // TODO: if (bEnableAutoCleanup) CleanupUnusedPools();
    }
    
    // Debug display updates every frame (if enabled)
    if (bEnableDebugMode)
    {
        // TODO: UpdateDebugDisplay();
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
    // TODO: Implement all pool statistics printing
}

void AGWIZPoolingManager::GetGlobalPerformanceMetrics(TArray<FGWIZPoolStatistics>& AllStats)
{
    // TODO: Implement global performance metrics collection
}

TArray<UGWIZObjectPool*> AGWIZPoolingManager::GetAllPools() const
{
    // TODO: Implement get all pools
    return TArray<UGWIZObjectPool*>();
}

int32 AGWIZPoolingManager::GetPoolCount() const
{
    return Pools.Num();
}

void AGWIZPoolingManager::ClearAllPools()
{
    // TODO: Implement clear all pools
}

UGWIZObjectPool* AGWIZPoolingManager::GetPoolForClass(TSubclassOf<UObject> ObjectClass) const
{
    // TODO: Implement get pool for specific class
    return nullptr;
}

TArray<UGWIZObjectPool*> AGWIZPoolingManager::GetPoolsByCategory(const FString& Category) const
{
    // TODO: Implement get pools by category
    return TArray<UGWIZObjectPool*>();
}

TArray<UGWIZObjectPool*> AGWIZPoolingManager::GetPoolsByPriority(int32 Priority) const
{
    // TODO: Implement get pools by priority
    return TArray<UGWIZObjectPool*>();
}

void AGWIZPoolingManager::CleanupUnusedPools()
{
    // TODO: Implement cleanup unused pools
}

int64 AGWIZPoolingManager::GetTotalMemoryUsage() const
{
    // TODO: Implement total memory usage calculation
    return 0;
}

int32 AGWIZPoolingManager::GetTotalObjects() const
{
    // TODO: Implement total objects calculation
    return 0;
}

int32 AGWIZPoolingManager::GetTotalObjectsInUse() const
{
    // TODO: Implement total objects in use calculation
    return 0;
}

