# GWIZ Central Metrics Reporter - Testing Guide

## Overview

This guide will help you test the GWIZ Central Metrics Reporter plugin in the Unreal Editor to ensure it's working correctly.

## Prerequisites

- ✅ Plugin compiles successfully (already verified)
- ✅ Project opens in Unreal Editor
- ✅ Plugin is enabled in the project

## Step 1: Verify Plugin Loading

### **1.1 Check Plugin Status**
1. Open your project in Unreal Editor
2. Go to **Edit → Plugins**
3. Search for "GWIZ Central Metrics Reporter"
4. Verify the plugin is **Enabled** and shows no errors

### **1.2 Check for Compilation Errors**
1. Open **Window → Developer Tools → Output Log**
2. Look for any error messages related to the plugin
3. If you see errors, check the **Compilation Results** section below

## Step 2: Test Basic Functionality

### **2.1 Create a Test Blueprint**
1. In the Content Browser, right-click and select **Blueprint Class**
2. Choose **Actor** as the parent class
3. Name it `BP_AnalyticsTester`
4. Double-click to open the Blueprint editor

### **2.2 Test Session Management**
1. In the Blueprint editor, add an **Event BeginPlay** node
2. Add a **Get Session Manager** node (search for "Get Session Manager")
3. Connect it to a **Initialize Session** node
4. Compile the Blueprint

**Expected Result**: Blueprint compiles without errors

### **2.3 Test Analytics Collection**
1. Add a **Get Analytics Reporter** node (search for "Get Analytics Reporter")
2. Add a **Create Event** node (search for "Create Event")
3. Set Event Type to "Test" and System Name to "Testing"
4. Connect to a **Collect Event** node
5. Compile the Blueprint

**Expected Result**: Blueprint compiles without errors

## Step 3: Test in Game

### **3.1 Place Test Actor**
1. Drag `BP_AnalyticsTester` into your level
2. Save the level
3. Click **Play** to start the game

### **3.2 Check Output Log**
1. While the game is running, open **Window → Developer Tools → Output Log**
2. Look for messages like:
   ```
   LogTemp: Log: Analytics session initialized: Session_YYYYMMDD_HHMMSS_XXXXXXXX
   LogTemp: Log: Collected analytics event: Test from Testing
   ```

**Expected Result**: You should see session initialization and event collection messages

## Step 4: Test Data Export

### **4.1 Trigger Export**
1. While the game is running, press **Tab** to open the console
2. Type: `ce ExportEvents` and press Enter
3. Check the Output Log for export messages

**Expected Result**: You should see:
```
LogTemp: Log: Exported X events to file: [path]/Analytics_YYYYMMDD_HHMMSS.json
```

### **4.2 Check Export Files**
1. Navigate to your project's `Saved/Analytics/` folder
2. Look for JSON files with names like `Analytics_YYYYMMDD_HHMMSS.json`
3. Open one of the files to verify the data structure

**Expected Result**: JSON files should contain analytics data in this format:
```json
{
  "EventID": "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX",
  "EventType": "Test",
  "SystemName": "Testing",
  "Timestamp": "YYYY-MM-DD HH:MM:SS",
  "SessionID": "Session_YYYYMMDD_HHMMSS_XXXXXXXX",
  "Platform": "Windows",
  "EngineVersion": "5.4.0-0+++UE5+Release-5.4-CL-0",
  "Data": "Event data serialization not yet implemented"
}
```

## Step 5: Test Performance Testing

### **5.1 Start Performance Test**
1. In the console, type: `ce StartPerformanceTest TestName=PoolingTest ActorCount=100 bUsePooling=true`
2. Check the Output Log for test start messages

**Expected Result**: You should see:
```
LogTemp: Log: Performance test started: PoolingTest
```

### **5.2 End Performance Test**
1. In the console, type: `ce EndPerformanceTest`
2. Check the Output Log for test results

**Expected Result**: You should see test completion messages with performance data

## Troubleshooting

### **Common Issues**

#### **1. Plugin Not Loading**
- **Symptom**: Plugin shows as disabled or has errors
- **Solution**: Check that the plugin compiled successfully and restart the editor

#### **2. Blueprint Functions Not Found**
- **Symptom**: Can't find "Get Session Manager" or "Get Analytics Reporter"
- **Solution**: Verify the plugin is enabled and restart the Blueprint editor

#### **3. Compilation Errors**
- **Symptom**: Blueprint won't compile
- **Solution**: Check the Output Log for specific error messages

#### **4. No Export Files**
- **Symptom**: No files in `Saved/Analytics/` folder
- **Solution**: Verify the export path is correct and check file permissions

### **Debug Commands**

Use these console commands to debug the plugin:

```bash
# Check plugin status
ce GetStatus

# Check session status
ce GetSessionStatus

# Check collected events count
ce GetEventCount

# Force export
ce ExportEvents

# Start performance test
ce StartPerformanceTest TestName=DebugTest ActorCount=10

# End performance test
ce EndPerformanceTest
```

## Expected Test Results

### **✅ Success Indicators**
- Plugin loads without errors
- Blueprint functions are available
- Session initializes successfully
- Events are collected
- Data exports to JSON files
- Performance tests run and complete

### **❌ Failure Indicators**
- Plugin shows compilation errors
- Blueprint functions are missing
- Console commands return errors
- No export files are created
- Performance tests fail to start/complete

## Next Steps After Testing

If all tests pass:

1. **Integration Testing**: Test with your existing pooling system
2. **Performance Validation**: Run larger performance tests
3. **Data Analysis**: Examine exported JSON data for insights
4. **Feature Development**: Continue with remaining implementation tasks

If tests fail:

1. **Check Compilation**: Ensure the plugin compiled successfully
2. **Review Logs**: Check Output Log for specific error messages
3. **Verify Dependencies**: Ensure all required modules are available
4. **Restart Editor**: Sometimes a restart resolves loading issues

## Support

If you encounter issues during testing:

1. **Check the Output Log** for specific error messages
2. **Verify the plugin compiled** successfully
3. **Check the implementation plan** for current status
4. **Review the troubleshooting section** above

The plugin is designed to be robust and provide clear error messages when something goes wrong.
