# Analog Clock Implementation

## Overview
The analog clock is implemented using LVGL 7's `lv_gauge` widget with 3 needles for hour, minute, and second hands.

## Current Status: IN PROGRESS (Dec 21, 2025)

### What Works
- ✅ All 12 hour labels displayed correctly (12, 1, 2, 3...11)
- ✅ Labels positioned at correct clock positions
- ✅ 12 tick marks aligned with hour labels
- ✅ 3 needles (hour, minute, second) with distinct colors
- ✅ Gap in dial arc is acceptable (between positions 1-2)

### What Needs Work
- 🔲 Hand positions not accurate for current time (Item #5)
- 🔲 Needle rotation offset needs calibration

---

## LVGL 7 Gauge Mechanics (IMPORTANT - READ FIRST)

### Gauge Coordinate System
The LVGL gauge widget has a specific coordinate system:
- **Default start position**: Value 0 starts at **9 o'clock** (left side, 270° in standard coords)
- **Direction**: Values increase **clockwise**
- **Full circle**: Would be 360°, but causes label overlap issues

### Key Functions and Their Effects

#### 1. `lv_gauge_set_scale(gauge, angle, line_count, label_count)`
| Parameter | Description | Current Value |
|-----------|-------------|---------------|
| `angle` | Arc span in degrees | 330° (leaves 30° gap to show all 12 labels) |
| `line_count` | Number of tick marks | 12 (one per hour) |
| `label_count` | Number of labels | 12 |

**Critical Note**: With 360° arc, LVGL skips drawing label at value=0 because it overlaps with the endpoint. Using 330° ensures all 12 labels are drawn but creates a visible gap.

#### 2. `lv_gauge_set_angle_offset(gauge, offset)`
- Rotates the **ENTIRE gauge** clockwise by `offset` degrees
- Affects: arc position, tick marks, labels, AND needle angles
- Current value: **225°**
- This puts the 30° gap roughly between the 1 and 2 o'clock positions

#### 3. `lv_gauge_set_range(gauge, min, max)`
- Sets the value range for needles
- Current: 0-59 (maps to seconds/minutes)
- Each unit = 6° of rotation (360° / 60 = 6°)

#### 4. `lv_gauge_set_formatter_cb(gauge, callback)`
- Custom function to determine what TEXT appears at each label position
- Does NOT change where labels are physically drawn
- Only changes the string content

### Label Formatter Logic
The formatter receives values: 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55 (every 5th value for 12 labels)

**Current Formula:**
```c
int hour = ((value / 5) + 7) % 12;
if (hour == 0) display "12", else display hour
```

**Why +7?**
- With angle_offset=225°, the physical positions shifted
- +7 compensates so "12" appears at top position
- This was determined empirically through testing

**Mapping Table (with current settings):**
| Gauge Value | Position on Dial | Displayed Hour |
|-------------|------------------|----------------|
| 0 | ~7 o'clock | 7 |
| 5 | ~8 o'clock | 8 |
| 10 | ~9 o'clock | 9 |
| 15 | ~10 o'clock | 10 |
| 20 | ~11 o'clock | 11 |
| 25 | ~12 o'clock (top) | 12 |
| 30 | ~1 o'clock | 1 |
| 35 | ~2 o'clock | 2 |
| 40 | ~3 o'clock | 3 |
| 45 | ~4 o'clock | 4 |
| 50 | ~5 o'clock | 5 |
| 55 | ~6 o'clock | 6 |

---

## Needle Configuration

### Colors
| Needle Index | Purpose | Color | RGB |
|--------------|---------|-------|-----|
| 0 | Hour hand | Light gray | (200, 200, 200) |
| 1 | Minute hand | Darker gray | (150, 150, 150) |
| 2 | Second hand | Red | LV_COLOR_RED |

### Current Needle Value Calculation (NEEDS FIXING)
Located in `clock_update_task()`:

```c
const int rotation_offset = 45;  // This value needs recalibration!

// Second hand
int sec_value = (timeinfo.tm_sec + rotation_offset) % 60;

// Minute hand  
int min_value = (timeinfo.tm_min + rotation_offset) % 60;

// Hour hand (with smooth movement based on minutes)
int hour_value = ((hour_12 * 5) + (timeinfo.tm_min / 12) + rotation_offset) % 60;
```

**Problem**: The rotation_offset of 45 was set before we changed angle_offset to 225°. These need to be synchronized.

### Needle Rotation Theory
- Needle value 0 points to where gauge value 0 is drawn
- With angle_offset=225°, value 0 is rotated 225° clockwise from default
- The needle rotation_offset should compensate so needles point correctly

**To Fix (Item #5):**
1. Determine where needle value=0 currently points
2. Calculate offset needed for needle to point at 12 o'clock when time is 12:00:00
3. Apply same offset to all three needles

---

## Current Code Locations

### main/main.c

| Line (approx) | Content |
|---------------|---------|
| ~407-418 | `clock_label_formatter()` - label text function |
| ~500-525 | Gauge creation and configuration |
| ~775-795 | `clock_update_task()` - needle updates every second |

### Key Configuration Block
```c
// Configure gauge for clock: 330° arc (11/12 of circle)
lv_gauge_set_scale(analog_clock_gauge, 330, 12, 12);
lv_gauge_set_range(analog_clock_gauge, 0, 59);
lv_gauge_set_angle_offset(analog_clock_gauge, 225);

// 3 needles with colors
static lv_color_t needle_colors[3];
needle_colors[0] = LV_COLOR_MAKE(200, 200, 200);  // Hour
needle_colors[1] = LV_COLOR_MAKE(150, 150, 150);  // Minute
needle_colors[2] = LV_COLOR_RED;                   // Second
lv_gauge_set_needle_count(analog_clock_gauge, 3, needle_colors);

// Custom label formatter
lv_gauge_set_formatter_cb(analog_clock_gauge, clock_label_formatter);
```

---

## Troubleshooting History

### Issue: Only 11 labels showing (missing "9")
- **Cause**: 360° arc causes first/last label overlap, LVGL skips one
- **Solution**: Use 330° arc instead

### Issue: Labels showing wrong numbers (0, 5, 10... instead of 12, 1, 2...)
- **Cause**: Default formatter shows raw gauge values
- **Solution**: Custom formatter with hour mapping formula

### Issue: "12" not at top position
- **Cause**: Gauge default starts at 9 o'clock position
- **Solution**: Combination of angle_offset (225°) and formatter adjustment (+7)

### Issue: Fine-tuning label positions
- **Finding**: `lv_gauge_set_angle_offset()` rotates EVERYTHING together (labels, ticks, arc, needles)
- Cannot rotate labels independently from tick marks
- Best achievable alignment is with current settings

---

## Next Steps (for tomorrow)

### Item #5: Fix Hand Positions
1. Flash current code and note where second hand points at :00 seconds
2. Calculate how many degrees off it is from 12 o'clock
3. Convert degrees to gauge value units (1 unit = 6°)
4. Adjust `rotation_offset` in `clock_update_task()`
5. Test and verify all three hands

### Potential Formula
If second hand at :00 should point to value X to align with 12:
```c
const int rotation_offset = X;  // Where X makes needle point at 12 when time is :00
```

The offset should be calculated as:
- Observe where needle points when value=0
- Calculate degrees from that position to 12 o'clock
- Divide by 6 to get value offset

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

// Custom labels
lv_gauge_set_formatter_cb(gauge, callback_function);
```

---

## Files Modified
- `main/main.c` - Main implementation
- `documentation/analog_clock.md` - This documentation
