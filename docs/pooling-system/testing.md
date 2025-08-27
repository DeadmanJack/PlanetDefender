# GWIZ Pooling System Testing Guide

## Overview

This document provides comprehensive guidance for testing the GWIZ Pooling System, including both C++ and Blueprint testing approaches. The testing framework is designed to be simple, reliable, and easy to use.

## Table of Contents

1. [Quick Start](#quick-start)
2. [C++ Testing](#c-testing)
3. [Blueprint Testing](#blueprint-testing)
4. [Test Functions Reference](#test-functions-reference)
5. [Interpreting Test Results](#interpreting-test-results)
6. [Troubleshooting](#troubleshooting)
7. [Advanced Testing](#advanced-testing)

## Quick Start

### Prerequisites

- Unreal Engine 5.4+ (Source Build)
- PlanetDefender project compiled successfully
- GWIZ Pooling System integrated into your project

### Running Your First Test

1. **C++ Test**: Call `RunQuickPoolingTest()` from any C++ code
2. **Blueprint Test**: Use the "Run Quick Pooling Test" function in Blueprints
3. **Console Test**: Use the console command `Test.Pooling.Quick` in the editor

## C++ Testing

### Available Test Functions

The GWIZ Pooling System provides several test functions that can be called from C++:

```cpp
// Quick test of basic functionality
void RunQuickPoolingTest();

// Comprehensive test suite
void RunAllPoolingTests();

// Individual test functions
void RunSimplePoolingTests();
```

### Including Test Functions

To use the test functions in your C++ code, include the test headers:

```cpp
#include "TestRunner.h"
#include "SimplePoolingTests.h"
```

### Example: Testing from Game Instance

```cpp
// In your GameInstance class
#include "TestRunner.h"

void UMyGameInstance::TestPoolingSystem()
{
    // Run quick test
    RunQuickPoolingTest();
    
    // Or run full test suite
    RunAllPoolingTests();
}
```

### Example: Testing from Actor

```cpp
// In your Actor class
#include "TestRunner.h"

void AMyActor::BeginPlay()
{
    Super::BeginPlay();
    
    // Test pooling system on startup
    if (bTestPoolingOnStartup)
    {
        RunQuickPoolingTest();
    }
}
```

### Example: Testing from Console Command

You can also create custom console commands for testing:

```cpp
// In your module's startup code
static void TestPoolingCommand(const TArray<FString>& Args, UWorld* World, FOutputDevice& Ar)
{
    RunQuickPoolingTest();
}

static FAutoConsoleCommand TestPoolingCmd(
    TEXT("Test.Pooling"),
    TEXT("Run pooling system tests"),
    FConsoleCommandWithWorldAndOutputDeviceDelegate::CreateStatic(TestPoolingCommand)
);
```

## Blueprint Testing

### Available Blueprint Functions

The test functions are exposed to Blueprints and can be found in the "GWIZ Pooling Tests" category:

- **Run Quick Pooling Test** - Quick test of basic functionality
- **Run All Pooling Tests** - Comprehensive test suite

### Setting Up Blueprint Tests

1. **Open Blueprint Editor**: Open any Blueprint that can execute functions
2. **Find Test Functions**: Search for "GWIZ Pooling" in the function list
3. **Call Test Functions**: Drag the test function into your Blueprint graph

### Example: Testing from Game Mode Blueprint

1. Open your Game Mode Blueprint
2. In the Event Graph, add an event (e.g., "Event BeginPlay")
3. Search for "Run Quick Pooling Test" in the function list
4. Connect the event to the test function
5. Compile and test

### Example: Testing from Widget Blueprint

1. Open your Widget Blueprint
2. Add a button to your widget
3. In the button's "On Clicked" event, call "Run Quick Pooling Test"
4. This allows you to test the pooling system from the UI

### Example: Testing from Level Blueprint

1. Open the Level Blueprint
2. Add an event (e.g., "Event BeginPlay" or "Event Tick")
3. Call the test function when the event fires
4. This allows you to test the pooling system at the level level

## Test Functions Reference

### RunQuickPoolingTest()

**Purpose**: Quick test of basic pooling functionality

**What it tests**:
- Pool creation and initialization
- Object retrieval from pool
- Object return to pool
- Basic pool statistics

**Output**: Log messages showing test results

**Example Output**:
```
LogTemp: === Quick Pooling Test ===
LogTemp: Pool created - Size: 3, In Use: 0
LogTemp: Object retrieved successfully
LogTemp: Object returned successfully
LogTemp: Quick test completed successfully
```

### RunAllPoolingTests()

**Purpose**: Comprehensive test suite covering all major functionality

**What it tests**:
- Basic pool creation and initialization
- Object retrieval and return functionality
- Pool configuration updates
- Pool statistics collection
- Pooling manager functionality

**Output**: Detailed log messages for each test

**Example Output**:
```
LogTemp: === Running GWIZ Pooling Tests ===
LogTemp: === Starting Simple Pooling Tests ===
LogTemp: Testing Basic Pool Creation...
LogTemp: Pool created successfully
LogTemp: Current pool size: 10
LogTemp: Objects in use: 0
LogTemp: Pool initialized: Yes
LogTemp: Testing Object Retrieval and Return...
LogTemp: Retrieved 3 objects
LogTemp: Pool size after retrieval: 7
LogTemp: Objects in use: 3
LogTemp: Returned all objects
LogTemp: Pool size after return: 10
LogTemp: Objects in use: 0
LogTemp: === Simple Pooling Tests Complete ===
LogTemp: === GWIZ Pooling Tests Complete ===
```

### RunSimplePoolingTests()

**Purpose**: Internal test suite used by RunAllPoolingTests()

**Note**: This function is called internally and typically not used directly

## Interpreting Test Results

### Successful Test Indicators

- **"Pool created successfully"** - Pool initialization works
- **"Object retrieved successfully"** - Object retrieval works
- **"Object returned successfully"** - Object return works
- **"Quick test completed successfully"** - All basic functionality works

### Warning Indicators

- **"Failed to create test pool"** - Pool creation issue
- **"Failed to retrieve pooled object"** - Object retrieval issue
- **"Failed to spawn pooling manager"** - Manager spawning issue

### Error Indicators

- **"Failed to create test world"** - World creation issue
- **"Failed to initialize pool"** - Pool initialization issue
- **Compilation errors** - Code issues that need fixing

### Understanding Pool Statistics

The tests output various pool statistics:

- **Current pool size**: Number of available objects in the pool
- **Objects in use**: Number of objects currently borrowed from the pool
- **Pool hits**: Number of successful object retrievals from pool
- **Pool misses**: Number of times new objects had to be created
- **Hit rate**: Percentage of successful pool retrievals

## Troubleshooting

### Common Issues

#### 1. Test Functions Not Found

**Problem**: Can't find test functions in Blueprints or C++

**Solution**:
- Ensure the test module is properly compiled
- Check that `#include "TestRunner.h"` is added to your C++ file
- Verify the test functions are exposed to Blueprints

#### 2. Compilation Errors

**Problem**: Test code won't compile

**Solution**:
- Check that all pooling system files compile successfully
- Ensure test includes are in the correct order
- Verify UE5 source build is properly configured

#### 3. Test Functions Not Working

**Problem**: Test functions run but don't produce expected results

**Solution**:
- Check the Output Log for error messages
- Verify pooling system is properly initialized
- Ensure test world creation is successful

#### 4. Blueprint Functions Not Available

**Problem**: Test functions don't appear in Blueprint function list

**Solution**:
- Recompile the test module
- Check that functions are marked as `UFUNCTION(BlueprintCallable)`
- Verify module dependencies are correct

### Debug Information

To get more detailed debug information, you can:

1. **Enable Verbose Logging**: Set log verbosity to "Verbose" in project settings
2. **Check Output Log**: Look for detailed test output in the Output Log window
3. **Use Debugger**: Set breakpoints in test functions to step through execution

## Advanced Testing

### Custom Test Scenarios

You can create custom test scenarios by:

1. **Extending Test Functions**: Create your own test functions based on the existing ones
2. **Testing Specific Classes**: Test pooling with your specific object classes
3. **Performance Testing**: Add performance measurements to your tests
4. **Stress Testing**: Create tests that stress the pooling system

### Example: Custom Performance Test

```cpp
void RunCustomPerformanceTest()
{
    UE_LOG(LogTemp, Log, TEXT("=== Custom Performance Test ==="));
    
    UGWIZObjectPool* TestPool = NewObject<UGWIZObjectPool>();
    TestPool->SetPooledObjectClass(UObject::StaticClass());
    TestPool->Config.MinPoolSize = 100;
    TestPool->Config.MaxPoolSize = 1000;
    TestPool->Config.InitialPoolSize = 500;
    TestPool->InitializePool();
    
    // Test rapid object retrieval
    TArray<UObject*> Objects;
    const int32 NumObjects = 1000;
    
    double StartTime = FPlatformTime::Seconds();
    
    for (int32 i = 0; i < NumObjects; ++i)
    {
        UObject* Obj = TestPool->GetFromPool();
        Objects.Add(Obj);
    }
    
    double EndTime = FPlatformTime::Seconds();
    double Duration = EndTime - StartTime;
    
    UE_LOG(LogTemp, Log, TEXT("Retrieved %d objects in %.3f seconds"), NumObjects, Duration);
    UE_LOG(LogTemp, Log, TEXT("Average time per object: %.6f seconds"), Duration / NumObjects);
    
    // Return all objects
    for (UObject* Obj : Objects)
    {
        TestPool->ReturnToPool(Obj);
    }
    
    TestPool->ClearPool();
}
```

### Integration Testing

For integration testing with your game:

1. **Test in Real Scenarios**: Test pooling in actual game scenarios
2. **Test with Real Objects**: Use your actual game objects in tests
3. **Test Performance**: Measure performance impact in real gameplay
4. **Test Memory Usage**: Monitor memory usage during tests

## Best Practices

### When to Run Tests

- **During Development**: Run tests frequently during development
- **Before Commits**: Run tests before committing code changes
- **After Integration**: Run tests after integrating pooling system
- **Performance Testing**: Run performance tests when optimizing

### Test Organization

- **Keep Tests Simple**: Focus on one aspect per test
- **Use Descriptive Names**: Name tests clearly to indicate what they test
- **Document Test Results**: Document expected vs actual results
- **Maintain Test Code**: Keep test code clean and up-to-date

### Test Data Management

- **Use Test Data**: Create specific test data for your scenarios
- **Clean Up After Tests**: Always clean up test objects and pools
- **Isolate Tests**: Ensure tests don't interfere with each other
- **Reset State**: Reset pooling system state between tests

## Conclusion

The GWIZ Pooling System provides a comprehensive testing framework that makes it easy to verify functionality and performance. By following this guide, you can effectively test your pooling system implementation and ensure it works correctly in your game.

For additional support or questions about testing, refer to the main pooling system documentation or contact the development team.
