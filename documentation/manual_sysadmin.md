# Lindi System Administrator Manual

## MQTT Command System

### Overview
Lindi supports remote command execution via MQTT. Commands are sent to the device's command topic and executed once based on a unique command identifier.

### Command Structure

#### Incoming Command Topic
```
{basetopic}/device/command
```

For default configuration: `lindi/device/command`

#### Command Payload Format
Commands must be sent as JSON with the following structure:

```json
{
  "command_id": "unique-identifier-string",
  "command": "command_name",
  "parameters": {}
}
```

**Fields:**
- `command_id` (required): Unique identifier for this command instance
  - Must be a string, max 32 characters recommended
  - Examples: UUID, timestamp-based ID, sequential number
  - Used for duplicate detection and execution tracking
  
- `command` (required): Name of the command to execute
  - Must match a whitelisted command name
  - Case-sensitive
  
- `parameters` (optional): Object containing command-specific parameters
  - Structure varies by command
  - Can be empty object `{}` if command requires no parameters

#### Example Command
```json
{
  "command_id": "cmd-2025-12-28-001",
  "command": "say_time",
  "parameters": {}
}
```

### Command Execution Tracking

#### Duplicate Prevention
The device maintains a persistent record of executed command IDs in NVS (Non-Volatile Storage) to prevent duplicate execution.

**Behavior:**
- When a command is received, the device checks if `command_id` has been executed before
- If `command_id` exists in the execution history, the command is ignored
- If `command_id` is new, the command is validated and executed
- After successful execution, `command_id` is stored in NVS

**Storage Details:**
- Namespace: `mqtt_cmds`
- Key: `command_id` (truncated to 15 characters if longer)
- Value: Execution timestamp (Unix epoch)
- Storage limit: Last 100 command IDs retained (FIFO)

#### Command History Topic
Executed commands are acknowledged on:
```
{basetopic}/device/command_ack
```

**Acknowledgment Payload:**
```json
{
  "command_id": "cmd-2025-12-28-001",
  "command": "say_time",
  "status": "executed",
  "timestamp": 1735382400
}
```

### Whitelisted Commands

Only explicitly configured commands will be executed. All other commands are rejected.

#### say_time
Publishes the device's current time configuration.

**Command:**
```json
{
  "command_id": "unique-id",
  "command": "say_time",
  "parameters": {}
}
```

**Parameters:** None

**Output Topic:**
```
{basetopic}/device/timestamp
```

**Output Payload:**
```json
{
  "timestamp": 1735382400,
  "datetime": "2025-12-28 14:30:00",
  "timezone": "UTC",
  "source": "SNTP"
}
```

**Description:**
- Reports current device time in multiple formats
- Includes time source (SNTP/RTC/Manual)
- Useful for time synchronization verification

### Polling Behavior

The device polls for commands by subscribing to the command topic. To prevent excessive network traffic and power consumption:

- **Polling Interval**: Commands are checked once every 10 seconds
- **Rate Limiting**: No command polling occurs more frequently than this interval
- **Power Efficiency**: Reduces WiFi activity and extends battery life (future battery-powered variants)

**Note**: Commands published to the broker are retained by MQTT QoS, so there is no risk of missing commands during the polling interval.

### Security Considerations

1. **Command Whitelisting**: Only pre-approved commands can execute. Unknown commands are rejected.

2. **ID-Based Execution**: Each command requires a unique identifier, preventing replay attacks and accidental re-execution.

3. **Persistent Tracking**: Command history survives reboots and power cycles.

4. **MQTT Authentication**: Commands are only accepted from authenticated MQTT connections (username/password + TLS).

5. **Rate Limiting**: 10-second polling interval prevents command flooding and resource exhaustion.

### Troubleshooting

#### Command Not Executing
1. Verify JSON format is valid
2. Check `command_id` is unique (not previously executed)
3. Ensure `command` name is in whitelist
4. Verify MQTT connection is established
5. Check device serial logs for error messages

#### Command History Full
If the device reports NVS storage full for commands:
- History is automatically pruned (FIFO, keeps last 100)
- Consider resetting command history if needed
- Check for command spam or automation loops

#### Time Sync Issues
If `say_time` reports incorrect time:
- Verify WiFi connection is stable
- Check SNTP server accessibility
- Ensure timezone is correctly configured
- Review NTP synchronization logs in serial output

### Examples

#### Single Command Execution
```bash
# Publish to broker
mosquitto_pub -h mqtt.syquens.com -p 8883 --capath /etc/ssl/certs \
  -u 23-09edh -P <password> \
  -t lindi/device/command \
  -m '{"command_id":"test-001","command":"say_time","parameters":{}}'
```

#### Monitoring Responses
```bash
# Subscribe to device responses
mosquitto_sub -h mqtt.syquens.com -p 8883 --capath /etc/ssl/certs \
  -u 23-09edh -P <password> \
  -t lindi/device/#
```

### Future Commands (Planned)
- `get_status`: Report device health metrics
- `set_brightness`: Adjust display brightness
- `reboot`: Graceful device restart
- `get_config`: Report current configuration
- `calibrate_touch`: Enter touch screen calibration mode
