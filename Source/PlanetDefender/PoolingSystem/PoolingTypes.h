// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "PoolingTypes.generated.h"

/**
 * Configuration structure for pool settings.
 * Configurable per object type with sensible defaults.
 */
USTRUCT(BlueprintType)
struct PLANETDEFENDER_API FGWIZPoolConfig
{
    GENERATED_BODY()

public:
    FGWIZPoolConfig()
        : MinPoolSize(5)
        , MaxPoolSize(100)
        , InitialPoolSize(10)
        , bEnableDebug(false)
        , bEnableMonitoring(true)
        , bEnableThreadSafety(false)
    {
    }

    /** Minimum number of objects to keep in pool */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GWIZ Pooling", meta = (ClampMin = "1"))
    int32 MinPoolSize;

    /** Maximum number of objects in pool */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GWIZ Pooling", meta = (ClampMin = "1"))
    int32 MaxPoolSize;

    /** Initial pool size for pre-warming */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GWIZ Pooling", meta = (ClampMin = "0"))
    int32 InitialPoolSize;

    /** Whether to enable debug logging for this pool */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GWIZ Pooling")
    bool bEnableDebug;

    /** Whether to enable performance monitoring */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GWIZ Pooling")
    bool bEnableMonitoring;

    /** Whether to enable thread safety features */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GWIZ Pooling")
    bool bEnableThreadSafety;

    /** Validate configuration values */
    bool IsValid() const
    {
        return MinPoolSize > 0 && 
               MaxPoolSize >= MinPoolSize && 
               InitialPoolSize >= 0 && 
               InitialPoolSize <= MaxPoolSize;
    }
};

/**
 * Statistics structure for pool performance monitoring.
 * Tracks usage patterns and performance metrics.
 */
USTRUCT(BlueprintType)
struct PLANETDEFENDER_API FGWIZPoolStatistics
{
    GENERATED_BODY()

public:
    FGWIZPoolStatistics()
        : CurrentPoolSize(0)
        , ObjectsInUse(0)
        , TotalObjectsCreated(0)
        , PoolHits(0)
        , PoolMisses(0)
        , HitRate(0.0f)
        , MemoryUsage(0)
        , AverageLifetime(0.0f)
        , PeakConcurrentUsage(0)
        , LastUpdateTime(0.0f)
    {
    }

    /** Current number of objects in pool */
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Pooling")
    int32 CurrentPoolSize;

    /** Number of objects currently in use */
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Pooling")
    int32 ObjectsInUse;

    /** Number of objects created total */
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Pooling")
    int32 TotalObjectsCreated;

    /** Number of times pool was accessed successfully */
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Pooling")
    int32 PoolHits;

    /** Number of times new object was created */
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Pooling")
    int32 PoolMisses;

    /** Hit rate percentage (0.0 to 1.0) */
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Pooling")
    float HitRate;

    /** Memory usage in bytes */
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Pooling")
    int64 MemoryUsage;

    /** Average object lifetime in seconds */
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Pooling")
    float AverageLifetime;

    /** Peak concurrent usage */
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Pooling")
    int32 PeakConcurrentUsage;

    /** Last time statistics were updated */
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Pooling")
    float LastUpdateTime;

    /** Calculate hit rate based on hits and misses */
    void CalculateHitRate()
    {
        const int32 TotalAccesses = PoolHits + PoolMisses;
        HitRate = TotalAccesses > 0 ? static_cast<float>(PoolHits) / static_cast<float>(TotalAccesses) : 0.0f;
    }

    /** Get total objects (pooled + in use) */
    int32 GetTotalObjects() const
    {
        return CurrentPoolSize + ObjectsInUse;
    }

    /** Reset all statistics to zero */
    void Reset()
    {
        CurrentPoolSize = 0;
        ObjectsInUse = 0;
        TotalObjectsCreated = 0;
        PoolHits = 0;
        PoolMisses = 0;
        HitRate = 0.0f;
        MemoryUsage = 0;
        AverageLifetime = 0.0f;
        PeakConcurrentUsage = 0;
        LastUpdateTime = 0.0f;
    }
};
