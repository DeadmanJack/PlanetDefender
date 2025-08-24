// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IGWIZPoolable.h"
#include "GameFramework/Actor.h"
#include "GWIZPoolableHelpers.generated.h"

/**
 * Helper functions for implementing default poolable behavior.
 * Provides sensible defaults for objects that don't implement custom poolable logic.
 */
UCLASS(Abstract)
class PLANETDEFENDER_API UGWIZPoolableHelpers : public UObject
{
    GENERATED_BODY()

public:
    /**
     * Default OnPooled implementation for actors.
     * Enables ticking, rendering, and collision.
     * @param Actor Actor to initialize
     */
    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    static void DefaultOnPooled(AActor* Actor);

    /**
     * Default OnUnpooled implementation for actors.
     * Disables ticking, rendering, and collision.
     * @param Actor Actor to clean up
     */
    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    static void DefaultOnUnpooled(AActor* Actor);

    /**
     * Default IsPooled implementation.
     * Returns false (object is not pooled when this is called).
     * @return false (object is in use)
     */
    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling")
    static bool DefaultIsPooled();

    /**
     * Safe interface execution with null checks.
     * @param Object Object to execute interface function on
     * @param InterfaceClass Interface class to check
     * @return true if object implements the interface
     */
    template<typename T>
    static bool ImplementsInterface(const UObject* Object)
    {
        return Object && Object->GetClass()->ImplementsInterface(T::StaticClass());
    }


};
