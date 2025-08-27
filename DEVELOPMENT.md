# Development Guidelines

## For AI Assistants and Contributors

### Code Verification Process
1. **Always check actual implementation** before making changes
2. **Search codebase** for existing patterns and method names
3. **Verify API macros** (PLANETDEFENDER_API for embedded code)
4. **Use real Unreal Engine API** names, not assumed ones

### Project Structure
- **Type**: Embedded Unreal Engine project (not plugin)
- **API Macro**: `PLANETDEFENDER_API` for all classes
- **Pooling System**: Located in `Source/PlanetDefender/PoolingSystem/`
- **Naming Convention**: GWIZ prefix for pooling system components

### Common Mistakes to Avoid
- **Don't assume Unreal Engine API method names** (e.g., `SetLocation` vs `SetActorLocation`)
- **Don't change API macros** without understanding project structure
- **Don't add unnecessary complexity** to integration guides
- **Don't use generic examples** when project-specific ones exist
- **Don't suggest plugin conversion** unless specifically requested

### Quality Checklist
Before making any changes:
- [ ] Read actual source files to understand current implementation
- [ ] Search codebase for existing patterns and method names
- [ ] Verify API macros being used in the project
- [ ] Test assumptions against actual code, not documentation
- [ ] Use real Unreal Engine API names, not assumed ones

### Documentation Standards
- API documentation must match actual method signatures
- Usage examples must use real Unreal Engine method names
- Integration guides must reflect actual project structure
- Don't assume custom module setup is needed for standard projects
- Verify all code examples compile and work

### Verification Process
1. Search codebase for existing implementations
2. Read actual header files to understand current API
3. Check method signatures against Unreal Engine documentation
4. Verify project structure before suggesting architectural changes
5. Test examples against actual codebase

### Error Prevention
- If unsure about a method name, search the codebase first
- If unsure about API macros, check existing class declarations
- If unsure about project structure, examine the actual file organization
- When in doubt, prefer reading over writing
- Always verify before making changes

## For Human Developers

### Getting Started
1. Familiarize yourself with the project structure
2. Understand the embedded nature of the pooling system
3. Review existing code patterns before making changes
4. Follow the established naming conventions

### Code Style
- Use `PLANETDEFENDER_API` for all classes
- Follow GWIZ prefix convention for pooling system
- Use actual Unreal Engine method names
- Verify against existing patterns in the codebase

### Testing
- Test all code examples against actual implementation
- Verify Blueprint integration works as documented
- Check that API documentation matches real signatures
- Ensure integration guides work with current project setup
