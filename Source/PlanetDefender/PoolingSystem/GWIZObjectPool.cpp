// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.

#include "GWIZObjectPool.h"
#include "Engine/Engine.h"
#include "HAL/PlatformTime.h"
#include "Misc/DateTime.h"

UGWIZObjectPool::UGWIZObjectPool()
    : bIsInitialized(false)
{
    // Initialize with default configuration
    Config = FGWIZPoolConfig();
    Statistics = FGWIZPoolStatistics();
}

UObject* UGWIZObjectPool::GetObject(TSubclassOf<UObject> ObjectClass)
{
    if (!ObjectClass)
    {
        LogDebug(TEXT("GetObject: Invalid object class"));
        return nullptr;
    }

    // Set object class if not already set
    if (!PooledObjectClass)
    {
        PooledObjectClass = ObjectClass;
    }

    UObject* Object = nullptr;

    // Try to get object from pool first
    if (AvailableObjects.Num() > 0)
    {
        Object = AvailableObjects.Pop();
        Statistics.PoolHits++;
        LogDebug(FString::Printf(TEXT("GetObject: Retrieved from pool, remaining: %d"), AvailableObjects.Num()));
    }
    else
    {
        // Pool is empty, create new object
        Object = CreateNewObject();
        Statistics.PoolMisses++;
        LogDebug(TEXT("GetObject: Created new object"));
    }

    if (Object)
    {
        // Move object to in-use list
        ObjectsInUse.Add(Object);
        
        // Initialize object for use
        InitializeObject(Object);
        
        // Update statistics
        UpdateStatistics();
    }

    return Object;
}

void UGWIZObjectPool::ReturnObject(UObject* Object)
{
    if (!Object)
    {
        LogDebug(TEXT("ReturnObject: Invalid object"));
        return;
    }

    // Remove from in-use list
    ObjectsInUse.Remove(Object);

    // Clean up object for pooling
    CleanupObject(Object);

    // Check if we should add to pool or destroy
    if (AvailableObjects.Num() < Config.MaxPoolSize)
    {
        AvailableObjects.Add(Object);
        LogDebug(FString::Printf(TEXT("ReturnObject: Added to pool, size: %d"), AvailableObjects.Num()));
    }
    else
    {
        // Pool is full, destroy object
        LogDebug(TEXT("ReturnObject: Pool full, destroying object"));
        // Note: In Unreal, we typically don't manually destroy UObjects
        // They're garbage collected when no references remain
    }

    // Update statistics
    UpdateStatistics();
}

void UGWIZObjectPool::AddToPool(UObject* Object)
{
    if (!ValidateObject(Object))
    {
        LogDebug(TEXT("AddToPool: Invalid object"));
        return;
    }

    if (AvailableObjects.Num() >= Config.MaxPoolSize)
    {
        LogDebug(TEXT("AddToPool: Pool is full"));
        return;
    }

    AvailableObjects.Add(Object);
    LogDebug(FString::Printf(TEXT("AddToPool: Added to pool, size: %d"), AvailableObjects.Num()));
    
    UpdateStatistics();
}

void UGWIZObjectPool::RemoveFromPool(UObject* Object)
{
    if (!Object)
    {
        return;
    }

    AvailableObjects.Remove(Object);
    LogDebug(FString::Printf(TEXT("RemoveFromPool: Removed from pool, size: %d"), AvailableObjects.Num()));
    
    UpdateStatistics();
}

void UGWIZObjectPool::ClearPool()
{
    LogDebug(FString::Printf(TEXT("ClearPool: Clearing %d available objects"), AvailableObjects.Num()));
    
    AvailableObjects.Empty();
    UpdateStatistics();
}

void UGWIZObjectPool::PreWarmPool(int32 Count)
{
    if (!PooledObjectClass)
    {
        LogDebug(TEXT("PreWarmPool: No object class set"));
        return;
    }

    const int32 TargetCount = FMath::Min(Count, Config.MaxPoolSize);
    const int32 CurrentCount = AvailableObjects.Num();
    const int32 ToCreate = FMath::Max(0, TargetCount - CurrentCount);

    LogDebug(FString::Printf(TEXT("PreWarmPool: Creating %d objects"), ToCreate));

    for (int32 i = 0; i < ToCreate; ++i)
    {
        UObject* Object = CreateNewObject();
        if (Object)
        {
            AvailableObjects.Add(Object);
        }
    }

    UpdateStatistics();
}

FGWIZPoolStatistics UGWIZObjectPool::GetStatistics() const
{
    return Statistics;
}

void UGWIZObjectPool::PrintDebugInfo()
{
    if (!Config.bEnableDebug)
    {
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("=== GWIZ Object Pool Debug Info ==="));
    UE_LOG(LogTemp, Log, TEXT("Pool Size: %d"), AvailableObjects.Num());
    UE_LOG(LogTemp, Log, TEXT("Objects In Use: %d"), ObjectsInUse.Num());
    UE_LOG(LogTemp, Log, TEXT("Total Created: %d"), Statistics.TotalObjectsCreated);
    UE_LOG(LogTemp, Log, TEXT("Pool Hits: %d"), Statistics.PoolHits);
    UE_LOG(LogTemp, Log, TEXT("Pool Misses: %d"), Statistics.PoolMisses);
    UE_LOG(LogTemp, Log, TEXT("Hit Rate: %.2f%%"), Statistics.HitRate * 100.0f);
    UE_LOG(LogTemp, Log, TEXT("Memory Usage: %lld bytes"), Statistics.MemoryUsage);
    UE_LOG(LogTemp, Log, TEXT("================================"));
}

