// Copyright (c) 2024 Groove Wizard, Inc. All Rights Reserved.
// This code is part of the GWIZ Generic Pooling System for Unreal Engine.

using UnrealBuildTool;

public class PlanetDefenderTests : ModuleRules
{
    public PlanetDefenderTests(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
        
        PrivateDependencyModuleNames.AddRange(new string[] { "PlanetDefender" });
        
        // Test framework dependencies
        PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd", "AutomationTest" });
        
        // Enable testing
        bEnableUndefinedIdentifierWarnings = false;
        bTreatAsEngineModule = false;
    }
}
