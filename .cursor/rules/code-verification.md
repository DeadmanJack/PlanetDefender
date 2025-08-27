# Code Verification Rules

## Core Principles
- **ALWAYS verify existing implementation before suggesting changes**
- **Search codebase for actual method names and API patterns before using them**
- **Check current API macros before changing them**
- **Verify Unreal Engine method names against actual codebase, not assumptions**

## Pre-Change Checklist
Before making any code/documentation changes:
1. Read actual source files to understand current implementation
2. Search codebase for existing patterns and method names
3. Verify API macros being used in the project
4. Test assumptions against actual code, not documentation
5. Use real Unreal Engine API names, not assumed ones

## Unreal Engine Specific Rules
- Always verify Unreal Engine method names exist before using in examples
- Check project structure (embedded vs plugin) before suggesting API changes
- Use actual codebase patterns, not generic examples
- This project uses `PLANETDEFENDER_API` for embedded code (not plugin APIs)
- Verify method signatures against actual Unreal Engine API

## Project-Specific Guidelines
- Pooling system is embedded in PlanetDefender project, not a separate plugin
- Use `PLANETDEFENDER_API` for all pooling system classes
- Check actual implementation in `Source/PlanetDefender/PoolingSystem/` before making changes
- Verify against existing code patterns in the project
- Follow the established naming conventions (GWIZ prefix for pooling system)

## Error Prevention
- If unsure about a method name, search the codebase first
- If unsure about API macros, check existing class declarations
- If unsure about project structure, examine the actual file organization
- When in doubt, prefer reading over writing
- Always verify before making changes
