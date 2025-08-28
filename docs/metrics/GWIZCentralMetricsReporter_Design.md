# GWIZ Central Metrics Reporter - Technical Design

## Overview

The GWIZ Central Metrics Reporter is a comprehensive **game analytics and performance monitoring system** designed to provide unified data collection capabilities across all game systems. It serves as a centralized hub for collecting, processing, and exporting performance data, game analytics, player behavior metrics, and operational statistics.

**Key Design Philosophy**: This system is designed as a **data science solution** that enables comprehensive analytics beyond just performance metrics, including player behavior analysis, game balance insights, and business intelligence.

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

## Core Components

### 1. UGWIZSessionManager Class

**Purpose**: Manages static session data and provides centralized event creation to avoid data duplication.

```cpp
UCLASS(BlueprintType, Blueprintable)
class PLANETDEFENDER_API UGWIZSessionManager : public UObject
{
    GENERATED_BODY()
    
public:
    // Singleton access
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GWIZ Session")
    static UGWIZSessionManager* GetSessionManager();
    
    // Static session data (collected once per session)
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Session")
    FGWIZStaticSessionData StaticData;
    
    // Session management
    UFUNCTION(BlueprintCallable, Category = "GWIZ Session")
    void InitializeSession();
    
    UFUNCTION(BlueprintCallable, Category = "GWIZ Session")
    void UpdateSessionData();
    
    UFUNCTION(BlueprintCallable, Category = "GWIZ Session")
    FGWIZEventData CreateEvent(const FString& EventType, const FString& SystemName);
    
    // Utility functions
    UFUNCTION(BlueprintCallable, Category = "GWIZ Session")
    float GetSessionDuration() const;
    
    UFUNCTION(BlueprintCallable, Category = "GWIZ Session")
    FString GetSessionID() const { return StaticData.SessionID; }
    
protected:
    virtual void BeginDestroy() override;
    
private:
    // Timer for session updates
    FTimerHandle SessionUpdateTimer;
    
    // Session start time
    float SessionStartTime;
};
```

### 2. FGWIZStaticSessionData Structure

**Purpose**: Stores data that remains constant throughout a session to avoid duplication in every event.

```cpp
USTRUCT(BlueprintType)
struct PLANETDEFENDER_API FGWIZStaticSessionData
{
    GENERATED_BODY()
    
    // Platform information (static)
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Static Session Data")
    FString BuildVersion;
    
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Static Session Data")
    FString PlatformName;
    
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Static Session Data")
    FString DistributionPlatform;
    
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Static Session Data")
    FString CPUModel;
    
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Static Session Data")
    FString GPUModel;
    
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Static Session Data")
    FString EngineVersion;
    
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Static Session Data")
    FString ProjectName;
    
    // Player information (static for session)
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Static Session Data")
    FString PlayerID;
    
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Static Session Data")
    FString PlayerName;
    
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Static Session Data")
    FString PlatformPlayerID;
    
    // Session identification (static)
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Static Session Data")
    FString SessionID;
    
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Static Session Data")
    float SessionStartTime;
    
    // Utility functions
    UFUNCTION(BlueprintCallable, Category = "GWIZ Static Session Data")
    void InitializeStaticData();
    
    UFUNCTION(BlueprintCallable, Category = "GWIZ Static Session Data")
    FString ToJSON() const;
};
```

### 3. FGWIZEventData Structure

**Purpose**: Contains only dynamic data that changes with each event, referencing static data via SessionID.

