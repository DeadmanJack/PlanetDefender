// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.
// This code is part of the GWIZ Generic Pooling System for Unreal Engine.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GWIZPoolable.generated.h"

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
        
        // Reset object state for reuse
        UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pooling")
        void ResetForReuse();
        
        // Get object's pool identifier
        UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pooling")
        FString GetPoolIdentifier() const;
};