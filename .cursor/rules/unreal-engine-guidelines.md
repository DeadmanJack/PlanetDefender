# Unreal Engine Guidelines

## API Usage Rules
- **Always verify Unreal Engine method names exist** before using in examples
- **Check project structure** (embedded vs plugin) before suggesting API changes
- **Use actual codebase patterns**, not generic examples
- **Verify method signatures** against actual Unreal Engine API

## Common Unreal Engine Mistakes to Avoid
- Using `SetLocation()` instead of `SetActorLocation()`
- Using `SetVelocity()` instead of proper movement methods
- Assuming method names without checking Unreal Engine documentation
- Using generic examples instead of project-specific patterns

## Project Structure Understanding
- This project uses **embedded code** (not plugins)
- API macro: `PLANETDEFENDER_API` (not plugin APIs)
- Pooling system location: `Source/PlanetDefender/PoolingSystem/`
- Standard Unreal Engine project structure

## Verification Process for UE Code
1. Search codebase for existing method usage
2. Check Unreal Engine documentation for method names
3. Verify against actual header files in project
4. Test assumptions against real implementation
5. Use actual Blueprint node names and categories

## Blueprint Integration Rules
- Use actual Blueprint category names from the codebase
- Verify Blueprint node signatures match C++ methods
- Check interface implementation patterns
- Use real Unreal Engine Blueprint nodes, not assumed ones
