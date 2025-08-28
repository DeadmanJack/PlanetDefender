# GWIZ Central Metrics Reporter - Documentation

## Overview

The GWIZ Central Metrics Reporter is a comprehensive **game analytics and performance monitoring system** designed to provide unified data collection capabilities across all game systems. It serves as a centralized hub for collecting, processing, and exporting performance data, game analytics, player behavior metrics, and operational statistics.

**Key Design Philosophy**: This system is designed as a **data science solution** that enables comprehensive analytics beyond just performance metrics, including player behavior analysis, game balance insights, and business intelligence.

## Documentation Structure

### Core Documentation
- **[GWIZCentralMetricsReporter_Design.md](GWIZCentralMetricsReporter_Design.md)** - Complete technical design and architecture
- **[GWIZCentralMetricsReporter_Implementation_Plan.md](GWIZCentralMetricsReporter_Implementation_Plan.md)** - Detailed implementation plan and phases

### Integration Documentation
- **[Usage_Examples.md](Usage_Examples.md)** - Practical usage examples and tutorials
- **[Performance_Testing_Guide.md](Performance_Testing_Guide.md)** - Performance testing guide and examples
- **[API_Reference.md](API_Reference.md)** - Complete API documentation
- **[Plugin_Integration.md](Plugin_Integration.md)** - Plugin integration guide

## Key Features

### Core Capabilities
- **Unified Analytics Collection**: Single system for all performance and game analytics
- **Performance Testing**: Automated performance testing with pooling vs non-pooling comparison
- **Real-time Monitoring**: Live analytics monitoring and alerting
- **Historical Analysis**: Analytics trend analysis and regression detection
- **Data Export**: JSON and CSV export for data science analysis

### Analytics Categories
- **Performance Analytics**: Frame rates, memory usage, load times, system performance
- **Gameplay Analytics**: Player behavior, game balance, engagement metrics, social features
- **Business Intelligence**: Monetization, user acquisition, retention, content performance
- **Technical Analytics**: Error tracking, platform performance, build analysis, infrastructure

### Integration Points
- **Pooling System**: Enhanced integration with GWIZ Pooling System plugin
- **Test Framework**: Integration with existing test framework
- **Game Engine**: Leverages Unreal Engine's built-in capabilities
- **Data Science Tools**: Export for external analytics and visualization tools

## Quick Start

### Basic Usage
```cpp
// Get the session manager and initialize
UGWIZSessionManager* SessionManager = UGWIZSessionManager::GetSessionManager();
SessionManager->InitializeSession();

// Get the analytics reporter
UGWIZCentralMetricsReporter* Reporter = UGWIZCentralMetricsReporter::GetMetricsReporter();

// Create an analytics event
FGWIZEventData GameplayEvent = SessionManager->CreateEvent("Gameplay", "CombatSystem");
GameplayEvent.Data.SetString("Action", "AbilityUsed");
GameplayEvent.Data.SetString("AbilityName", "Fireball");
GameplayEvent.Data.SetVector("PlayerPosition", PlayerLocation);
GameplayEvent.Data.SetFloat("AbilityDamage", 150.0f);

// Collect the event
Reporter->CollectEvent(GameplayEvent);

// Run a performance test
FGWIZPerformanceTestConfig Config;
Config.TestDuration = 30.0f;
Config.SpawnInterval = 0.1f;
Reporter->PerformanceTest(AActor::StaticClass(), 100, 5.0f, true, Config);
```

### Blueprint Usage
1. Get the Session Manager using "Get Session Manager"
2. Initialize the session using "Initialize Session"
3. Get the Analytics Reporter using "Get Analytics Reporter"
4. Create events using "Create Event"
5. Add data to events using flexible data functions
6. Collect events using "Collect Event"
7. Export analytics using "Export Analytics To File"

## Architecture Overview

```mermaid
graph TB
    subgraph "Game Systems"
        PS[Pooling System]
        GS[Game Systems]
        AS[AI Systems]
        RS[Rendering Systems]
        CS[Combat Systems]
        IS[Inventory Systems]
    end
    
    subgraph "GWIZ Central Metrics Reporter"
        CMR[GWIZCentralMetricsReporter]
        SM[UGWIZSessionManager]
        SD[FGWIZStaticSessionData]
        ED[FGWIZEventData]
        FD[FGWIZFlexibleData]
    end
    
    subgraph "Data Processing"
        RT[Real-time Processing]
        HT[Historical Analysis]
        AL[Analytics Engine]
    end
    
    subgraph "Output Systems"
        CON[Console Output]
        FILE[File Export]
        JSON[JSON Analytics]
        CSV[CSV Reports]
        WEB[Webhooks]
    end
    
    PS --> CMR
    GS --> CMR
    AS --> CMR
    RS --> CMR
    CS --> CMR
    IS --> CMR
    
    CMR --> SM
    SM --> SD
    CMR --> ED
    ED --> FD
    
    ED --> RT
    ED --> HT
    ED --> AL
    
    RT --> CON
    RT --> FILE
    RT --> JSON
    RT --> CSV
    RT --> WEB
```

