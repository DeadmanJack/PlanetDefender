// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.
// This code is part of the GWIZ Generic Pooling System for Unreal Engine.

#pragma once

#include "CoreMinimal.h"
#include "GWIZPoolConfig.generated.h"

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
    
    // Pool category for grouping related objects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
    FString PoolCategory = TEXT("Default");
    
    // Priority level for this pool (higher = more important)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
    int32 Priority = 0;
    
    // Auto-cleanup timeout in seconds (0 = no auto-cleanup)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
    float AutoCleanupTimeout = 0.0f;
};
