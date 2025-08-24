// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IGWIZPoolable.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, MinimalAPI)
class UGWIZPoolable : public UInterface
{
    GENERATED_BODY()
};

/**
 * Interface for objects that can be pooled by the GWIZ pooling system.
 * Provides lifecycle management methods for pool operations.
 */
class PLANETDEFENDER_API IGWIZPoolable
{
    GENERATED_BODY()

public:
    /**
     * Called when object is taken from the pool and is about to be used.
     * Implement this to initialize/reset the object state.
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GWIZ Pooling")
    void OnPooled();
    
    /**
     * Called when object is returned to the pool and is no longer in use.
     * Implement this to clean up the object state.
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GWIZ Pooling")
    void OnUnpooled();
    
    /**
     * Check if object is currently pooled (not in use).
     * @return true if object is in the pool, false if it's currently in use
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GWIZ Pooling")
    bool IsPooled() const;
};
