# Lindi Level - Quick Reference Guide

**© Syquens B.V. 2025**

---

## 📱 Interface Overview

The device has **3 main tabs** you can switch between by tapping:

| Tab | Purpose | Key Features |
|-----|---------|--------------|
| **Start** | Clock display | Analog/Digital clock, tap to switch modes |
| **Level** | Spirit level | Pitch (forward/back) and Roll (left/right) measurements |
| **Info** | Settings & status | WiFi status, timezone, theme, language, version info |

---

## 🕐 Using the Clock (Start Tab)

**Switch Clock Mode:**
- Tap anywhere on the clock display
- Toggles between **Analog** (round clock face) and **Digital** (HH:MM:SS)
- Your preference is saved automatically

**Time Synchronization:**
- Automatic via WiFi and NTP servers
- Updates within seconds of WiFi connection
- Check WiFi status in Info tab if time is incorrect

---

## 📏 Using the Spirit Level (Level Tab)

**Reading the Display:**

```
        Pitch (Vertical Bar)
             ▲
        Backward tilt
             |
    -------- 0° --------  ← Level
             |
        Forward tilt
             ▼

    Roll (Horizontal Bar)
    Left ← --0°-- → Right
```

**Measurement Tips:**
1. Place device flat on surface
2. Wait 1-2 seconds for stabilization
3. Both bars centered = Surface is level
4. Numbers show exact tilt in degrees (±0.1°)

**Measurement Range:** ±30° (recommended ±10° for best accuracy)

### Calibration (Left Side Buttons)

**When to Calibrate:**
- Sensor cannot be mounted perfectly level
- Need to compensate for fixed mounting angle

**How to Calibrate:**
1. Level RV manually (use jacks/blocks + external level)
2. Wait for readings to stabilize
3. Press **Calibrate** button (left side)
4. Confirm when prompted
5. Current position is now zero reference

**Reset Calibration:**
- Press **Reset** button (below Calibrate)
- Confirm (previous offset will be lost)
- Returns to raw sensor readings

⚠️ **Important:** Always level RV manually BEFORE calibrating!

---

## ⚙️ Settings (Info Tab)

### Essential Settings

| Setting | Options | Description |
|---------|---------|-------------|
| **Timezone** | GMT-12 to GMT+12 | Your local time offset (default: GMT+1) |
| **Winter Time** | ON/OFF | Enable for daylight saving time |
| **Language** | EN/NL | English or Dutch (labels update immediately, tab names need restart) |
| **Accent Color** | 16 colors | Choose UI accent color from color picker (changes apply immediately) |
| **Invert Level** | ON/OFF | Reverse tilt direction if sensor mounted backward |
| **Dark Theme** | ON/OFF | Dark display for low-light environments |

### Quick Settings Guide

**To change timezone:**
1. Go to Info tab
2. Tap the Timezone dropdown
3. Select your GMT offset
4. Time updates immediately

**To change language:**
1. Go to Info tab
2. Toggle the EN/NL switch
3. Setting labels update immediately
4. For tab names, restart device (disconnect/reconnect power)

**To change language:**
1. Go to Info tab
2. Toggle Language switch (OFF=English, ON=Dutch)
3. **Restart the device** for changes to apply

**If level readings are reversed:**
1. Go to Info tab
2. Toggle "Invert Level" switch ON
3. Changes apply immediately

---

## 🔧 Troubleshooting

| Problem | Solution |
|---------|----------|
| ⏰ **Clock not updating** | Check WiFi in Info tab, wait 60 seconds, restart if needed |
| 📏 **Level readings wrong** | Toggle "Invert Level" setting in Info tab |
| 📡 **WiFi won't connect** | Check 2.4GHz network, wait for 5 retry attempts (~60 sec) |
| 🖐️ **Touch not responding** | Tap firmly, clean screen, restart device |
| 🌐 **Language won't change** | **Must restart device** after toggling language switch |

---

## 🎯 Quick Start Steps

1. **Power on** the device (USB-C or 5V supply)
2. **Wait 5-10 seconds** for boot and WiFi connection
3. **Set timezone** in Info tab (default GMT+1)
4. **Switch to Level tab** to use spirit level
5. **Tap clock** in Start tab to change display mode

---

## 📊 Technical Quick Facts

- **Display:** 3.2" touchscreen (320×240)
- **Sensor:** MPU6050 6-axis (±30° range, ±0.1° precision)
- **WiFi:** 2.4GHz only, WPA2 supported
- **Update Rate:** Level 10Hz, Clock 1Hz
- **Power:** 5V USB-C, 200-400mA
- **Storage:** All settings auto-saved to flash memory

---

## 💡 Pro Tips

✓ **Clock mode is saved** - Your analog/digital preference persists  
✓ **All settings auto-save** - No save button needed  
✓ **Level updates 10x/sec** - Real-time feedback for precise adjustments  
✓ **Dark theme reduces glare** - Good for low-light work environments  
✓ **Invert Level for mounting** - Flip sensor readings if device is backward  
✓ **Language needs restart** - Only setting that requires power cycle  

---

## 📞 Support

**Syquens B.V. 2025**  
Author: V.N. Verbon

For detailed information, see full user manual:
- `documentation/manual_EN.md` (English)
- `documentation/manual_NL.md` (Nederlands)

---

**Quick Reference v1.0**
