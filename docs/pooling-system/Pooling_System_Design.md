# Generic Pooling System - Technical Design

## Architecture Overview

### Core Components

#### 1. IGWIZPoolable Interface
```cpp
UINTERFACE(BlueprintType)
class PLANETDEFENDER_API UGWIZPoolable : public UInterface
{
    GENERATED_BODY()
};

class PLANETDEFENDER_API IGWIZPoolable
{
    GENERATED_BODY()

public:
    // Called when object is taken from pool
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pooling")
    void OnPooled();
    
    // Called when object is returned to pool
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pooling")
    void OnUnpooled();
    
    // Check if object is currently pooled
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pooling")
    bool IsPooled() const;
};
```

#### 2. FGWIZPoolConfig Structure
```cpp
USTRUCT(BlueprintType)
struct PLANETDEFENDER_API FGWIZPoolConfig
{
    GENERATED_BODY()
    
    // Minimum number of objects to keep in pool
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
    int32 MinPoolSize = 5;
    
    // Maximum number of objects in pool
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
    int32 MaxPoolSize = 100;
    
    // Initial pool size for pre-warming
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
    int32 InitialPoolSize = 10;
    
    // Whether to enable debug logging for this pool
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
    bool bEnableDebug = false;
    
    // Whether to enable performance monitoring
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
    bool bEnableMonitoring = true;
    
    // Whether to enable thread safety features
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
    bool bEnableThreadSafety = false;
};
```

#### 3. FGWIZPoolStatistics Structure
```cpp
USTRUCT(BlueprintType)
struct PLANETDEFENDER_API FGWIZPoolStatistics
{
    GENERATED_BODY()
    
    // Current number of objects in pool
    UPROPERTY(BlueprintReadOnly, Category = "Pooling")
    int32 CurrentPoolSize = 0;
    
    // Number of objects currently in use
    UPROPERTY(BlueprintReadOnly, Category = "Pooling")
    int32 ObjectsInUse = 0;
    
    // Number of objects created total
    UPROPERTY(BlueprintReadOnly, Category = "Pooling")
    int32 TotalObjectsCreated = 0;
    
    // Number of times pool was accessed
    UPROPERTY(BlueprintReadOnly, Category = "Pooling")
    int32 PoolHits = 0;
    
    // Number of times new object was created
    UPROPERTY(BlueprintReadOnly, Category = "Pooling")
    int32 PoolMisses = 0;
    
    // Hit rate percentage
    UPROPERTY(BlueprintReadOnly, Category = "Pooling")
    float HitRate = 0.0f;
    
    // Memory usage in bytes
    UPROPERTY(BlueprintReadOnly, Category = "Pooling")
    int64 MemoryUsage = 0;
    
    // Average object lifetime
    UPROPERTY(BlueprintReadOnly, Category = "Pooling")
    float AverageLifetime = 0.0f;
    
    // Peak concurrent usage
    UPROPERTY(BlueprintReadOnly, Category = "Pooling")
    int32 PeakConcurrentUsage = 0;
};
```

#### 4. UGWIZObjectPool Class
```cpp
UCLASS(BlueprintType, Blueprintable)
class PLANETDEFENDER_API UGWIZObjectPool : public UObject
{
    GENERATED_BODY()
    
public:
    // Pool configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
    FGWIZPoolConfig Config;
    
    // Pool statistics
    UPROPERTY(BlueprintReadOnly, Category = "Pooling")
    FGWIZPoolStatistics Statistics;
    
    // Get object from pool
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    UObject* GetObject(TSubclassOf<UObject> ObjectClass);
    
    // Return object to pool
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    void ReturnObject(UObject* Object);
    
    // Add object to pool
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    void AddToPool(UObject* Object);
    
    // Remove object from pool
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    void RemoveFromPool(UObject* Object);
    
    // Clear entire pool
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    void ClearPool();
    
    // Pre-warm pool with specified number of objects
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    void PreWarmPool(int32 Count);
    
    // Get pool statistics
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    FGWIZPoolStatistics GetStatistics() const;
    
    // Print debug information
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    void PrintDebugInfo();
    
    // Get memory usage information
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    int64 GetMemoryUsage() const;
    
    // Validate pool integrity
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    bool ValidatePool() const;

    // Get pool size
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    int32 GetPoolSize() const;

    // Get number of objects in use
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    int32 GetObjectsInUse() const;

    // Get total objects (pooled + in use)
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    int32 GetTotalObjects() const { return GetPoolSize() + GetObjectsInUse(); }

    // Get pool configuration
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    FGWIZPoolConfig GetConfig() const;
};
```

