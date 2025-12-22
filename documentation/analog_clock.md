# Analog Clock Implementation

## Overview
The analog clock is implemented using LVGL 7's `lv_gauge` widget with 3 needles for hour, minute, and second hands.

## Current Status: COMPLETE ✅ (Dec 22, 2025)

### Final Configuration
- ✅ 360° full circle with 60 tick marks (one per minute position)
- ✅ No labels (cleaner look, easier to read)
- ✅ 3 needles (hour, minute, second) with distinct colors and 1.5x length
- ✅ Perfect hand alignment at all positions (linear 360° mapping)
- ✅ Clock size: 126×126 pixels
- ✅ Non-scrollable Start tab

---

## LVGL 7 Gauge Configuration (FINAL)

### Gauge Setup
```c
analog_clock_gauge = lv_gauge_create(tab_start, NULL);
lv_obj_set_size(analog_clock_gauge, 126, 126);  // 70% of original 180px
lv_obj_align(analog_clock_gauge, clock_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

// Configure gauge: 360° full circle with 60 tick marks
lv_gauge_set_scale(analog_clock_gauge, 360, 60, 0);  // 360° arc, 60 ticks, 0 labels
lv_gauge_set_range(analog_clock_gauge, 0, 59);       // 0-59 range (each unit = 6°)
lv_gauge_set_angle_offset(analog_clock_gauge, 270);  // Value 0 starts at 6 o'clock
```

### Key Parameters Explained

| Parameter | Value | Explanation |
|-----------|-------|-------------|
| Arc angle | 360° | Full circle, perfect linear mapping |
| Tick count | 60 | One tick per minute/second position |
| Label count | 0 | No labels (cleaner appearance) |
| Range | 0-59 | Maps to 60 positions around the dial |
| Angle offset | 270° | Rotates gauge so value 0 starts at 6 o'clock (bottom) |
| Rotation offset | 30 | Shifts needle by +30 units to point at 12 o'clock when value=0 |

### Why These Values?

**360° Arc**: Provides perfect linear mapping where each unit = exactly 6° (360° ÷ 60 = 6°). No distortion or non-linearity.

