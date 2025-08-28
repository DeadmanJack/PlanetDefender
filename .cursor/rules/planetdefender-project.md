# PlanetDefender Project Configuration

## Unreal Engine Setup
- UE5 Source Build Location: d:\UE5\UnrealEngine
- Project Type: Unreal Engine 5.4 C++ Project
- Build System: UnrealBuildTool

## Project Structure
- Main Game Module: PlanetDefender
- Pooling System: Custom GWIZ Pooling System
- Test Module: PlanetDefenderTests

## Build Commands
- Generate Project Files: Use UE5 source build at d:\UE5\UnrealEngine
- Compile: Use UnrealBuildTool from UE5 source build
- Editor: Use UnrealEditor from UE5 source build

## Critical Development Rules
- **NEVER make up UE5 APIs** - Always verify against actual UE5 source code
- **NEVER assume API compatibility** - UE5 APIs change between versions
- **ALWAYS research actual working APIs** before implementing
- **Use the UE5 source build at d:\UE5\UnrealEngine** for API verification
- **Test compilation immediately** after any API changes

## Important Notes
- Always use the UE5 source build at d:\UE5\UnrealEngine for compilation
- This is a custom UE5 build, not the Epic Games launcher version
- Pooling system uses custom USTRUCT types that require proper UHT compilation
- When in doubt about UE5 APIs, search the actual source code or ask the user to research