## Data Structure Philosophy

### Optimized Data Storage
**Why This Approach is Best:**
- **Storage efficiency**: No duplication of static data across events
- **Analytics ready**: Perfect for data science analysis
- **Scalable**: Can handle thousands of events efficiently
- **Flexible**: Easy to join static and dynamic data for analysis

### Flexible Data Structure
**Recommended Approach**: Flexible JSON-like Structure

**Why This Approach is Best:**
- **Maximum flexibility**: Can handle any data complexity
- **Blueprint friendly**: All types are Blueprint-compatible
- **JSON ready**: Easy to serialize to JSON for external systems
- **Readable**: Clear structure that's easy to understand
- **Extensible**: Can add new data types easily

## Related Documentation

### Pooling System Integration
- **[Pooling System Requirements](../pooling-system/Pooling_System_Requirements.md)** - Updated with analytics requirements
- **[Pooling System Design](../pooling-system/Pooling_System_Design.md)** - Updated with analytics integration
- **[Pooling System Implementation Plan](../pooling-system/Pooling_System_Implementation_Plan.md)** - Updated with analytics implementation

### Project Documentation
- **[ToDo List](../pooling-system/ToDo.md)** - Updated with Phase 10 analytics implementation
- **[API Reference](../pooling-system/API.md)** - Pooling system API documentation
- **[Integration Guide](../pooling-system/Integration_Guide.md)** - Pooling system integration guide

## Implementation Status

### Completed
- ✅ **Phase 1-6**: Core pooling system implementation
- ✅ **Phase 7**: Documentation and examples
- ✅ **Phase 8**: Performance optimization

### In Progress
- 🔄 **Phase 10**: Performance testing and centralized analytics system

### Planned
- **Plugin Conversion**: Both systems as separate plugins
- **Analytics Integration**: Comprehensive game analytics implementation

## Plugin Strategy

### Implementation Approach
**Recommended Approach**: Create Both Systems as Plugins Now

**Why This Approach is Best:**
- **Clean separation**: Pooling and analytics are independent systems
- **Reusability**: Both can be used in other projects
- **Distribution**: Can be shared or sold separately
- **Maintenance**: Easier to maintain and update independently

### Plugin Structure
```
Plugins/
├── GWIZPoolingSystem/
│   ├── GWIZPoolingSystem.uplugin
│   ├── Source/GWIZPoolingSystem/
│   ├── Content/Examples/
│   └── Documentation/
└── GWIZCentralMetricsReporter/
    ├── GWIZCentralMetricsReporter.uplugin
    ├── Source/GWIZCentralMetricsReporter/
    ├── Content/Examples/
    └── Documentation/
```

## Getting Help

### Documentation
- Check the [Design Document](GWIZCentralMetricsReporter_Design.md) for technical details
- Review the [Implementation Plan](GWIZCentralMetricsReporter_Implementation_Plan.md) for development phases
- See [Usage Examples](Usage_Examples.md) for practical examples

### Integration
- Review [Plugin Integration](Plugin_Integration.md) for plugin setup
- Check [Performance Testing Guide](Performance_Testing_Guide.md) for benchmarking
- See [API Reference](API_Reference.md) for complete function documentation

### Troubleshooting
- Check the [Integration Guide](../pooling-system/Integration_Guide.md) for common issues
- Review the [Testing Guide](../pooling-system/testing.md) for validation steps
- See the [ToDo List](../pooling-system/ToDo.md) for current implementation status

## Analytics Examples

### Performance Analytics
```cpp
// Performance event
FGWIZEventData PerformanceEvent = SessionManager->CreateEvent("Performance", "PoolingSystem");
PerformanceEvent.Data.SetFloat("FrameTime", 16.67f);
PerformanceEvent.Data.SetFloat("CPUUsage", 45.2f);
PerformanceEvent.Data.SetInt("PoolHitRate", 85);
PerformanceEvent.Data.SetInt64("MemoryUsage", 1024 * 1024 * 100); // 100MB
Reporter->CollectEvent(PerformanceEvent);
```

### Gameplay Analytics
```cpp
// Player behavior event
FGWIZEventData BehaviorEvent = SessionManager->CreateEvent("Gameplay", "PlayerSystem");
BehaviorEvent.Data.SetString("Action", "AbilityUsed");
BehaviorEvent.Data.SetString("AbilityName", "Fireball");
BehaviorEvent.Data.SetVector("PlayerPosition", PlayerLocation);
BehaviorEvent.Data.SetFloat("AbilityDamage", 150.0f);

// Nested data for complex events
FGWIZFlexibleData TargetData;
TargetData.SetString("TargetType", "Enemy");
TargetData.SetFloat("TargetHealth", 200.0f);
TargetData.SetVector("TargetPosition", EnemyLocation);
BehaviorEvent.Data.SetNested("Target", TargetData);

Reporter->CollectEvent(BehaviorEvent);
```