```cpp
USTRUCT(BlueprintType)
struct PLANETDEFENDER_API FGWIZEventData
{
    GENERATED_BODY()
    
    // Event identification
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Event Data")
    FString EventID;
    
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Event Data")
    FString EventType;
    
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Event Data")
    float Timestamp;
    
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Event Data")
    FString SystemName;
    
    // Session reference (links to static data)
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Event Data")
    FString SessionID;
    
    // Dynamic session data (changes during session)
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Event Data")
    float SessionDuration;
    
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Event Data")
    FString LevelName;
    
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Event Data")
    FString GameMode;
    
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Event Data")
    float GameTime;
    
    // Event-specific data
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Event Data")
    FGWIZFlexibleData Data;
    
    // Utility functions
    UFUNCTION(BlueprintCallable, Category = "GWIZ Event Data")
    void InitializeEvent(const FString& InEventType, const FString& InSystemName);
    
    UFUNCTION(BlueprintCallable, Category = "GWIZ Event Data")
    FString ToJSON() const;
};
```

### 4. FGWIZFlexibleData Structure

**Purpose**: Provides maximum flexibility for storing any type of game data while maintaining readability and JSON compatibility.

**Recommended Approach**: Flexible JSON-like Structure

**Why This Approach is Best:**
- **Maximum flexibility**: Can handle any data complexity
- **Blueprint friendly**: All types are Blueprint-compatible
- **JSON ready**: Easy to serialize to JSON for external systems
- **Readable**: Clear structure that's easy to understand
- **Extensible**: Can add new data types easily

```cpp
USTRUCT(BlueprintType)
struct PLANETDEFENDER_API FGWIZFlexibleData
{
    GENERATED_BODY()
    
    // Simple key-value pairs
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Flexible Data")
    TMap<FString, FString> StringData;
    
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Flexible Data")
    TMap<FString, float> FloatData;
    
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Flexible Data")
    TMap<FString, int32> IntData;
    
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Flexible Data")
    TMap<FString, bool> BoolData;
    
    // Arrays of values
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Flexible Data")
    TMap<FString, TArray<FString>> StringArrayData;
    
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Flexible Data")
    TMap<FString, TArray<float>> FloatArrayData;
    
    // Nested structures
    UPROPERTY(BlueprintReadOnly, Category = "GWIZ Flexible Data")
    TMap<FString, FGWIZFlexibleData> NestedData;
    
    // Utility functions
    UFUNCTION(BlueprintCallable, Category = "GWIZ Flexible Data")
    void SetString(const FString& Key, const FString& Value);
    
    UFUNCTION(BlueprintCallable, Category = "GWIZ Flexible Data")
    void SetFloat(const FString& Key, float Value);
    
    UFUNCTION(BlueprintCallable, Category = "GWIZ Flexible Data")
    void SetVector(const FString& Key, const FVector& Value);
    
    UFUNCTION(BlueprintCallable, Category = "GWIZ Flexible Data")
    void SetArray(const FString& Key, const TArray<FString>& Values);
    
    UFUNCTION(BlueprintCallable, Category = "GWIZ Flexible Data")
    void SetNested(const FString& Key, const FGWIZFlexibleData& Data);
    
    UFUNCTION(BlueprintCallable, Category = "GWIZ Flexible Data")
    FString ToJSON() const;
};
```

### 5. GWIZCentralMetricsReporter Class

**Purpose**: Main interface for collecting and processing analytics data from all game systems.

