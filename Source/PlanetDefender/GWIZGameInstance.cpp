// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.
// This code is part of the GWIZ Generic Pooling System for Unreal Engine.

#include "GWIZGameInstance.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonReader.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

UGWIZGameInstance::UGWIZGameInstance()
{
    GlobalPoolingManager = nullptr;
    bPoolingSystemInitialized = false;
}

void UGWIZGameInstance::Init()
{
    Super::Init();
    
    // Initialize the pooling system
    InitializePoolingSystem();
}

void UGWIZGameInstance::Shutdown()
{
    // Save pooling system state before shutdown
    if (bPoolingSystemInitialized)
    {
        SavePoolingSystemState();
    }
    
    // Cleanup global pooling manager
    if (GlobalPoolingManager != nullptr)
    {
        GlobalPoolingManager->ClearAllPools();
        GlobalPoolingManager = nullptr;
    }
    
    Super::Shutdown();
}

void UGWIZGameInstance::OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld)
{
    Super::OnWorldChanged(OldWorld, NewWorld);
    
    // Handle level transitions
    HandleLevelTransition(OldWorld, NewWorld);
}

void UGWIZGameInstance::InitializePoolingSystem()
{
    FScopeLock Lock(&PoolingMutex);
    
    if (bPoolingSystemInitialized)
    {
        return;
    }
    
    // Try to find existing pooling manager in the world
    if (GetWorld())
    {
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGWIZPoolingManager::StaticClass(), FoundActors);
        
        if (FoundActors.Num() > 0)
        {
            GlobalPoolingManager = Cast<AGWIZPoolingManager>(FoundActors[0]);
        }
    }
    
    // Create new pooling manager if none exists
    if (GlobalPoolingManager == nullptr && GetWorld())
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        SpawnParams.bDeferConstruction = false;
        
        GlobalPoolingManager = GetWorld()->SpawnActor<AGWIZPoolingManager>(
            AGWIZPoolingManager::StaticClass(), 
            FVector::ZeroVector, 
            FRotator::ZeroRotator, 
            SpawnParams
        );
    }
    
    if (GlobalPoolingManager != nullptr)
    {
        // Load persistent pool configurations
        LoadPoolConfigurations();
        
        // Mark as initialized
        bPoolingSystemInitialized = true;
        
        UE_LOG(LogTemp, Log, TEXT("GWIZGameInstance::InitializePoolingSystem - Pooling system initialized successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GWIZGameInstance::InitializePoolingSystem - Failed to create global pooling manager"));
    }
}

AGWIZPoolingManager* UGWIZGameInstance::GetGlobalPoolingManager() const
{
    FScopeLock Lock(&PoolingMutex);
    return GlobalPoolingManager;
}

bool UGWIZGameInstance::IsPoolingSystemInitialized() const
{
    FScopeLock Lock(&PoolingMutex);
    return bPoolingSystemInitialized;
}

void UGWIZGameInstance::SavePoolingSystemState()
{
    if (!bPoolingSystemInitialized || GlobalPoolingManager == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("GWIZGameInstance::SavePoolingSystemState - Pooling system not initialized"));
        return;
    }
    
    FScopeLock Lock(&PoolingMutex);
    
    // Save pool configurations
    SavePoolConfigurations();
    
    // Serialize pool state
    SerializePoolState();
    
    UE_LOG(LogTemp, Log, TEXT("GWIZGameInstance::SavePoolingSystemState - Pooling system state saved successfully"));
}

void UGWIZGameInstance::LoadPoolingSystemState()
{
    if (!bPoolingSystemInitialized || GlobalPoolingManager == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("GWIZGameInstance::LoadPoolingSystemState - Pooling system not initialized"));
        return;
    }
    
    FScopeLock Lock(&PoolingMutex);
    
    // Load pool configurations
    LoadPoolConfigurations();
    
    // Deserialize pool state
    DeserializePoolState();
    
    UE_LOG(LogTemp, Log, TEXT("GWIZGameInstance::LoadPoolingSystemState - Pooling system state loaded successfully"));
}

