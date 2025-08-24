// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.

#include "GWIZPoolableHelpers.h"
#include "Engine/Engine.h"

void UGWIZPoolableHelpers::DefaultOnPooled(AActor* Actor)
{
    if (!Actor)
    {
        return;
    }

    // Enable ticking
    Actor->SetActorTickEnabled(true);
    
    // Enable rendering
    Actor->SetActorHiddenInGame(false);
    
    // Enable collision
    Actor->SetActorEnableCollision(true);
    
    // Log debug info
    UE_LOG(LogTemp, Log, TEXT("[GWIZ Helpers] DefaultOnPooled: Actor %s initialized"), *Actor->GetName());
}

void UGWIZPoolableHelpers::DefaultOnUnpooled(AActor* Actor)
{
    if (!Actor)
    {
        return;
    }

    // Disable ticking
    Actor->SetActorTickEnabled(false);
    
    // Disable rendering
    Actor->SetActorHiddenInGame(true);
    
    // Disable collision
    Actor->SetActorEnableCollision(false);
    
    // Log debug info
    UE_LOG(LogTemp, Log, TEXT("[GWIZ Helpers] DefaultOnUnpooled: Actor %s cleaned up"), *Actor->GetName());
}

bool UGWIZPoolableHelpers::DefaultIsPooled()
{
    // When this function is called, the object is typically in use (not pooled)
    // So we return false to indicate it's not currently pooled
    return false;
}
