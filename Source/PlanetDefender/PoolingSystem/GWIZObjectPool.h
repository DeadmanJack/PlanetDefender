// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PoolingTypes.h"
#include "IGWIZPoolable.h"
#include "GWIZObjectPool.generated.h"

/**
 * Individual object pool for a specific object type.
 * Manages object lifecycle, statistics, and pool operations.
 */
UCLASS(BlueprintType, Blueprintable)
class PLANETDEFENDER_API UGWIZObjectPool : public UObject
{
    GENERATED_BODY()

public:
    UGWIZObjectPool();

    /** Pool configuration */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GWIZ Pooling")
    FGWIZPoolConfig Config;

    /** Pool statistics */
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Pooling")
    FGWIZPoolStatistics Statistics;

    /**
     * Get object from pool. If pool is empty, creates new object.
     * @param ObjectClass Class of object to get
     * @return Object from pool or newly created object
     */
    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    UObject* GetObject(TSubclassOf<UObject> ObjectClass);

    /**
     * Return object to pool for reuse.
     * @param Object Object to return to pool
     */
    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    void ReturnObject(UObject* Object);

    /**
     * Add object to pool manually.
     * @param Object Object to add to pool
     */
    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    void AddToPool(UObject* Object);

    /**
     * Remove object from pool permanently.
     * @param Object Object to remove from pool
     */
    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    void RemoveFromPool(UObject* Object);

    /**
     * Clear entire pool, destroying all objects.
     */
    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    void ClearPool();

    /**
     * Pre-warm pool with specified number of objects.
     * @param Count Number of objects to create
     */
    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    void PreWarmPool(int32 Count);

    /**
     * Get current pool statistics.
     * @return Copy of current statistics
     */
    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    FGWIZPoolStatistics GetStatistics() const;

    /**
     * Print debug information to log.
     */
    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    void PrintDebugInfo();

    /**
     * Get memory usage information.
     * @return Memory usage in bytes
     */
    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    int64 GetMemoryUsage() const;

    /**
     * Validate pool integrity and consistency.
     * @return true if pool is valid, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    bool ValidatePool() const;

    /**
     * Get current pool size (number of available objects).
     * @return Number of objects currently in pool
     */
    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    int32 GetPoolSize() const;

    /**
     * Get number of objects currently in use.
     * @return Number of objects currently borrowed from pool
     */
    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    int32 GetObjectsInUse() const;

    /**
     * Check if pool is empty (no available objects).
     * @return true if pool is empty, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    bool IsPoolEmpty() const;

    /**
     * Check if pool is at maximum capacity.
     * @return true if pool is full, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    bool IsPoolFull() const;

protected:
    /** Array of available objects in the pool */
    UPROPERTY()
    TArray<UObject*> AvailableObjects;

    /** Array of objects currently in use */
    UPROPERTY()
    TArray<UObject*> ObjectsInUse;

    /** Class type this pool manages */
    UPROPERTY()
    TSubclassOf<UObject> PooledObjectClass;

    /** Whether pool has been initialized */
    UPROPERTY()
    bool bIsInitialized;

    /** Thread safety mutex */
    FCriticalSection PoolMutex;

    /**
     * Create new object instance.
     * @return Newly created object
     */
    UObject* CreateNewObject();

    /**
     * Initialize object for use (call OnPooled).
     * @param Object Object to initialize
     */
    void InitializeObject(UObject* Object);

    /**
     * Clean up object for pooling (call OnUnpooled).
     * @param Object Object to clean up
     */
    void CleanupObject(UObject* Object);

    /**
     * Update pool statistics.
     */
    void UpdateStatistics();

    /**
     * Calculate memory usage of pool.
     */
    void CalculateMemoryUsage();

    /**
     * Log debug information if enabled.
     * @param Message Debug message to log
     */
    void LogDebug(const FString& Message);

    /**
     * Validate object before adding to pool.
     * @param Object Object to validate
     * @return true if object is valid, false otherwise
     */
    bool ValidateObject(UObject* Object) const;
};