#### 5. AGWIZPoolingManager Class
```cpp
UCLASS(BlueprintType, Blueprintable)
class PLANETDEFENDER_API AGWIZPoolingManager : public AActor
{
    GENERATED_BODY()
    
public:
    AGWIZPoolingManager();
    
    // Global pool manager instance
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pooling")
    static AGWIZPoolingManager* GetPoolingManager(const UObject* WorldContext);
    
    // Get or create pool for object class
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    UGWIZObjectPool* GetPool(TSubclassOf<UObject> ObjectClass);
    
    // Configure pool for object class
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    void ConfigurePool(TSubclassOf<UObject> ObjectClass, const FGWIZPoolConfig& Config);
    
    // Get object from global pool
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    UObject* GetPooledObject(TSubclassOf<UObject> ObjectClass);
    
    // Return object to global pool
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    void ReturnPooledObject(UObject* Object);
    
    // Pre-warm all pools
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    void PreWarmAllPools();
    
    // Print all pool statistics
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    void PrintAllPoolStatistics();
    
    // Get global performance metrics
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    void GetGlobalPerformanceMetrics(TArray<FGWIZPoolStatistics>& AllStats);
    
    // Enable/disable debug mode
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
    bool bEnableDebugMode = false;
    
    // Enable/disable monitoring mode
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
    bool bEnableMonitoring = true;
    
    // Enable/disable thread safety
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
    bool bEnableThreadSafety = false;
    
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
private:
    // Map of object classes to their pools
    UPROPERTY()
    TMap<TSubclassOf<UObject>, UGWIZObjectPool*> Pools;
    
    // Default pool configuration
    UPROPERTY(EditAnywhere, Category = "Pooling")
    FGWIZPoolConfig DefaultConfig;
    
    // Thread safety mutex
    FCriticalSection PoolMutex;
    
    // Performance monitoring data
    TArray<FGWIZPoolStatistics> HistoricalStats;
};
```

## Data Flow

### 1. Object Creation Flow
1. Request object from pool
2. Check if pool exists for object class
3. If pool exists and has available objects:
   - Remove object from pool
   - Call `OnPooled()` on object
   - Update statistics
   - Return object
4. If no pool or no available objects:
   - Create new object
   - Call `OnPooled()` on object
   - Update statistics
   - Return object

### 2. Object Return Flow
1. Call `OnUnpooled()` on object
2. Disable object (ticking, rendering, collision)
3. Add object to appropriate pool
4. Update pool statistics
5. Validate pool integrity

### 3. Pool Management Flow
1. Pool manager maintains map of object classes to pools
2. Each pool maintains queue of available objects
3. Configuration stored per object class
4. Statistics tracked per pool
5. Performance monitoring enabled per configuration

### 4. Pre-warming Flow
1. Pool configuration specifies initial size
2. Objects created during pool initialization
3. All objects added to available pool
4. Statistics initialized with pre-warmed count

## Blueprint Integration

### Interface Implementation
- Blueprint classes can implement `IGWIZPoolable` interface
- Interface functions can be overridden in Blueprint
- Default implementations provided for non-implementing objects

### Manager Access
- Global access through `AGWIZPoolingManager::GetPoolingManager()`
- Blueprint functions for all major operations
- Editor integration for configuration

