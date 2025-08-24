// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.
// This code is part of the GWIZ Generic Pooling System for Unreal Engine.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GWIZPoolConfig.h"
#include "GWIZPoolStatistics.h"
#include "GWIZObjectPool.generated.h"

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
    
    // Set pool configuration
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    void SetConfig(const FGWIZPoolConfig& NewConfig);
    
    // Check if pool is empty
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    bool IsEmpty() const;
    
    // Check if pool is full
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    bool IsFull() const;
    
    // Get pool hit rate
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    float GetHitRate() const;
    
    // Force cleanup of unused objects
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    void ForceCleanup();
    
    // Get pool category
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    FString GetPoolCategory() const;
    
    // Get pool priority
    UFUNCTION(BlueprintCallable, Category = "Pooling")
    int32 GetPoolPriority() const;    
    
protected:
    // Initialize properties after object creation
    virtual void PostInitProperties() override;
};