void UGWIZGameInstance::PreWarmPoolsForLevel(const FString& LevelName)
{
    if (!bPoolingSystemInitialized || GlobalPoolingManager == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("GWIZGameInstance::PreWarmPoolsForLevel - Pooling system not initialized"));
        return;
    }
    
    FScopeLock Lock(&PoolingMutex);
    
    // Setup pools for the specified level
    SetupPoolsForLevel(LevelName);
    
    // Pre-warm all pools
    GlobalPoolingManager->PreWarmAllPools();
    
    UE_LOG(LogTemp, Log, TEXT("GWIZGameInstance::PreWarmPoolsForLevel - Pools pre-warmed for level: %s"), *LevelName);
}

void UGWIZGameInstance::CleanupUnusedPoolsForLevel(const FString& LevelName)
{
    if (!bPoolingSystemInitialized || GlobalPoolingManager == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("GWIZGameInstance::CleanupUnusedPoolsForLevel - Pooling system not initialized"));
        return;
    }
    
    FScopeLock Lock(&PoolingMutex);
    
    // Cleanup unused pools
    GlobalPoolingManager->CleanupUnusedPools();
    
    // Cleanup level-specific pools
    CleanupPoolsForLevel(LevelName);
    
    UE_LOG(LogTemp, Log, TEXT("GWIZGameInstance::CleanupUnusedPoolsForLevel - Unused pools cleaned up for level: %s"), *LevelName);
}

void UGWIZGameInstance::HandleLevelTransition(UWorld* OldWorld, UWorld* NewWorld)
{
    if (!bPoolingSystemInitialized)
    {
        return;
    }
    
    FScopeLock Lock(&PoolingMutex);
    
    // Save state from old world
    if (OldWorld != nullptr)
    {
        FString OldLevelName = OldWorld->GetMapName();
        CleanupPoolsForLevel(OldLevelName);
    }
    
    // Setup pools for new world
    if (NewWorld != nullptr)
    {
        FString NewLevelName = NewWorld->GetMapName();
        SetupPoolsForLevel(NewLevelName);
        
        // Pre-warm pools for the new level
        PreWarmPoolsForLevel(NewLevelName);
    }
}

void UGWIZGameInstance::SavePoolConfigurations()
{
    if (GlobalPoolingManager == nullptr)
    {
        return;
    }
    
    // Get all pools and their configurations
    TArray<UGWIZObjectPool*> AllPools = GlobalPoolingManager->GetAllPools();
    
    for (UGWIZObjectPool* Pool : AllPools)
    {
        if (Pool != nullptr && Pool->GetPooledObjectClass() != nullptr)
        {
            FString ClassName = Pool->GetPooledObjectClass()->GetName();
            PersistentPoolConfigs.Add(ClassName, Pool->Config);
        }
    }
}

void UGWIZGameInstance::LoadPoolConfigurations()
{
    if (GlobalPoolingManager == nullptr)
    {
        return;
    }
    
    // Apply saved configurations to existing pools
    for (const auto& ConfigPair : PersistentPoolConfigs)
    {
        const FString& ClassName = ConfigPair.Key;
        const FGWIZPoolConfig& Config = ConfigPair.Value;
        
        // Find the class and configure the pool
        UClass* ObjectClass = FindObject<UClass>(nullptr, *ClassName);
        if (ObjectClass != nullptr)
        {
            GlobalPoolingManager->ConfigurePool(ObjectClass, Config);
        }
    }
}