int64 UGWIZObjectPool::GetMemoryUsage() const
{
    return Statistics.MemoryUsage;
}

bool UGWIZObjectPool::ValidatePool() const
{
    // Check if pool size matches statistics
    if (AvailableObjects.Num() != Statistics.CurrentPoolSize)
    {
        return false;
    }

    // Check if objects in use matches statistics
    if (ObjectsInUse.Num() != Statistics.ObjectsInUse)
    {
        return false;
    }

    // Check if total objects matches statistics
    if (AvailableObjects.Num() + ObjectsInUse.Num() != Statistics.GetTotalObjects())
    {
        return false;
    }

    return true;
}

int32 UGWIZObjectPool::GetPoolSize() const
{
    return AvailableObjects.Num();
}

int32 UGWIZObjectPool::GetObjectsInUse() const
{
    return ObjectsInUse.Num();
}

bool UGWIZObjectPool::IsPoolEmpty() const
{
    return AvailableObjects.Num() == 0;
}

bool UGWIZObjectPool::IsPoolFull() const
{
    return AvailableObjects.Num() >= Config.MaxPoolSize;
}

UObject* UGWIZObjectPool::CreateNewObject()
{
    if (!PooledObjectClass)
    {
        return nullptr;
    }

    UObject* Object = NewObject<UObject>(this, PooledObjectClass);
    if (Object)
    {
        Statistics.TotalObjectsCreated++;
        LogDebug(TEXT("CreateNewObject: Created new object"));
    }

    return Object;
}

void UGWIZObjectPool::InitializeObject(UObject* Object)
{
    if (!Object)
    {
        return;
    }

    // Check if object implements the poolable interface
    if (Object->GetClass()->ImplementsInterface(UGWIZPoolable::StaticClass()))
    {
        IGWIZPoolable::Execute_OnPooled(Object);
    }

    // For actors, we can do additional initialization
    if (AActor* Actor = Cast<AActor>(Object))
    {
        // Enable ticking and rendering
        Actor->SetActorTickEnabled(true);
        Actor->SetActorHiddenInGame(false);
        Actor->SetActorEnableCollision(true);
    }
}

void UGWIZObjectPool::CleanupObject(UObject* Object)
{
    if (!Object)
    {
        return;
    }

    // Check if object implements the poolable interface
    if (Object->GetClass()->ImplementsInterface(UGWIZPoolable::StaticClass()))
    {
        IGWIZPoolable::Execute_OnUnpooled(Object);
    }

    // For actors, we can do additional cleanup
    if (AActor* Actor = Cast<AActor>(Object))
    {
        // Disable ticking and rendering
        Actor->SetActorTickEnabled(false);
        Actor->SetActorHiddenInGame(true);
        Actor->SetActorEnableCollision(false);
    }
}

void UGWIZObjectPool::UpdateStatistics()
{
    Statistics.CurrentPoolSize = AvailableObjects.Num();
    Statistics.ObjectsInUse = ObjectsInUse.Num();
    Statistics.LastUpdateTime = FPlatformTime::Seconds();
    
    // Update peak concurrent usage
    const int32 TotalObjects = Statistics.GetTotalObjects();
    if (TotalObjects > Statistics.PeakConcurrentUsage)
    {
        Statistics.PeakConcurrentUsage = TotalObjects;
    }

    // Calculate hit rate
    Statistics.CalculateHitRate();

    // Calculate memory usage if monitoring is enabled
    if (Config.bEnableMonitoring)
    {
        CalculateMemoryUsage();
    }
}

void UGWIZObjectPool::CalculateMemoryUsage()
{
    int64 TotalMemory = 0;

    // Calculate memory for available objects
    for (UObject* Object : AvailableObjects)
    {
        if (Object)
        {
            TotalMemory += Object->GetClass()->GetPropertiesSize();
        }
    }

    // Calculate memory for objects in use
    for (UObject* Object : ObjectsInUse)
    {
        if (Object)
        {
            TotalMemory += Object->GetClass()->GetPropertiesSize();
        }
    }

    Statistics.MemoryUsage = TotalMemory;
}

void UGWIZObjectPool::LogDebug(const FString& Message)
{
    if (Config.bEnableDebug)
    {
        UE_LOG(LogTemp, Log, TEXT("[GWIZ Pool] %s"), *Message);
    }
}

bool UGWIZObjectPool::ValidateObject(UObject* Object) const
{
    if (!Object)
    {
        return false;
    }

    // Check if object is of the correct class
    if (PooledObjectClass && !Object->IsA(PooledObjectClass))
    {
        return false;
    }

    return true;
}