```cpp
UCLASS(BlueprintType, Blueprintable)
class PLANETDEFENDER_API UGWIZCentralMetricsReporter : public UObject
{
    GENERATED_BODY()
    
public:
    // Singleton access
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GWIZ Analytics")
    static UGWIZCentralMetricsReporter* GetMetricsReporter();
    
    // Event collection
    UFUNCTION(BlueprintCallable, Category = "GWIZ Analytics")
    void CollectEvent(const FGWIZEventData& Event);
    
    // Performance testing
    UFUNCTION(BlueprintCallable, Category = "GWIZ Analytics")
    void PerformanceTest(TSubclassOf<AActor> ActorClass, int32 SpawnCount, 
                        float Lifetime, bool bUsePooling, 
                        const FGWIZPerformanceTestConfig& Config);
    
    // Reporting functions
    UFUNCTION(BlueprintCallable, Category = "GWIZ Analytics")
    void PrintAnalyticsReport(const FString& SystemName = TEXT(""));
    
    UFUNCTION(BlueprintCallable, Category = "GWIZ Analytics")
    void ExportAnalyticsToFile(const FString& FilePath, const FString& Format = TEXT("JSON"));
    
    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GWIZ Analytics")
    bool bEnableRealTimeMonitoring = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GWIZ Analytics")
    bool bEnableHistoricalData = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GWIZ Analytics")
    float AnalyticsCollectionInterval = 1.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GWIZ Analytics")
    int32 MaxHistoricalEntries = 1000;
    
protected:
    virtual void BeginDestroy() override;
    
private:
    // Event storage
    UPROPERTY()
    TArray<FGWIZEventData> HistoricalEvents;
    
    // Thread safety
    FCriticalSection AnalyticsMutex;
    
    // Timers
    FTimerHandle AnalyticsCollectionTimer;
    FTimerHandle HistoricalCleanupTimer;
};
```

## Data Flow

### 1. Session Initialization Flow

```mermaid
sequenceDiagram
    participant Game as Game Startup
    participant SM as Session Manager
    participant SD as Static Data
    participant CMR as Central Reporter
    
    Game->>SM: InitializeSession()
    SM->>SD: InitializeStaticData()
    SD->>SD: Collect platform info
    SD->>SD: Collect player info
    SD->>SD: Generate session ID
    SM->>CMR: Register session
    CMR->>CMR: Start monitoring
```

### 2. Event Collection Flow

```mermaid
sequenceDiagram
    participant GS as Game System
    participant SM as Session Manager
    participant ED as Event Data
    participant CMR as Central Reporter
    
    GS->>SM: CreateEvent(EventType, SystemName)
    SM->>ED: Initialize with dynamic data
    GS->>ED: Add event-specific data
    GS->>CMR: CollectEvent(EventData)
    CMR->>CMR: Store event
    CMR->>CMR: Process analytics
```

### 3. Analytics Processing Flow

```mermaid
sequenceDiagram
    participant CMR as Central Reporter
    participant RT as Real-time Processing
    participant HT as Historical Analysis
    participant AL as Analytics Engine
    participant OUT as Output Systems
    
    CMR->>RT: Process new event
    RT->>AL: Update analytics
    CMR->>HT: Store for analysis
    HT->>AL: Historical trends
    AL->>OUT: Generate reports
    AL->>OUT: Export data
```

## Analytics Categories

### 1. Performance Analytics
- **Frame rate analysis**: Performance trends and bottlenecks
- **Memory usage**: Memory leak detection and optimization
- **Load times**: Level loading and asset streaming performance
- **System performance**: CPU, GPU, and network utilization

### 2. Gameplay Analytics
- **Player behavior**: Movement patterns, ability usage, decision making
- **Game balance**: Win/loss rates, difficulty progression, item usage
- **Engagement metrics**: Session duration, retention, progression
- **Social features**: Multiplayer interactions, communication patterns

### 3. Business Intelligence
- **Monetization**: Purchase patterns, conversion rates, revenue analysis
- **User acquisition**: Marketing funnel analysis, platform performance
- **Retention**: Churn analysis, re-engagement patterns
- **Content performance**: Level completion rates, feature adoption

### 4. Technical Analytics
- **Error tracking**: Crash analysis, bug frequency, error patterns
- **Platform performance**: Cross-platform comparison, optimization opportunities
- **Build analysis**: Version performance, regression detection
- **Infrastructure**: Server performance, network latency, scalability

## Integration with Existing Systems

### Pooling System Integration

The Central Metrics Reporter integrates with the existing GWIZ Pooling System by:

