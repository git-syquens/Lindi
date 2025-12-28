# Known Error Behaviors and Troubleshooting

## MPU6050 Intermittent Startup Failures

### Symptom
- Device boots with I2C timeout errors (ESP_ERR_TIMEOUT)
- MPU6050 WHO_AM_I register reads fail
- Behavior is intermittent - sometimes works, sometimes fails
- May work briefly then fail after a few seconds
- Both development board and reference board exhibit same issue simultaneously

### Root Cause: Parasitic Power via I2C Lines

**Explanation:**
When the VCC connection to the MPU6050 is loose or intermittent, the sensor can be temporarily powered through the I2C signal lines (SCL/SDA) rather than the dedicated power pin.

**Mechanism:**
1. I2C lines have pull-up resistors (typically 4.7kΩ external + internal ESP32 pull-ups)
2. When pulled high, these lines can source small amounts of current
3. If VCC is disconnected but SCL/SDA are connected, current flows: `ESP32 GPIO → Pull-up → SCL/SDA → MPU6050 internal protection diodes → VCC rail`
4. This parasitic current charges capacitors on the MPU6050 board
5. The sensor appears to respond briefly as capacitors provide temporary power
6. As capacitors discharge, the sensor becomes unstable and eventually fails
7. The charge/discharge cycle creates intermittent behavior

**Why It's Deceptive:**
- The sensor may respond to initial I2C transactions (WHO_AM_I reads)
- Appears to work during boot, then fails during operation
- Power LED on MPU board may glow dimly, suggesting "adequate" power
- Issue can affect multiple boards simultaneously if same connector/wiring issue exists

### Verification Steps

1. **Visual Inspection:**
   - Check VCC solder joints on MPU6050 board
   - Inspect Dupont connector crimps for loose contacts
   - Verify breadboard connections are fully seated

2. **Voltage Measurement:**
   - Measure VCC at MPU6050 with multimeter
   - Should read stable 3.3V
   - Any voltage drop or fluctuation indicates power issue

3. **Connection Test:**
   - Gently wiggle VCC wire while device is running
   - If MPU stops responding, VCC connection is faulty

4. **Power-Off Test:**
   - Disconnect VCC deliberately
   - If device still shows any MPU activity (even briefly), confirms parasitic power

### Solution

**Immediate Fix:**
- Re-solder VCC connection on MPU6050 board
- Replace faulty Dupont connectors
- Use soldered wire connections instead of breadboard for critical power lines

**Preventive Measures:**
- Use twist-on wire connectors or crimped terminals for permanent installations
- Add 10µF capacitor close to MPU6050 VCC pin for power stability
- Verify all power connections with continuity tester before assembly
- Document connector types and replacement intervals in maintenance log

**Hardware Design Improvement:**
- Consider adding power monitoring circuit
- Use screw terminals for external sensor connections
- Add power indicator LED dedicated to sensor rail

### Related Issues

- **Date Observed:** December 28, 2025
- **Session Log:** sessions/2025-12-28.md
- **Git Commits:** 5906763 (stable baseline before issue)
- **Affected Hardware:** Both development board and reference board
- **Secondary Factor:** 4+ second boot delay from i2c_scan() debug code masked the real issue initially

### Lesson Learned

**Never assume software when hardware fails consistently:**
- If multiple identical boards fail simultaneously, suspect common hardware cause
- Power issues can masquerade as I2C communication failures
- Parasitic powering is a real phenomenon with I2C devices
- Always verify VCC before debugging I2C protocol issues
- Remove debug code (like i2c_scan()) promptly to avoid obscuring real problems

---

## Future Error Patterns (To Be Documented)

*This section will be updated as new error patterns are identified and resolved.*
