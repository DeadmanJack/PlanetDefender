// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.
// This code is part of the GWIZ Generic Pooling System for Unreal Engine.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GWIZObjectPool.h"
#include "PoolingTypes.h"
#include "GWIZPoolingManager.generated.h"

UCLASS()
class PLANETDEFENDER_API AGWIZPoolingManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
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

	// Enable/disable performance monitoring
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
	bool bEnablePerformanceMonitoring = true;

	// Enable/disable thread safety
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
	bool bEnableThreadSafety = false;

	// Enable/disable performance monitoring
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pooling")
	bool bEnableAutoCleanup = true;

	// Get all pools
	UFUNCTION(BlueprintCallable, Category = "Pooling")
	TArray<UGWIZObjectPool*> GetAllPools() const;

	// Get pool count
	UFUNCTION(BlueprintCallable, Category = "Pooling")
	int32 GetPoolCount() const;

	// Clear all pools
	UFUNCTION(BlueprintCallable, Category = "Pooling")
	void ClearAllPools();

	// Get pool for specific object class
	UFUNCTION(BlueprintCallable, Category = "Pooling")
	UGWIZObjectPool* GetPoolForClass(TSubclassOf<UObject> ObjectClass) const;

	// Get pools by category
	UFUNCTION(BlueprintCallable, Category = "Pooling")
	TArray<UGWIZObjectPool*> GetPoolsByCategory(const FString& Category) const;

	// Get pools by priority
	UFUNCTION(BlueprintCallable, Category = "Pooling")
	TArray<UGWIZObjectPool*> GetPoolsByPriority(int32 Priority) const;

	// Cleanup unused pools
	UFUNCTION(BlueprintCallable, Category = "Pooling")
	void CleanupUnusedPools();

	// Get total memory usage across all pools
	UFUNCTION(BlueprintCallable, Category = "Pooling")
	int64 GetTotalMemoryUsage() const;

	// Get total objects across all pools
	UFUNCTION(BlueprintCallable, Category = "Pooling")
	int32 GetTotalObjects() const;

	// Get total objects in use across all pools
	UFUNCTION(BlueprintCallable, Category = "Pooling")
	int32 GetTotalObjectsInUse() const;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
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