void UGWIZGameInstance::SetupPoolsForLevel(const FString& LevelName)
{
    if (GlobalPoolingManager == nullptr)
    {
        return;
    }
    
    // Check if we have level-specific configurations
    if (LevelPoolConfigs.Contains(LevelName))
    {
        const FGWIZLevelPoolConfigs& LevelConfigs = LevelPoolConfigs[LevelName];
        
        for (const auto& ConfigPair : LevelConfigs.PoolConfigs)
        {
            const FString& ClassName = ConfigPair.Key;
            const FGWIZPoolConfig& Config = ConfigPair.Value;
            
            // Find the class and configure the pool
            UClass* ObjectClass = FindObject<UClass>(nullptr, *ClassName);
            if (ObjectClass != nullptr)
            {
                GlobalPoolingManager->ConfigurePool(ObjectClass, Config);
            }
        }
    }
}

void UGWIZGameInstance::CleanupPoolsForLevel(const FString& LevelName)
{
    if (GlobalPoolingManager == nullptr)
    {
        return;
    }
    
    // Remove level-specific configurations
    LevelPoolConfigs.Remove(LevelName);
    
    // Note: We don't destroy the pools themselves as they might be needed
    // for other levels. We just clean up unused objects.
}

void UGWIZGameInstance::SerializePoolState()
{
    // Create JSON object for pool state
    TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);
    
    // Serialize persistent pool configurations
    TSharedPtr<FJsonObject> ConfigsObject = MakeShareable(new FJsonObject);
    for (const auto& ConfigPair : PersistentPoolConfigs)
    {
        const FString& ClassName = ConfigPair.Key;
        const FGWIZPoolConfig& Config = ConfigPair.Value;
        
        TSharedPtr<FJsonObject> ConfigObject = MakeShareable(new FJsonObject);
        ConfigObject->SetNumberField(TEXT("MinPoolSize"), Config.MinPoolSize);
        ConfigObject->SetNumberField(TEXT("MaxPoolSize"), Config.MaxPoolSize);
        ConfigObject->SetNumberField(TEXT("InitialPoolSize"), Config.InitialPoolSize);
        ConfigObject->SetNumberField(TEXT("Priority"), static_cast<double>(Config.Priority));
        ConfigObject->SetStringField(TEXT("Category"), Config.Category);
        
        ConfigsObject->SetObjectField(ClassName, ConfigObject);
    }
    RootObject->SetObjectField("PoolConfigs", ConfigsObject);
    
    // Serialize level-specific configurations
    TSharedPtr<FJsonObject> LevelConfigsObject = MakeShareable(new FJsonObject);
    for (const auto& LevelPair : LevelPoolConfigs)
    {
        const FString& LevelName = LevelPair.Key;
        const FGWIZLevelPoolConfigs& LevelConfigs = LevelPair.Value;
        
        TSharedPtr<FJsonObject> LevelObject = MakeShareable(new FJsonObject);
        for (const auto& ConfigPair : LevelConfigs.PoolConfigs)
        {
            const FString& ClassName = ConfigPair.Key;
            const FGWIZPoolConfig& Config = ConfigPair.Value;
            
            TSharedPtr<FJsonObject> ConfigObject = MakeShareable(new FJsonObject);
            ConfigObject->SetNumberField(TEXT("MinPoolSize"), Config.MinPoolSize);
            ConfigObject->SetNumberField(TEXT("MaxPoolSize"), Config.MaxPoolSize);
            ConfigObject->SetNumberField(TEXT("InitialPoolSize"), Config.InitialPoolSize);
            ConfigObject->SetNumberField(TEXT("Priority"), static_cast<double>(Config.Priority));
            ConfigObject->SetStringField(TEXT("Category"), Config.Category);
            
            LevelObject->SetObjectField(ClassName, ConfigObject);
        }
        LevelConfigsObject->SetObjectField(LevelName, LevelObject);
    }
    RootObject->SetObjectField("LevelConfigs", LevelConfigsObject);
    
    // Convert to JSON string
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);
    
    // Save to file
    FString SavePath = FPaths::ProjectSavedDir() / TEXT("PoolingSystem") / TEXT("PoolState.json");
    FFileHelper::SaveStringToFile(OutputString, *SavePath);
}

