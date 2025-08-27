# GWIZ Pooling System API Reference

This document describes the public APIs exposed by the GWIZ Pooling System for both C++ and Blueprint usage.

### Modules and Key Types
- `AGWIZPoolingManager` (Actor): Global manager providing access to pools and system-wide utilities.
- `UGWIZObjectPool` (UObject): Pool for a specific `UObject` subclass.
- `IGWIZPoolable` (Interface): Optional interface for pooled objects to receive lifecycle callbacks.
- `UGWIZGameInstance` (GameInstance): Convenience integration point exposing the global pooling manager.
- `FGWIZPoolConfig`, `FGWIZPoolStatistics` (Structs in `PoolingTypes.h`): Configuration and stats for pools.

### FGWIZPoolConfig
Configuration structure for pool settings.

- Properties
  - `int32 MinPoolSize` - Minimum number of objects to keep in pool (default: 5)
  - `int32 MaxPoolSize` - Maximum number of objects in pool (default: 100)
  - `int32 InitialPoolSize` - Initial pool size for pre-warming (default: 10)
  - `bool bEnableDebug` - Whether to enable debug logging for this pool
  - `bool bEnableMonitoring` - Whether to enable performance monitoring
  - `bool bEnableThreadSafety` - Whether to enable thread safety features
  - `FString Category` - Category for organizing pools (default: "Default")
  - `int32 Priority` - Priority level for pool management (default: 5, range: 0-10)

- Methods
  - `bool IsValid() const` - Validate configuration values

### FGWIZPoolStatistics
Statistics structure for pool performance monitoring.

- Properties
  - `int32 CurrentPoolSize` - Current number of objects in pool
  - `int32 ObjectsInUse` - Number of objects currently in use
  - `int32 TotalObjectsCreated` - Number of objects created total
  - `int32 PoolHits` - Number of times pool was accessed successfully
  - `int32 PoolMisses` - Number of times new object was created
  - `float HitRate` - Hit rate percentage (0.0 to 1.0)
  - `int64 MemoryUsage` - Memory usage in bytes
  - `float AverageLifetime` - Average object lifetime in seconds
  - `int32 PeakConcurrentUsage` - Peak concurrent usage
  - `float LastUpdateTime` - Last time statistics were updated

- Methods
  - `void CalculateHitRate()` - Calculate hit rate based on hits and misses
  - `int32 GetTotalObjects() const` - Get total objects (pooled + in use)
  - `void Reset()` - Reset all statistics to zero

### IGWIZPoolable
Optional interface for objects managed by pools.

- `void OnPooled()`
  - Called when an object is retrieved from a pool.
- `void OnUnpooled()`
  - Called when an object is returned to a pool.
- `bool IsPooled() const`
  - Returns true if object is currently in the pool.

Blueprint: Implement these as events on your Blueprint that implements `GWIZPoolable`.

### UGWIZObjectPool
Pool for a specific object class.

- Properties
  - `FGWIZPoolConfig Config`
  - `FGWIZPoolStatistics Statistics` (read-only)

- Methods
  - `UObject* GetObject(TSubclassOf<UObject> ObjectClass)`
  - `void ReturnObject(UObject* Object)`
  - `void AddToPool(UObject* Object)`
  - `void RemoveFromPool(UObject* Object)`
  - `void ClearPool()`
  - `void PreWarmPool(int32 Count)`
  - `FGWIZPoolStatistics GetStatistics() const`
  - `void PrintDebugInfo()`
  - `int64 GetMemoryUsage() const`
  - `bool ValidatePool() const`
  - `int32 GetPoolSize() const`
  - `int32 GetObjectsInUse() const`
  - `bool IsPoolEmpty() const`
  - `bool IsPoolFull() const`

Blueprint: All methods are exposed under category "GWIZ Pooling".

### AGWIZPoolingManager
Global pooling manager. Obtain via `GetPoolingManager(WorldContext)`.

- Access
  - `static AGWIZPoolingManager* GetPoolingManager(const UObject* WorldContext)`

- Pool Operations
  - `UGWIZObjectPool* GetPool(TSubclassOf<UObject> ObjectClass)`
  - `void ConfigurePool(TSubclassOf<UObject> ObjectClass, const FGWIZPoolConfig& Config)`
  - `UObject* GetPooledObject(TSubclassOf<UObject> ObjectClass)`
  - `void ReturnPooledObject(UObject* Object)`

- Lifecycle and Utilities
  - `void PreWarmAllPools()`
  - `void PrintAllPoolStatistics()`
  - `void GetGlobalPerformanceMetrics(TArray<FGWIZPoolStatistics>& AllStats)`
  - `TArray<UGWIZObjectPool*> GetAllPools() const`
  - `int32 GetPoolCount() const`
  - `void ClearAllPools()`
  - `UGWIZObjectPool* GetPoolForClass(TSubclassOf<UObject> ObjectClass) const`
  - `TArray<UGWIZObjectPool*> GetPoolsByCategory(const FString& Category) const`
  - `TArray<UGWIZObjectPool*> GetPoolsByPriority(int32 Priority) const`
  - `void CleanupUnusedPools()`
  - `int64 GetTotalMemoryUsage() const`
  - `int32 GetTotalObjects() const`
  - `int32 GetTotalObjectsInUse() const`

- Settings
  - `bool bEnableDebugMode`
  - `bool bEnablePerformanceMonitoring`
  - `bool bEnableThreadSafety`
  - `bool bEnableAutoCleanup`

Blueprint: All callable functions are exposed under category "Pooling".

### UGWIZPoolableHelpers
Helper functions for implementing default poolable behavior.

- Static Methods
  - `static void DefaultOnPooled(AActor* Actor)` - Default OnPooled implementation for actors
  - `static void DefaultOnUnpooled(AActor* Actor)` - Default OnUnpooled implementation for actors
  - `static bool DefaultIsPooled()` - Default IsPooled implementation (returns false)
  - `template<typename T> static bool ImplementsInterface(const UObject* Object)` - Safe interface check

Blueprint: Methods exposed under category "GWIZ Pooling".

### UGWIZGameInstance
Integration with the pooling system for global persistence.

- Methods
  - `AGWIZPoolingManager* GetGlobalPoolingManager() const`
  - `bool IsPoolingSystemInitialized() const`
  - `void SavePoolingSystemState()`
  - `void LoadPoolingSystemState()`
  - `void PreWarmPoolsForLevel(const FString& LevelName)`
  - `void CleanupUnusedPoolsForLevel(const FString& LevelName)`

Blueprint: Methods exposed under category "GWIZ Pooling".

### Minimal C++ Example
```cpp
// Acquire global manager and get an object
AGWIZPoolingManager* Manager = AGWIZPoolingManager::GetPoolingManager(GetWorld());
UObject* Obj = Manager->GetPooledObject(MyClass);

// Use the object ... then return it
Manager->ReturnPooledObject(Obj);
```

### Minimal Blueprint Example
1. Call "Get Pooling Manager" (WorldContext).
2. Call "Get Pooled Object" with your class.
3. When finished, call "Return Pooled Object" passing the object.