1. **Replacing existing debug output**: `PrintDebugInfo()` and `PrintAllPoolStatistics()` now use the centralized reporter
2. **Enhanced analytics collection**: Pooling statistics are collected as comprehensive analytics events
3. **Performance testing**: Direct integration with pooling system for comparison testing
4. **Real-time monitoring**: Live pooling system analytics in the centralized dashboard

### Test Framework Integration

Integration with the existing test framework:

1. **Enhanced FSimplePoolingTests**: Analytics benchmarks added to existing tests
2. **Automated testing**: Analytics tests can be run as part of the test suite
3. **Result comparison**: Historical analytics data for regression testing
4. **Continuous integration**: Analytics metrics for CI/CD pipelines

## Data Export Strategy

### Referenced Data Approach (Recommended)

**Why This Approach is Best:**
- **Storage efficiency**: No duplication of static data
- **Analytics ready**: Perfect for data science analysis
- **Scalable**: Can handle thousands of events efficiently
- **Flexible**: Easy to join static and dynamic data for analysis

### JSON Output Example

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

## Performance Considerations

### Memory Management

- **Efficient storage**: Historical data is stored in a ring buffer to prevent memory bloat
- **Configurable limits**: Maximum historical entries can be configured
- **Automatic cleanup**: Old data is automatically removed based on configuration
- **Memory pooling**: Reuse of analytics structures to reduce allocations

### Thread Safety

- **Critical sections**: All shared data access is protected by mutexes
- **Lock-free operations**: Where possible, use atomic operations for better performance
- **Async processing**: Heavy analytics operations are performed asynchronously
- **Batch processing**: Multiple events are processed in batches to reduce overhead

### Real-time Performance

- **Minimal overhead**: Analytics collection has minimal impact on game performance
- **Configurable intervals**: Collection frequency can be adjusted based on needs
- **Efficient data structures**: Optimized data structures for fast access and modification
- **Background processing**: Heavy analysis is performed in background threads

## Blueprint Integration

### Available Blueprint Functions

- **Get Metrics Reporter**: Access the singleton instance
- **Get Session Manager**: Access session management
- **Create Event**: Create new analytics events
- **Collect Event**: Add events to the analytics system
- **Print Report**: Generate and display analytics reports
- **Export Data**: Export analytics to various formats

### Blueprint Events

- **On Analytics Updated**: Fired when new analytics are collected
- **On Performance Test Complete**: Fired when performance tests finish
- **On Alert Triggered**: Fired when analytics thresholds are exceeded
- **On Session Started**: Fired when a new session begins

## Configuration and Setup

### Default Configuration

The system comes with sensible defaults that can be overridden:

```cpp
// Default configuration
UGWIZCentralMetricsReporter::UGWIZCentralMetricsReporter()
{
    bEnableRealTimeMonitoring = true;
    bEnableHistoricalData = true;
    AnalyticsCollectionInterval = 1.0f;
    MaxHistoricalEntries = 1000;
}
```

### Runtime Configuration

Configuration can be changed at runtime through Blueprint or C++:

```cpp
// Runtime configuration example
UGWIZCentralMetricsReporter* Reporter = UGWIZCentralMetricsReporter::GetMetricsReporter();
Reporter->bEnableRealTimeMonitoring = true;
Reporter->AnalyticsCollectionInterval = 0.5f;
Reporter->MaxHistoricalEntries = 500;
```

## Future Extensibility

### Plugin Architecture

The system is designed for easy extension:

1. **Custom analytics types**: New analytics types can be added through the event system
2. **External system plugins**: New external systems can be added through the plugin architecture
3. **Custom report formats**: New output formats can be implemented
4. **Advanced analytics**: Machine learning and predictive analytics can be added

### API Stability

The core API is designed to remain stable while allowing for extensions:

1. **Backward compatibility**: Existing code will continue to work
2. **Versioned APIs**: New features are added without breaking existing functionality
3. **Deprecation warnings**: Old APIs are marked as deprecated before removal
4. **Migration guides**: Clear migration paths for API changes