**60 Tick Marks**: One tick per minute position. This ensures:
- Clear reference lines at cardinal positions (12, 3, 6, 9 o'clock)
- Easy verification of hand positions
- Every 5-minute mark aligns with traditional hour positions

**Angle Offset 270°**: LVGL's gauge widget has value 0 at 9 o'clock by default (270° in standard coordinates). Adding 270° offset rotates everything so value 0 is at 6 o'clock (bottom).

**Rotation Offset 30**: Since angle_offset puts value 0 at 6 o'clock, we need to add 30 units to make hands point at 12 o'clock:
- From 6 o'clock to 12 o'clock = 180°
- 180° ÷ 6° per unit = 30 units

---

## Needle Configuration

### Colors and Styling
```c
static lv_color_t needle_colors[3];
needle_colors[0] = LV_COLOR_MAKE(200, 200, 200);  // Hour hand - light gray
needle_colors[1] = LV_COLOR_MAKE(150, 150, 150);  // Minute hand - darker gray  
needle_colors[2] = LV_COLOR_RED;                   // Second hand - red
lv_gauge_set_needle_count(analog_clock_gauge, 3, needle_colors);

// Make needles 1.5x longer by reducing inner padding
lv_obj_set_style_local_pad_inner(analog_clock_gauge, LV_GAUGE_PART_MAIN, LV_STATE_DEFAULT, 10);
```

| Needle Index | Purpose | Color | RGB |
|--------------|---------|-------|-----|
| 0 | Hour hand | Light gray | (200, 200, 200) |
| 1 | Minute hand | Darker gray | (150, 150, 150) |
| 2 | Second hand | Red | LV_COLOR_RED |

### Needle Value Calculation (FINAL)
Located in `clock_update_task()` in [main/main.c](../main/main.c):

```c
// Rotation offset for 360° arc with angle_offset=270°
// angle_offset=270° puts value 0 at 6 o'clock (bottom)
// Need +30 units to reach 12 o'clock (top): 180° / 6° = 30
const int rotation_offset = 30;

// Second hand: direct mapping (0-59 seconds)
int sec_value = (timeinfo.tm_sec + rotation_offset) % 60;
lv_gauge_set_value(analog_clock_gauge, 2, sec_value);

// Minute hand: direct mapping (0-59 minutes)
int min_value = (timeinfo.tm_min + rotation_offset) % 60;
lv_gauge_set_value(analog_clock_gauge, 1, min_value);

// Hour hand: 5 units per hour + smooth interpolation
// Uses (min * 5 / 60) for smooth movement between hours
int hour_value = ((hour_12 * 5) + (timeinfo.tm_min * 5 / 60) + rotation_offset) % 60;
lv_gauge_set_value(analog_clock_gauge, 0, hour_value);
```

### How It Works

**Second Hand**: Maps directly to seconds (0-59). At 0 seconds, points to 12 o'clock.

**Minute Hand**: Maps directly to minutes (0-59). At 0 minutes, points to 12 o'clock.

**Hour Hand**: 
- Each hour = 5 units (12 hours × 5 = 60 units for full rotation)
- Smooth movement: `(min * 5 / 60)` adds fractional position based on minutes
- At 12:00, points to 12 o'clock
- At 3:00, points to 3 o'clock (15 units + 30 offset = 45)
- At 6:00, points to 6 o'clock (30 units + 30 offset = 60 = 0)

### Perfect Alignment Verification

With 360° and linear mapping, hands align perfectly at all positions:

| Time | Sec Value | Min Value | Hour Value | Visual Check |
|------|-----------|-----------|------------|--------------|
| 12:00:00 | 30 | 30 | 30 | All at 12 o'clock |
| 03:15:30 | 0 | 45 | 45 | Sec at 12, Min & Hour at 3 |
| 06:30:00 | 30 | 0 | 0 | Hour & Sec at 12, Min at 6 |
| 09:45:15 | 45 | 15 | 15 | Sec at 9, Min & Hour at 3 |

---

## Code Locations in main/main.c

| Line (approx) | Content |
|---------------|---------|
| ~502-520 | Gauge creation and configuration |
| ~745-812 | `clock_update_task()` - needle updates every second |

---

## Troubleshooting History

### Dec 21, 2025: Initial Implementation
**Issue**: Labels not aligned, only 11 showing, hour hand position incorrect

**Attempted Solutions**:
- Used 330° arc with 12 labels → Fixed label count but created gap and non-linear distortion
- Calibrated individual offsets for each hand → Helped but hands still off at different positions
- Created custom label formatter → Fixed label numbering

**Root Cause**: 330° arc creates non-linear mapping around the dial, causing hands to be ahead/behind depending on position relative to the 30° gap.

### Dec 22, 2025: Final Solution  
**Solution**: Switched to 360° full circle with 60 tick marks
- Removed all labels for cleaner appearance
- Perfect linear mapping (each unit = exactly 6°)
- Single rotation_offset (30) works for all hands at all positions
- 60 tick marks provide clear reference lines at all positions

**Result**: ✅ Perfect hand alignment verified at cardinal positions (12, 3, 6, 9 o'clock) and all intermediate positions.

---

## Summary

The analog clock is now implemented as a modular, reusable component:
- **Modular Design**: Separate files for clock component logic
- **360° full circle** with 60 tick marks for linear mapping
- **No labels** for cleaner appearance
- **Three colored needles** (gray hour, gray minute, red second) with 1.5x length
- **Single rotation offset** (30 units) for all hands
- **Perfect alignment** at all clock positions
- **NVS persistence** of clock mode preference (analog/digital)
- **Toggle button** for mode switching

All configuration values are empirically calibrated and documented above for future reference.

---

## Modularization

### Architecture (Option A: External Task Management)

The clock component is designed with **external task management** for maximum flexibility:

**Component Responsibility**:
- Create and manage LVGL widgets (analog gauge, digital label, toggle button)
- Handle NVS persistence of clock mode preference
- Toggle between analog and digital modes
- Update display when provided with time data

**Caller Responsibility**:
- Manage LVGL task that calls clock_update() periodically
- Retrieve current time from system or RTC
- Control update frequency (typically 1 second)

This design allows the clock component to be used in different contexts:
- With WiFi + SNTP time synchronization (current implementation)
- With external RTC module
- With manual time setting
- Different update frequencies for power optimization

### Files

**[main/clock_component.h](../main/clock_component.h)** - Public API:
- `clock_config_t` structure for initialization parameters
- `clock_handle_t` opaque handle to clock instance
- `clock_create()` - Create clock component
- `clock_update()` - Update display with provided time
- `clock_toggle_mode()` - Toggle between analog/digital
- `clock_set_mode()` - Programmatically set mode
- `clock_is_digital_mode()` - Query current mode
- `clock_destroy()` - Cleanup and free resources

**[main/clock_component.c](../main/clock_component.c)** - Implementation:
- Internal clock widget management
- NVS persistence using namespace "lindi_cfg", key "digital_mode"
- Toggle button event handling
- Analog and digital clock update logic

**[main/main.c](../main/main.c)** - Integration:
- Creates clock component in guiTask()
- Updates clock every second from LVGL task
- Retrieves time from system time (WiFi + SNTP synchronized)

### Usage Example

```c
#include "clock_component.h"

// Global handle
static clock_handle_t my_clock = NULL;

// In GUI initialization function:
void init_gui(lv_obj_t *parent_tab) {
    clock_config_t clock_cfg = {
        .parent = parent_tab,
        .x_offset = 0,
        .y_offset = 0,
        .start_with_digital = false,  // Default to analog (NVS will override)
        .show_toggle_button = true
    };
    my_clock = clock_create(&clock_cfg);
    if (!my_clock) {
        ESP_LOGE("TAG", "Failed to create clock component");
    }
}

// In periodic update task (e.g., LVGL task, called every 1 second):
void clock_update_task(lv_task_t *task) {
    if (!my_clock) return;
    
    // Get current time from system
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    
    // Update clock component
    clock_update(my_clock, &timeinfo);
}

// Programmatic mode control (optional):
void switch_to_digital() {
    if (my_clock) {
        clock_set_mode(my_clock, true);  // true = digital mode
    }
}

// Query current mode (optional):
void log_current_mode() {
    if (my_clock) {
        bool is_digital = clock_is_digital_mode(my_clock);
        ESP_LOGI("TAG", "Clock is in %s mode", is_digital ? "digital" : "analog");
    }
}

// Cleanup on exit:
void cleanup() {
    if (my_clock) {
        clock_destroy(my_clock);
        my_clock = NULL;
    }
}
```

### Configuration Options

When creating the clock with `clock_create()`, you can customize:

| Parameter | Type | Description | Default |
|-----------|------|-------------|---------|
| `parent` | `lv_obj_t*` | Parent LVGL object | Required |
| `x_offset` | `lv_coord_t` | Horizontal position offset | 0 |
| `y_offset` | `lv_coord_t` | Vertical position offset | 0 |
| `start_with_digital` | `bool` | Initial mode if no NVS data | false (analog) |
| `show_toggle_button` | `bool` | Show mode toggle button | true |

**Note**: The `start_with_digital` parameter is only used if no mode preference is saved in NVS. Once the user toggles the mode, the NVS setting takes precedence.

### Design Tradeoffs

**Why External Task Management (Option A)?**

Pros:
- ✅ Caller controls update frequency (power optimization)
- ✅ Works with any time source (SNTP, RTC, manual)
- ✅ No threading complexity within component
- ✅ Simpler component implementation
- ✅ LVGL thread-safety managed by caller

Cons:
- ❌ Caller must periodically call clock_update()
- ❌ Slightly more integration code in main.c

**Alternative: Internal Task Management (Option B - Not Implemented)**

Would have the component create its own LVGL task internally. Rejected because:
- More complex component implementation
- Less flexible for different use cases
- Harder to control update frequency
- Component would need to handle LVGL thread-safety

---

## Reference: LVGL Gauge API

```c
// Create gauge
lv_obj_t *gauge = lv_gauge_create(parent, NULL);

// Size
lv_obj_set_size(gauge, width, height);

// Scale configuration  
lv_gauge_set_scale(gauge, angle, line_count, label_count);

// Value range
lv_gauge_set_range(gauge, min, max);

// Rotation offset (degrees, clockwise)
lv_gauge_set_angle_offset(gauge, offset);

// Needles
lv_gauge_set_needle_count(gauge, count, color_array);
lv_gauge_set_value(gauge, needle_index, value);

// Styling
lv_obj_set_style_local_pad_inner(gauge, part, state, padding);
```

---

## Files Modified
- [main/main.c](../main/main.c) - Main application with clock integration
- [main/clock_component.h](../main/clock_component.h) - Clock component public API
- [main/clock_component.c](../main/clock_component.c) - Clock component implementation
- [documentation/analog_clock.md](analog_clock.md) - This documentation
