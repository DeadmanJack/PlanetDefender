# GWIZ Pooling System - Usage Examples

This document provides practical examples of how to use the GWIZ Pooling System in various scenarios.

## Table of Contents
1. [Basic Usage](#basic-usage)
2. [Blueprint Integration](#blueprint-integration)
3. [Advanced Configuration](#advanced-configuration)
4. [Performance Monitoring](#performance-monitoring)
5. [Thread Safety](#thread-safety)
6. [Game Instance Integration](#game-instance-integration)
7. [Custom Poolable Objects](#custom-poolable-objects)

## Basic Usage

### C++ Basic Example
```cpp
// In your Actor or Component class
void AMyActor::SpawnProjectile()
{
    // Get the global pooling manager
    AGWIZPoolingManager* PoolManager = AGWIZPoolingManager::GetPoolingManager(GetWorld());
    
    // Get a projectile from the pool
    AProjectile* Projectile = Cast<AProjectile>(PoolManager->GetPooledObject(AProjectile::StaticClass()));
    
    if (Projectile)
    {
        // Configure the projectile
        Projectile->SetActorLocation(GetActorLocation());
        Projectile->SetActorRotation(GetActorRotation());
        
        // Store reference for later return
        ActiveProjectiles.Add(Projectile);
    }
}

void AMyActor::OnProjectileHit(AProjectile* Projectile)
{
    // Return the projectile to the pool
    AGWIZPoolingManager* PoolManager = AGWIZPoolingManager::GetPoolingManager(GetWorld());
    PoolManager->ReturnPooledObject(Projectile);
    
    // Remove from active list
    ActiveProjectiles.Remove(Projectile);
}
```

### Blueprint Basic Example
1. **Getting a Pooled Object:**
   - Call "Get Pooling Manager" (World Context)
   - Call "Get Pooled Object" with your class
   - Store the returned object reference

2. **Returning an Object:**
   - Call "Get Pooling Manager" (World Context)
   - Call "Return Pooled Object" with your object reference

## Blueprint Integration

### Setting Up Poolable Blueprint
1. Create a Blueprint that inherits from your desired class
2. In Class Settings, add the "GWIZPoolable" interface
3. Implement the interface events:
   - `OnPooled`: Initialize/reset your object
   - `OnUnpooled`: Clean up your object
   - `IsPooled`: Return whether object is currently pooled

### Example Blueprint Implementation
```
OnPooled Event:
- Set Visibility to true
- Reset health to max
- Clear any active effects
- Reset position and rotation

OnUnpooled Event:
- Set Visibility to false
- Stop any active timers
- Clear any active effects
- Reset all variables to defaults

IsPooled Function:
- Return true if object is not currently in use
```

## Advanced Configuration

### Configuring Pool Settings
```cpp
// Configure a pool with specific settings
AGWIZPoolingManager* PoolManager = AGWIZPoolingManager::GetPoolingManager(GetWorld());

FGWIZPoolConfig Config;
Config.MinPoolSize = 5;
Config.MaxPoolSize = 50;
Config.InitialPoolSize = 10;
Config.Category = TEXT("Projectiles");
Config.Priority = 1;

PoolManager->ConfigurePool(AProjectile::StaticClass(), Config);
```

### Pre-warming Pools
```cpp
// Pre-warm all pools for better performance
AGWIZPoolingManager* PoolManager = AGWIZPoolingManager::GetPoolingManager(GetWorld());
PoolManager->PreWarmAllPools();
```

### Blueprint Configuration
1. Get the pooling manager
2. Create a "Make GWIZ Pool Config" struct
3. Set the desired values
4. Call "Configure Pool" with your class and config

## Performance Monitoring

### C++ Performance Monitoring
```cpp
// Enable performance monitoring
AGWIZPoolingManager* PoolManager = AGWIZPoolingManager::GetPoolingManager(GetWorld());
PoolManager->bEnablePerformanceMonitoring = true;

// Get performance metrics
TArray<FGWIZPoolStatistics> AllStats;
PoolManager->GetGlobalPerformanceMetrics(AllStats);

// Print statistics to log
PoolManager->PrintAllPoolStatistics();

// Get specific metrics
int64 TotalMemory = PoolManager->GetTotalMemoryUsage();
int32 TotalObjects = PoolManager->GetTotalObjects();
int32 ObjectsInUse = PoolManager->GetTotalObjectsInUse();
```

### Blueprint Performance Monitoring
1. Enable performance monitoring on the pooling manager
2. Use "Get Global Performance Metrics" to get statistics
3. Use "Print All Pool Statistics" for debug output
4. Access individual metrics like "Get Total Memory Usage"

## Thread Safety

### Enabling Thread Safety
```cpp
// Enable thread safety for multi-threaded access
AGWIZPoolingManager* PoolManager = AGWIZPoolingManager::GetPoolingManager(GetWorld());
PoolManager->bEnableThreadSafety = true;
```

### Thread-Safe Usage
```cpp
// In a worker thread
void WorkerThread::SpawnObject()
{
    // Thread-safe pool access
    AGWIZPoolingManager* PoolManager = AGWIZPoolingManager::GetPoolingManager(WorldContext);
    UObject* Obj = PoolManager->GetPooledObject(MyClass);
    
    // Use object...
    
    // Return to pool
    PoolManager->ReturnPooledObject(Obj);
}
```

## Game Instance Integration

### Setting Up Game Instance
1. Create a Game Instance Blueprint that inherits from `UGWIZGameInstance`
2. Set this as your project's Game Instance in Project Settings

### Using Game Instance Integration
```cpp
// Get global pooling manager through game instance
UGWIZGameInstance* GameInstance = Cast<UGWIZGameInstance>(GetGameInstance());
AGWIZPoolingManager* PoolManager = GameInstance->GetGlobalPoolingManager();

// Pre-warm pools for specific level
GameInstance->PreWarmPoolsForLevel(TEXT("Level1"));

// Save/load pool state
GameInstance->SavePoolingSystemState();
GameInstance->LoadPoolingSystemState();
```

### Blueprint Game Instance Usage
1. Cast to your Game Instance
2. Use "Get Global Pooling Manager"
3. Use level-specific methods like "Pre Warm Pools For Level"

## Custom Poolable Objects

### C++ Poolable Implementation
```cpp
// In your custom class header
UCLASS()
class AMyPoolableActor : public AActor, public IGWIZPoolable
{
    GENERATED_BODY()

public:
    // IGWIZPoolable interface
    virtual void OnPooled_Implementation() override;
    virtual void OnUnpooled_Implementation() override;
    virtual bool IsPooled_Implementation() const override;

private:
    bool bIsCurrentlyPooled;
};

// In your custom class implementation
void AMyPoolableActor::OnPooled_Implementation()
{
    // Initialize for use
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
    SetActorTickEnabled(true);
    bIsCurrentlyPooled = false;
}

void AMyPoolableActor::OnUnpooled_Implementation()
{
    // Clean up for pooling
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    SetActorTickEnabled(false);
    bIsCurrentlyPooled = true;
}

bool AMyPoolableActor::IsPooled_Implementation() const
{
    return bIsCurrentlyPooled;
}
```

### Blueprint Poolable Implementation
1. Add the "GWIZPoolable" interface to your Blueprint
2. Implement the interface events:
   - `OnPooled`: Set up your object for use
   - `OnUnpooled`: Clean up your object for pooling
   - `IsPooled`: Return the pooled state

## Best Practices

### Performance Tips
1. **Pre-warm pools** before heavy usage
2. **Configure appropriate pool sizes** based on expected usage
3. **Use categories** to organize related pools
4. **Monitor performance** regularly
5. **Clean up unused pools** periodically

### Memory Management
1. **Return objects promptly** when no longer needed
2. **Use appropriate max pool sizes** to prevent memory bloat
3. **Enable auto-cleanup** for automatic maintenance
4. **Monitor memory usage** with performance metrics

### Thread Safety
1. **Enable thread safety** for multi-threaded access
2. **Minimize pool operations** in performance-critical threads
3. **Use appropriate synchronization** when needed
4. **Test thoroughly** in multi-threaded scenarios

### Debugging
1. **Enable debug mode** for detailed logging
2. **Use Print All Pool Statistics** for overview
3. **Validate pools** periodically
4. **Monitor hit rates** and performance metrics

## Common Patterns

### Object Lifecycle Management
```cpp
// Pattern: Get object, use it, return it
UObject* Obj = PoolManager->GetPooledObject(MyClass);
if (Obj)
{
    // Use the object
    UseObject(Obj);
    
    // Always return it when done
    PoolManager->ReturnPooledObject(Obj);
}
```

### Batch Operations
```cpp
// Pattern: Get multiple objects, use them, return them
TArray<UObject*> Objects;
for (int32 i = 0; i < Count; ++i)
{
    UObject* Obj = PoolManager->GetPooledObject(MyClass);
    if (Obj)
    {
        Objects.Add(Obj);
    }
}

// Use all objects
for (UObject* Obj : Objects)
{
    UseObject(Obj);
}

// Return all objects
for (UObject* Obj : Objects)
{
    PoolManager->ReturnPooledObject(Obj);
}
```

### Conditional Pooling
```cpp
// Pattern: Only pool if manager is available
AGWIZPoolingManager* PoolManager = AGWIZPoolingManager::GetPoolingManager(GetWorld());
if (PoolManager)
{
    UObject* Obj = PoolManager->GetPooledObject(MyClass);
    // Use pooled object
}
else
{
    // Fallback to regular creation
    UObject* Obj = NewObject<UMyClass>();
    // Use regular object
}
```