### Debug Features
- Blueprint callable debug functions
- Runtime statistics display
- Configurable debug logging per pool
- Performance monitoring integration

## Performance Considerations

### Memory Management
- Objects stored in TArray for fast access
- Pre-warmed initialization for immediate availability
- Smart object lifecycle management
- Memory usage tracking and monitoring

### Thread Safety
- Optional thread safety features
- Critical section protection for pool operations
- Safe concurrent access handling
- Configurable thread safety per pool

### Optimization Strategies
- Object reuse without recreation
- Minimal overhead for pool operations
- Efficient object state management
- Batch operations for bulk pool management
- Performance metrics collection and analysis

## Integration with Centralized Analytics System

### GWIZCentralMetricsReporter Integration
The pooling system integrates with the centralized analytics system to provide comprehensive game analytics and performance monitoring capabilities.

#### Analytics Collection
- **Pool Statistics**: All pool statistics are collected as comprehensive analytics events
- **Performance Data**: Real-time performance data from pool operations
- **Memory Usage**: Detailed memory usage tracking and analysis
- **Custom Analytics**: Support for custom pooling-specific analytics

#### Performance Testing
- **Automated Testing**: Performance tests with pooling vs non-pooling comparison
- **Benchmarking**: Automated benchmarking of pool performance
- **Regression Testing**: Analytics regression detection and reporting
- **Load Testing**: Stress testing with high object counts

#### Debug Output Replacement
- **Centralized Debug**: `PrintDebugInfo()` and `PrintAllPoolStatistics()` use centralized analytics reporter
- **Structured Logging**: Enhanced logging with severity levels and categories
- **Real-time Monitoring**: Live pooling system analytics in centralized dashboard
- **Historical Analysis**: Long-term analytics trend analysis

#### Data Export Integration
- **JSON Export**: Pooling analytics exported to JSON for data science analysis
- **CSV Export**: Pooling analytics exported to CSV for spreadsheet analysis
- **Custom Dashboards**: Custom analytics dashboards for pooling metrics
- **Data Export**: Comprehensive analytics export for external analysis

### Enhanced Pooling Manager
The `AGWIZPoolingManager` is enhanced with performance testing capabilities:

```cpp
// Performance testing function
UFUNCTION(BlueprintCallable, Category = "Pooling")
void PerformanceTest(TSubclassOf<AActor> ActorClass, int32 SpawnCount, 
                    float Lifetime, bool bUsePooling, 
                    const FGWIZPerformanceTestConfig& Config);
```

### Analytics Integration
Pooling statistics are automatically integrated into the centralized analytics system:

```cpp
// Automatic analytics collection
void AGWIZPoolingManager::CollectPoolingAnalytics()
{
    // Get session manager and create event
    UGWIZSessionManager* SessionManager = UGWIZSessionManager::GetSessionManager();
    FGWIZEventData PoolingEvent = SessionManager->CreateEvent("Performance", "PoolingSystem");
    
    // Add pooling-specific analytics data
    PoolingEvent.Data.SetInt("TotalObjects", GetTotalObjects());
    PoolingEvent.Data.SetInt64("MemoryUsage", GetTotalMemoryUsage());
    PoolingEvent.Data.SetFloat("CPUUsage", CalculateCPUUsage());
    PoolingEvent.Data.SetInt("PoolCount", GetPoolCount());
    PoolingEvent.Data.SetFloat("HitRate", GetGlobalHitRate());
    
    // Add nested data for detailed analysis
    FGWIZFlexibleData PoolDetails;
    PoolDetails.SetInt("ActivePools", GetActivePoolCount());
    PoolDetails.SetInt("InactivePools", GetInactivePoolCount());
    PoolDetails.SetFloat("AveragePoolSize", GetAveragePoolSize());
    PoolingEvent.Data.SetNested("PoolDetails", PoolDetails);
    
    // Send to centralized analytics reporter
    UGWIZCentralMetricsReporter::GetMetricsReporter()->CollectEvent(PoolingEvent);
}
```
