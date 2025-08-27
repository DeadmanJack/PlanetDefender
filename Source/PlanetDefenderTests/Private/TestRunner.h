// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.
// This code is part of the GWIZ Generic Pooling System for Unreal Engine.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "TestRunner.generated.h"

/**
 * Simple test runner for GWIZ Pooling System.
 * Provides functions that can be called from game instance or console.
 */

UCLASS()
class PLANETDEFENDERTESTS_API UGWIZPoolingTestRunner : public UObject
{
    GENERATED_BODY()

public:
    /** Run a quick test of basic pooling functionality */
    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling Tests", CallInEditor)
    static void RunQuickPoolingTest();

    /** Run all comprehensive pooling tests */
    UFUNCTION(BlueprintCallable, Category = "GWIZ Pooling Tests", CallInEditor)
    static void RunAllPoolingTests();
};

/** Run a quick test of basic pooling functionality */
void RunQuickPoolingTest();

/** Run all comprehensive pooling tests */
void RunAllPoolingTests();