### Business Intelligence
```cpp
// Monetization event
FGWIZEventData MonetizationEvent = SessionManager->CreateEvent("Business", "StoreSystem");
MonetizationEvent.Data.SetString("Action", "Purchase");
MonetizationEvent.Data.SetString("ItemName", "PremiumSkin");
MonetizationEvent.Data.SetFloat("Price", 9.99f);
MonetizationEvent.Data.SetString("Currency", "USD");
MonetizationEvent.Data.SetString("Platform", "Steam");
Reporter->CollectEvent(MonetizationEvent);
```

## Data Export Examples

### JSON Export
```json
{
  "EventID": "1705123456-Windows-DEF456",
  "EventType": "Gameplay",
  "Timestamp": 1705123456.789,
  "SystemName": "CombatSystem",
  "SessionID": "SESSION-1705123456-ABC123",
  "SessionDuration": 1250.5,
  "LevelName": "Level_01",
  "GameMode": "SinglePlayer",
  "GameTime": 1250.5,
  "Data": {
    "Action": "AbilityUsed",
    "AbilityName": "Fireball",
    "PlayerPosition": {"X": 100.0, "Y": 200.0, "Z": 50.0},
    "AbilityDamage": 150.0,
    "Target": {
      "TargetType": "Enemy",
      "TargetHealth": 200.0,
      "TargetPosition": {"X": 150.0, "Y": 250.0, "Z": 50.0}
    }
  }
}
```

### CSV Export
```csv
EventID,EventType,Timestamp,SystemName,SessionID,SessionDuration,LevelName,GameMode,GameTime,Action,AbilityName,AbilityDamage
1705123456-Windows-DEF456,Gameplay,1705123456.789,CombatSystem,SESSION-1705123456-ABC123,1250.5,Level_01,SinglePlayer,1250.5,AbilityUsed,Fireball,150.0
```

## External Server Integration

### PostgreSQL Integration
```cpp
// Configure export to PostgreSQL
FGWIZExportConfig Config;
Config.bEnableHTTPExport = true;
Config.HTTPEndpoint = TEXT("https://your-api-server.com/analytics");
Config.HTTPHeaders.Add(TEXT("Content-Type"), TEXT("application/json"));
Config.HTTPHeaders.Add(TEXT("Authorization"), TEXT("Bearer your-api-key"));
Config.BatchSize = 50;
Config.ExportInterval = 10.0f;
Config.bEnableOfflineCaching = true;
Config.MaxCachedEvents = 500;

// Enable PostgreSQL export
UGWIZCentralMetricsReporter* Reporter = UGWIZCentralMetricsReporter::GetMetricsReporter();
Reporter->ConfigureExport(Config);
Reporter->ExportAnalyticsToPostgreSQL(TEXT("postgresql://user:pass@localhost:5432/game_analytics"));
```

### Firebase Integration
```cpp
// Configure export to Firebase
FGWIZExportConfig Config;
Config.bEnableHTTPExport = true;
Config.HTTPEndpoint = TEXT("https://your-project.firebaseio.com/analytics.json");
Config.HTTPHeaders.Add(TEXT("Content-Type"), TEXT("application/json"));
Config.BatchSize = 100;
Config.ExportInterval = 5.0f;
Config.bEnableOfflineCaching = true;
Config.MaxCachedEvents = 1000;

// Enable Firebase export
UGWIZCentralMetricsReporter* Reporter = UGWIZCentralMetricsReporter::GetMetricsReporter();
Reporter->ConfigureExport(Config);
Reporter->ExportAnalyticsToFirebase(TEXT("your-project-id"), TEXT("your-api-key"));
```

### Custom API Integration
```cpp
// Configure export to custom API
FGWIZExportConfig Config;
Config.bEnableHTTPExport = true;
Config.HTTPEndpoint = TEXT("https://your-custom-api.com/analytics");
Config.HTTPHeaders.Add(TEXT("Content-Type"), TEXT("application/json"));
Config.HTTPHeaders.Add(TEXT("X-API-Key"), TEXT("your-api-key"));
Config.BatchSize = 75;
Config.ExportInterval = 15.0f;
Config.bEnableOfflineCaching = true;
Config.MaxCachedEvents = 750;

// Enable custom API export
UGWIZCentralMetricsReporter* Reporter = UGWIZCentralMetricsReporter::GetMetricsReporter();
Reporter->ConfigureExport(Config);
Reporter->ExportAnalyticsToCustomAPI(TEXT("https://your-custom-api.com/analytics"), TEXT("your-api-key"));
```
