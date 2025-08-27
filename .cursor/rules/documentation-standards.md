# Documentation Standards

## API Documentation Requirements
- API documentation must match actual method signatures
- Usage examples must use real Unreal Engine method names
- Integration guides must reflect actual project structure
- Don't assume custom module setup is needed for standard projects
- Verify all code examples compile and work

## Common Mistakes to Avoid
- Don't assume Unreal Engine API method names (e.g., `SetLocation` vs `SetActorLocation`)
- Don't change API macros without understanding project structure
- Don't add unnecessary complexity to integration guides
- Don't use generic examples when project-specific ones exist
- Don't suggest plugin conversion unless specifically requested

## Documentation Quality Checklist
- [ ] Verify method names against actual Unreal Engine API
- [ ] Check API macros match current project structure
- [ ] Test code examples against actual implementation
- [ ] Ensure integration steps match project setup
- [ ] Verify Blueprint node names and categories

## Project-Specific Documentation Rules
- Use `PLANETDEFENDER_API` in all code examples (not plugin APIs)
- Reference actual file paths in `Source/PlanetDefender/PoolingSystem/`
- Use GWIZ prefix consistently in examples
- Show actual Unreal Engine method names, not assumed ones
- Include both C++ and Blueprint examples where applicable
