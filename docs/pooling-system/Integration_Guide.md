# GWIZ Pooling System - Integration Guide

This guide provides step-by-step instructions for integrating the GWIZ Pooling System into new Unreal Engine projects.

## Table of Contents
1. [Prerequisites](#prerequisites)
2. [Installation](#installation)
3. [Basic Setup](#basic-setup)
4. [Configuration](#configuration)
5. [Creating Poolable Objects](#creating-poolable-objects)
6. [Advanced Integration](#advanced-integration)
7. [Troubleshooting](#troubleshooting)
8. [Migration Guide](#migration-guide)

## Prerequisites

### System Requirements
- Unreal Engine 5.0 or later
- Visual Studio 2019 or later (for C++ development)
- Git (for version control)

### Project Requirements
- C++ project (Blueprint-only projects require minimal C++ setup)
- Basic understanding of Unreal Engine C++ and Blueprint systems

## Installation

### Step 1: Copy Source Files
Copy the following files to your project's `Source/YourProject/` directory:

```
PoolingSystem/
├── IGWIZPoolable.h
├── GWIZObjectPool.h
├── GWIZObjectPool.cpp
├── GWIZPoolingManager.h
├── GWIZPoolingManager.cpp
├── GWIZPoolableHelpers.h
├── GWIZPoolableHelpers.cpp
└── PoolingTypes.h
```

### Step 2: Update Build Configuration
Add the pooling system to your project's build configuration:

**In `Source/YourProject/YourProject.Build.cs`:**
```csharp
public class YourProject : ModuleRules
{
    public YourProject(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDependencyModuleNames.AddRange(new string[] { 
            "Core", 
            "CoreUObject", 
            "Engine", 
            "InputCore",
            "HAL" // Required for thread safety features
        });
        
        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
```

### Step 3: Update Module Header
**In `Source/YourProject/YourProject.h`:**
```cpp
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FYourProjectModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
```

**In `Source/YourProject/YourProject.cpp`:**
```cpp
#include "YourProject.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FYourProjectModule, YourProject, "YourProject");

void FYourProjectModule::StartupModule()
{
    // Initialize pooling system if needed
}

void FYourProjectModule::ShutdownModule()
{
    // Cleanup pooling system if needed
}
```

### Step 4: Update Game Instance
Create a custom Game Instance that inherits from `UGWIZGameInstance`:

**In `Source/YourProject/YourGameInstance.h`:**
```cpp
#pragma once

#include "CoreMinimal.h"
#include "PoolingSystem/GWIZGameInstance.h"
#include "YourGameInstance.generated.h"

UCLASS()
class YOURPROJECT_API UYourGameInstance : public UGWIZGameInstance
{
    GENERATED_BODY()

public:
    UYourGameInstance();

    // Add your custom game instance functionality here
};
```

**In `Source/YourProject/YourGameInstance.cpp`:**
```cpp
#include "YourGameInstance.h"

UYourGameInstance::UYourGameInstance()
{
    // Initialize your game instance
}
```

## Basic Setup

### Step 1: Configure Project Settings
1. Open **Project Settings** in the Unreal Editor
2. Navigate to **Project > Maps & Modes**
3. Set **Game Instance Class** to your custom Game Instance (e.g., `YourGameInstance`)

### Step 2: Create Pooling Manager Blueprint
1. In the Content Browser, right-click and select **Blueprint Class**
2. Choose **GWIZ Pooling Manager** as the parent class
3. Name it `BP_PoolingManager`
4. Place an instance of this Blueprint in your level

### Step 3: Test Basic Integration
Create a simple test to verify the integration:

**In any Actor Blueprint:**
1. Call **Get Pooling Manager** (World Context)
2. Call **Print All Pool Statistics**
3. Check the Output Log for pool information

## Configuration

### Pool Configuration
Configure pools for your specific object types:

```cpp
// In your game initialization code
AGWIZPoolingManager* PoolManager = AGWIZPoolingManager::GetPoolingManager(GetWorld());

// Configure projectile pool
FGWIZPoolConfig ProjectileConfig;
ProjectileConfig.MinPoolSize = 10;
ProjectileConfig.MaxPoolSize = 100;
ProjectileConfig.InitialPoolSize = 20;
ProjectileConfig.Category = TEXT("Projectiles");
ProjectileConfig.Priority = 1;

PoolManager->ConfigurePool(AProjectile::StaticClass(), ProjectileConfig);

// Configure enemy pool
FGWIZPoolConfig EnemyConfig;
EnemyConfig.MinPoolSize = 5;
EnemyConfig.MaxPoolSize = 50;
EnemyConfig.InitialPoolSize = 10;
EnemyConfig.Category = TEXT("Enemies");
EnemyConfig.Priority = 2;

PoolManager->ConfigurePool(AEnemy::StaticClass(), EnemyConfig);
```

### Performance Settings
Configure performance monitoring and thread safety:

```cpp
// Enable performance monitoring
PoolManager->bEnablePerformanceMonitoring = true;

// Enable thread safety if using multi-threading
PoolManager->bEnableThreadSafety = true;

// Enable auto-cleanup
PoolManager->bEnableAutoCleanup = true;

// Enable debug mode for development
PoolManager->bEnableDebugMode = true;
```

## Creating Poolable Objects

### C++ Poolable Objects
Create objects that can be pooled:

**Header file:**
```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PoolingSystem/IGWIZPoolable.h"
#include "MyPoolableActor.generated.h"

UCLASS()
class YOURPROJECT_API AMyPoolableActor : public AActor, public IGWIZPoolable
{
    GENERATED_BODY()

public:
    AMyPoolableActor();

    // IGWIZPoolable interface
    virtual void OnPooled_Implementation() override;
    virtual void OnUnpooled_Implementation() override;
    virtual bool IsPooled_Implementation() const override;

protected:
    virtual void BeginPlay() override;

private:
    bool bIsCurrentlyPooled;
    float MaxHealth;
    float CurrentHealth;
};
```

**Implementation file:**
```cpp
#include "MyPoolableActor.h"

AMyPoolableActor::AMyPoolableActor()
{
    PrimaryActorTick.bCanEverTick = true;
    bIsCurrentlyPooled = false;
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
}

void AMyPoolableActor::BeginPlay()
{
    Super::BeginPlay();
}

void AMyPoolableActor::OnPooled_Implementation()
{
    // Initialize for use
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
    SetActorTickEnabled(true);
    CurrentHealth = MaxHealth;
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

### Blueprint Poolable Objects
1. Create a Blueprint that inherits from your desired class
2. In **Class Settings**, add the **GWIZPoolable** interface
3. Implement the interface events:
   - `OnPooled`: Initialize your object for use
   - `OnUnpooled`: Clean up your object for pooling
   - `IsPooled`: Return the pooled state

## Advanced Integration

### Level-Specific Pool Configuration
Configure pools for specific levels:

```cpp
// In your Game Instance or Level Blueprint
UGWIZGameInstance* GameInstance = Cast<UGWIZGameInstance>(GetGameInstance());

// Pre-warm pools for specific level
GameInstance->PreWarmPoolsForLevel(TEXT("CombatLevel"));

// Configure level-specific pools
AGWIZPoolingManager* PoolManager = GameInstance->GetGlobalPoolingManager();

// Configure pools based on level requirements
if (CurrentLevel == TEXT("CombatLevel"))
{
    // Configure combat-specific pools
    FGWIZPoolConfig CombatConfig;
    CombatConfig.MinPoolSize = 20;
    CombatConfig.MaxPoolSize = 200;
    PoolManager->ConfigurePool(AProjectile::StaticClass(), CombatConfig);
}
```

### Save/Load Pool State
Save and restore pool configurations:

```cpp
// Save pool state
UGWIZGameInstance* GameInstance = Cast<UGWIZGameInstance>(GetGameInstance());
GameInstance->SavePoolingSystemState();

// Load pool state
GameInstance->LoadPoolingSystemState();
```

### Performance Monitoring
Monitor pool performance in real-time:

```cpp
// Get performance metrics
AGWIZPoolingManager* PoolManager = AGWIZPoolingManager::GetPoolingManager(GetWorld());
TArray<FGWIZPoolStatistics> AllStats;
PoolManager->GetGlobalPerformanceMetrics(AllStats);

// Print statistics
PoolManager->PrintAllPoolStatistics();

// Get specific metrics
int64 TotalMemory = PoolManager->GetTotalMemoryUsage();
int32 TotalObjects = PoolManager->GetTotalObjects();
int32 ObjectsInUse = PoolManager->GetTotalObjectsInUse();
```

## Troubleshooting

### Common Issues

#### Issue: Pooling Manager Not Found
**Symptoms:** `GetPoolingManager` returns null
**Solution:**
1. Ensure a `GWIZPoolingManager` instance exists in the level
2. Check that the Game Instance is properly configured
3. Verify the pooling system is initialized

#### Issue: Objects Not Being Pooled
**Symptoms:** Objects are created but not reused
**Solution:**
1. Ensure objects implement the `IGWIZPoolable` interface
2. Check that `OnPooled` and `OnUnpooled` are properly implemented
3. Verify objects are being returned to the pool

#### Issue: Performance Problems
**Symptoms:** Poor performance with pooling enabled
**Solution:**
1. Check pool sizes are appropriate for your use case
2. Enable performance monitoring to identify bottlenecks
3. Consider enabling auto-cleanup for unused pools

#### Issue: Thread Safety Problems
**Symptoms:** Crashes or undefined behavior in multi-threaded scenarios
**Solution:**
1. Enable thread safety mode
2. Ensure proper synchronization in your code
3. Test thoroughly in multi-threaded scenarios

### Debug Tips
1. **Enable Debug Mode:** Set `bEnableDebugMode = true` for detailed logging
2. **Use Print Statistics:** Call `PrintAllPoolStatistics()` for overview
3. **Validate Pools:** Use `ValidatePool()` to check pool integrity
4. **Monitor Performance:** Use performance metrics to identify issues

## Migration Guide

### From Manual Object Creation
If you're migrating from manual object creation:

1. **Replace Object Creation:**
   ```cpp
   // Old code
   AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>();
   
   // New code
   AGWIZPoolingManager* PoolManager = AGWIZPoolingManager::GetPoolingManager(GetWorld());
   AProjectile* Projectile = Cast<AProjectile>(PoolManager->GetPooledObject(AProjectile::StaticClass()));
   ```

2. **Replace Object Destruction:**
   ```cpp
   // Old code
   Projectile->Destroy();
   
   // New code
   PoolManager->ReturnPooledObject(Projectile);
   ```

3. **Update Object Classes:**
   - Add `IGWIZPoolable` interface to your classes
   - Implement `OnPooled` and `OnUnpooled` methods
   - Update object lifecycle management

### From Other Pooling Systems
If migrating from another pooling system:

1. **Replace Pool Manager Access:**
   - Update all pool manager references to use `AGWIZPoolingManager`
   - Replace pool configuration with `FGWIZPoolConfig`

2. **Update Object Interfaces:**
   - Replace existing pooling interfaces with `IGWIZPoolable`
   - Update method names to match the new interface

3. **Migrate Configuration:**
   - Convert existing pool configurations to `FGWIZPoolConfig`
   - Update pool initialization code

### Testing Migration
After migration:

1. **Test Basic Functionality:**
   - Verify objects are being pooled correctly
   - Check that objects are being reused
   - Ensure proper cleanup

2. **Test Performance:**
   - Compare performance before and after migration
   - Monitor memory usage
   - Check for any performance regressions

3. **Test Edge Cases:**
   - Test with high object counts
   - Test rapid creation/destruction
   - Test level transitions

## Best Practices

### Performance
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

## Conclusion

The GWIZ Pooling System provides a robust and flexible solution for object pooling in Unreal Engine projects. By following this integration guide, you can successfully integrate the system into your project and take advantage of its performance benefits.

For additional support and examples, refer to the API documentation and usage examples provided with the system.
