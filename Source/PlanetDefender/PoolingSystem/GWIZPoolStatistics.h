// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.
// This code is part of the GWIZ Generic Pooling System for Unreal Engine.

#pragma once

#include "CoreMinimal.h"
#include "GWIZPoolStatistics.generated.h"

USTRUCT(BlueprintType)
struct PLANETDEFENDER_API FGWIZPoolStatistics
{
    GENERATED_BODY()

    // Current number of objects in pool
    UPROPERTY(BlueprintReadOnly, Category = "Pooling")
    int32 CurrentPoolSize = 0;

    // number of objects currently in use
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

    // Total time spent in pool
    UPROPERTY(BlueprintReadOnly, Category = "Pooling")
    float TotalTimeInPool = 0.0f;

    // Average time spent in pool
    UPROPERTY(BlueprintReadOnly, Category = "Pooling")
    float AverageTimeInPool = 0.0f;
    
    // Last access time
    UPROPERTY(BlueprintReadOnly, Category = "Pooling")
    float LastAccessTime = 0.0f;
    
    // Pool creation time
    UPROPERTY(BlueprintReadOnly, Category = "Pooling")
    float PoolCreationTime = 0.0f;
    
    // Number of times pool was resized
    UPROPERTY(BlueprintReadOnly, Category = "Pooling")
    int32 PoolResizeCount = 0;
    
    // Number of cleanup operations performed
    UPROPERTY(BlueprintReadOnly, Category = "Pooling")
    int32 CleanupCount = 0;
};