void UGWIZGameInstance::DeserializePoolState()
{
    FString SavePath = FPaths::ProjectSavedDir() / TEXT("PoolingSystem") / TEXT("PoolState.json");
    
    if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*SavePath))
    {
        return;
    }
    
    FString JsonString;
    if (!FFileHelper::LoadFileToString(JsonString, *SavePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("GWIZGameInstance::DeserializePoolState - Failed to load pool state file"));
        return;
    }
    
    // Parse JSON
    TSharedPtr<FJsonObject> RootObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    if (!FJsonSerializer::Deserialize(Reader, RootObject))
    {
        UE_LOG(LogTemp, Warning, TEXT("GWIZGameInstance::DeserializePoolState - Failed to parse pool state JSON"));
        return;
    }
    
    // Deserialize persistent pool configurations
    const TSharedPtr<FJsonObject>* ConfigsObject;
    if (RootObject->TryGetObjectField(TEXT("PoolConfigs"), ConfigsObject))
    {
        for (const auto& ConfigPair : (*ConfigsObject)->Values)
        {
            const FString& ClassName = ConfigPair.Key;
            const TSharedPtr<FJsonValue>& ConfigValue = ConfigPair.Value;
            
            if (ConfigValue->Type == EJson::Object)
            {
                TSharedPtr<FJsonObject> ConfigObject = ConfigValue->AsObject();
                
                FGWIZPoolConfig Config;
                Config.MinPoolSize = ConfigObject->GetIntegerField(TEXT("MinPoolSize"));
                Config.MaxPoolSize = ConfigObject->GetIntegerField(TEXT("MaxPoolSize"));
                Config.InitialPoolSize = ConfigObject->GetIntegerField(TEXT("InitialPoolSize"));
                Config.Priority = static_cast<uint8>(ConfigObject->GetNumberField(TEXT("Priority")));
                Config.Category = ConfigObject->GetStringField(TEXT("Category"));
                
                PersistentPoolConfigs.Add(ClassName, Config);
            }
        }
    }
    
    // Deserialize level-specific configurations
    const TSharedPtr<FJsonObject>* LevelConfigsObject;
    if (RootObject->TryGetObjectField(TEXT("LevelConfigs"), LevelConfigsObject))
    {
        for (const auto& LevelPair : (*LevelConfigsObject)->Values)
        {
            const FString& LevelName = LevelPair.Key;
            const TSharedPtr<FJsonValue>& LevelValue = LevelPair.Value;
            
            if (LevelValue->Type == EJson::Object)
            {
                TSharedPtr<FJsonObject> LevelObject = LevelValue->AsObject();
                FGWIZLevelPoolConfigs LevelConfigs;
                
                for (const auto& ConfigPair : LevelObject->Values)
                {
                    const FString& ClassName = ConfigPair.Key;
                    const TSharedPtr<FJsonValue>& ConfigValue = ConfigPair.Value;
                    
                    if (ConfigValue->Type == EJson::Object)
                    {
                        TSharedPtr<FJsonObject> ConfigObject = ConfigValue->AsObject();
                        
                        FGWIZPoolConfig Config;
                        Config.MinPoolSize = ConfigObject->GetIntegerField(TEXT("MinPoolSize"));
                        Config.MaxPoolSize = ConfigObject->GetIntegerField(TEXT("MaxPoolSize"));
                        Config.InitialPoolSize = ConfigObject->GetIntegerField(TEXT("InitialPoolSize"));
                        Config.Priority = static_cast<uint8>(ConfigObject->GetNumberField(TEXT("Priority")));
                        Config.Category = ConfigObject->GetStringField(TEXT("Category"));
                        
                        LevelConfigs.SetPoolConfig(ClassName, Config);
                    }
                }
                
                LevelPoolConfigs.Add(LevelName, LevelConfigs);
            }
        }
    }
}

