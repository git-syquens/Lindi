# LVGL UI Elements Reference

This document provides complete reference for all LVGL UI elements available on this board, with code examples extracted from the demo widgets.

## Table of Contents

1. [Basic Concepts](#basic-concepts)
2. [Buttons](#buttons)
3. [Switches](#switches)
4. [Checkboxes](#checkboxes)
5. [Sliders](#sliders)
6. [Labels](#labels)
7. [Text Areas & Keyboard](#text-areas--keyboard)
8. [Tabs (TabView)](#tabs-tabview)
9. [Charts](#charts)
10. [Line Meters](#line-meters)
11. [Gauges](#gauges)
12. [Arcs](#arcs)
13. [Progress Bars](#progress-bars)
14. [LEDs](#leds)
15. [Calendars](#calendars)
16. [Rollers](#rollers)
17. [Dropdowns](#dropdowns)
18. [Lists](#lists)
19. [Tables](#tables)
20. [Message Boxes](#message-boxes)
21. [Containers & Layouts](#containers--layouts)
22. [Animations](#animations)
23. [Styles](#styles)
24. [Events](#events)
25. [Theme Switching](#theme-switching)

---

## Basic Concepts

### Screen and Objects

```c
// Get active screen
lv_obj_t *scr = lv_disp_get_scr_act(NULL);

// Create object on screen
lv_obj_t *obj = lv_obj_create(scr, NULL);  // NULL = no copy source

// Create object as child of another
lv_obj_t *child = lv_btn_create(parent, NULL);
```

### Object Positioning

```c
// Set position (top-left origin)
lv_obj_set_pos(obj, x, y);

// Set size
lv_obj_set_size(obj, width, height);
lv_obj_set_width(obj, width);
lv_obj_set_height(obj, height);

// Alignment relative to parent or another object
lv_obj_align(obj, NULL, LV_ALIGN_CENTER, x_offset, y_offset);
lv_obj_align(obj, reference_obj, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
```

### Alignment Constants

| Constant | Description |
|----------|-------------|
| LV_ALIGN_CENTER | Center of parent |
| LV_ALIGN_IN_TOP_LEFT | Inside, top-left corner |
| LV_ALIGN_IN_TOP_MID | Inside, top center |
| LV_ALIGN_IN_TOP_RIGHT | Inside, top-right corner |
| LV_ALIGN_IN_BOTTOM_LEFT | Inside, bottom-left |
| LV_ALIGN_IN_BOTTOM_MID | Inside, bottom center |
| LV_ALIGN_IN_BOTTOM_RIGHT | Inside, bottom-right |
| LV_ALIGN_OUT_TOP_MID | Outside, above center |
| LV_ALIGN_OUT_BOTTOM_MID | Outside, below center |
| LV_ALIGN_OUT_LEFT_MID | Outside, left center |
| LV_ALIGN_OUT_RIGHT_MID | Outside, right center |

---

## Buttons

### Basic Button

```c
lv_obj_t *btn = lv_btn_create(parent, NULL);
lv_obj_set_size(btn, 120, 50);

// Add label to button
lv_obj_t *label = lv_label_create(btn, NULL);
lv_label_set_text(label, "Click Me");
```

### Toggle Button

```c
lv_obj_t *btn = lv_btn_create(parent, NULL);
lv_btn_set_checkable(btn, true);  // Enable toggle mode
lv_btn_toggle(btn);  // Toggle the state

// Check current state
if (lv_btn_get_state(btn) == LV_BTN_STATE_CHECKED_RELEASED) {
    // Button is toggled ON
}
```

### Button States

| State | Description |
|-------|-------------|
| LV_BTN_STATE_RELEASED | Normal state |
| LV_BTN_STATE_PRESSED | Being pressed |
| LV_BTN_STATE_DISABLED | Cannot be pressed |
| LV_BTN_STATE_CHECKED_RELEASED | Toggle ON, not pressed |
| LV_BTN_STATE_CHECKED_PRESSED | Toggle ON, being pressed |

### Button with Event

```c
void btn_event_cb(lv_obj_t *btn, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED) {
        printf("Button clicked!\n");
    }
}

lv_obj_t *btn = lv_btn_create(parent, NULL);
lv_obj_set_event_cb(btn, btn_event_cb);
```

### Disabled Button

```c
lv_btn_set_state(btn, LV_BTN_STATE_DISABLED);
```

---

## Switches

```c
// Create switch
lv_obj_t *sw = lv_switch_create(parent, NULL);

// Set initial state
lv_switch_on(sw, LV_ANIM_OFF);   // Turn ON
lv_switch_off(sw, LV_ANIM_ON);   // Turn OFF with animation

// Check state
bool is_on = lv_switch_get_state(sw);

// Event handler
void switch_event_cb(lv_obj_t *sw, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED) {
        if (lv_switch_get_state(sw)) {
            printf("Switch ON\n");
        } else {
            printf("Switch OFF\n");
        }
    }
}
lv_obj_set_event_cb(sw, switch_event_cb);
```

### Switch with Label (from demo)

```c
lv_obj_t *sw = lv_switch_create(lv_scr_act(), NULL);
lv_obj_set_style_local_value_str(sw, LV_SWITCH_PART_BG, 
                                  LV_STATE_DEFAULT, "Dark");
lv_obj_set_style_local_value_align(sw, LV_SWITCH_PART_BG, 
                                    LV_STATE_DEFAULT, LV_ALIGN_OUT_RIGHT_MID);
```

---

## Checkboxes

```c
// Create checkbox
lv_obj_t *cb = lv_checkbox_create(parent, NULL);
lv_checkbox_set_text(cb, "Enable Feature");

// Check/uncheck
lv_checkbox_set_checked(cb, true);

// Get state
bool checked = lv_checkbox_is_checked(cb);

// Event handler
void cb_event_cb(lv_obj_t *cb, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED) {
        printf("Checkbox: %s\n", 
               lv_checkbox_is_checked(cb) ? "checked" : "unchecked");
    }
}
lv_obj_set_event_cb(cb, cb_event_cb);
```

---

## Sliders

### Basic Slider

```c
lv_obj_t *slider = lv_slider_create(parent, NULL);
lv_obj_set_width(slider, 200);

// Set range and value
lv_slider_set_range(slider, 0, 100);
lv_slider_set_value(slider, 50, LV_ANIM_OFF);

// Get value
int16_t value = lv_slider_get_value(slider);
```

### Range Slider (two handles)

```c
lv_obj_t *slider = lv_slider_create(parent, NULL);
lv_slider_set_type(slider, LV_SLIDER_TYPE_RANGE);
lv_slider_set_value(slider, 70, LV_ANIM_OFF);       // Right value
lv_slider_set_left_value(slider, 30, LV_ANIM_OFF);  // Left value
```

### Slider with Value Display (from demo)

```c
void slider_event_cb(lv_obj_t *slider, lv_event_t e)
{
    if (e == LV_EVENT_VALUE_CHANGED) {
        static char buf[16];
        lv_snprintf(buf, sizeof(buf), "%d", lv_slider_get_value(slider));
        lv_obj_set_style_local_value_str(slider, LV_SLIDER_PART_KNOB, 
                                          LV_STATE_DEFAULT, buf);
    }
}

lv_obj_t *slider = lv_slider_create(parent, NULL);
lv_slider_set_value(slider, 40, LV_ANIM_OFF);
lv_obj_set_event_cb(slider, slider_event_cb);

// Style for value display on knob
lv_obj_set_style_local_value_font(slider, LV_SLIDER_PART_KNOB, 
                                   LV_STATE_DEFAULT, lv_theme_get_font_small());
lv_obj_set_style_local_value_ofs_y(slider, LV_SLIDER_PART_KNOB, 
                                    LV_STATE_FOCUSED, -25);
```

---

## Labels

```c
// Create label
lv_obj_t *label = lv_label_create(parent, NULL);
lv_label_set_text(label, "Hello World!");

// Formatted text
lv_label_set_text_fmt(label, "Value: %d", value);

// Long text modes
lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);    // Wrap text
lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL);   // Scroll horizontally
lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);      // Truncate with ...
lv_label_set_long_mode(label, LV_LABEL_LONG_CROP);     // Clip text

// Set width for long modes
lv_obj_set_width(label, 150);

// Text alignment
lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
lv_label_set_align(label, LV_LABEL_ALIGN_LEFT);
lv_label_set_align(label, LV_LABEL_ALIGN_RIGHT);
```

---

## Text Areas & Keyboard

### Text Area

```c
// Create text area
lv_obj_t *ta = lv_textarea_create(parent, NULL);
lv_obj_set_size(ta, 200, 100);
lv_textarea_set_text(ta, "");

// Placeholder text
lv_textarea_set_placeholder_text(ta, "Enter text...");

// Single line mode
lv_textarea_set_one_line(ta, true);

// Password mode
lv_textarea_set_pwd_mode(ta, true);

// Hide cursor
lv_textarea_set_cursor_hidden(ta, true);

// Set max length
lv_textarea_set_max_length(ta, 32);

// Accepted characters
lv_textarea_set_accepted_chars(ta, "0123456789");
```

### Keyboard Integration (from demo)

```c
static lv_obj_t *kb = NULL;

void ta_event_cb(lv_obj_t *ta, lv_event_t e)
{
    if (e == LV_EVENT_FOCUSED) {
        if (kb == NULL) {
            kb = lv_keyboard_create(lv_scr_act(), NULL);
            lv_obj_set_event_cb(kb, kb_event_cb);
        }
        lv_textarea_set_cursor_hidden(ta, false);
        lv_keyboard_set_textarea(kb, ta);
    } else if (e == LV_EVENT_DEFOCUSED) {
        lv_textarea_set_cursor_hidden(ta, true);
    }
}

void kb_event_cb(lv_obj_t *_kb, lv_event_t e)
{
    lv_keyboard_def_event_cb(kb, e);
    
    if (e == LV_EVENT_CANCEL) {
        lv_obj_del(kb);
        kb = NULL;
    }
}
```

---

## Tabs (TabView)

```c
// Create tabview
lv_obj_t *tv = lv_tabview_create(lv_scr_act(), NULL);

// Add tabs
lv_obj_t *tab1 = lv_tabview_add_tab(tv, "Settings");
lv_obj_t *tab2 = lv_tabview_add_tab(tv, "Status");
lv_obj_t *tab3 = lv_tabview_add_tab(tv, "About");

// Add content to tabs (tabs are page objects)
lv_obj_t *label = lv_label_create(tab1, NULL);
lv_label_set_text(label, "Settings content here");

// Set active tab
lv_tabview_set_tab_act(tv, 1, LV_ANIM_ON);  // Switch to "Status"

// Get current tab
uint16_t active = lv_tabview_get_tab_act(tv);

// Tab button position
lv_tabview_set_btns_pos(tv, LV_TABVIEW_TAB_POS_TOP);    // Default
lv_tabview_set_btns_pos(tv, LV_TABVIEW_TAB_POS_BOTTOM);
lv_tabview_set_btns_pos(tv, LV_TABVIEW_TAB_POS_LEFT);
lv_tabview_set_btns_pos(tv, LV_TABVIEW_TAB_POS_RIGHT);
```

---

## Charts

### Line Chart

```c
lv_obj_t *chart = lv_chart_create(parent, NULL);
lv_obj_set_size(chart, 200, 150);

// Set chart type
lv_chart_set_type(chart, LV_CHART_TYPE_LINE);

// Configure axes
lv_chart_set_div_line_count(chart, 3, 0);  // 3 horizontal, 0 vertical
lv_chart_set_point_count(chart, 8);

// Add data series
lv_chart_series_t *ser1 = lv_chart_add_series(chart, LV_COLOR_RED);
lv_chart_series_t *ser2 = lv_chart_add_series(chart, LV_COLOR_BLUE);

// Add data points
lv_chart_set_next(chart, ser1, 10);
lv_chart_set_next(chart, ser1, 90);
lv_chart_set_next(chart, ser1, 30);
lv_chart_set_next(chart, ser1, 60);

// Or set all points at once
lv_chart_set_points(chart, ser1, (lv_coord_t[]){10, 90, 30, 60, 10, 90, 30, 60});

// Set Y range
lv_chart_set_range(chart, 0, 100);

// Axis tick labels
lv_chart_set_y_tick_texts(chart, "100\n75\n50\n25\n0", 0, LV_CHART_AXIS_DRAW_LAST_TICK);
lv_chart_set_x_tick_texts(chart, "Jan\nFeb\nMar\nApr", 0, LV_CHART_AXIS_DRAW_LAST_TICK);
```

### Column/Bar Chart

```c
lv_obj_t *chart = lv_chart_create(parent, NULL);
lv_chart_set_type(chart, LV_CHART_TYPE_COLUMN);

lv_chart_series_t *ser = lv_chart_add_series(chart, LV_COLOR_GREEN);
lv_chart_set_next(chart, ser, 30);
lv_chart_set_next(chart, ser, 70);
lv_chart_set_next(chart, ser, 50);
```

---

## Line Meters

```c
lv_obj_t *lmeter = lv_linemeter_create(parent, NULL);
lv_obj_set_size(lmeter, 150, 150);

// Set range and value
lv_linemeter_set_range(lmeter, 0, 100);
lv_linemeter_set_value(lmeter, 50);

// Set scale
lv_linemeter_set_scale(lmeter, 240, 31);  // 240° angle, 31 lines

// Add label in center
lv_obj_t *label = lv_label_create(lmeter, NULL);
lv_label_set_text(label, "50");
lv_obj_align(label, lmeter, LV_ALIGN_CENTER, 0, 0);

// Animate (from demo)
lv_anim_t a;
lv_anim_init(&a);
lv_anim_set_var(&a, lmeter);
lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_linemeter_set_value);
lv_anim_set_values(&a, 0, 100);
lv_anim_set_time(&a, 4000);
lv_anim_set_playback_time(&a, 1000);
lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
lv_anim_start(&a);
```

---

## Gauges

```c
lv_obj_t *gauge = lv_gauge_create(parent, NULL);
lv_obj_set_size(gauge, 150, 150);

// Configure scale
lv_gauge_set_range(gauge, 0, 100);
lv_gauge_set_scale(gauge, 240, 21, 5);  // 240°, 21 labels, 5 major ticks

// Set needle value (needle index, value)
lv_gauge_set_value(gauge, 0, 50);

// Multiple needles
lv_gauge_set_needle_count(gauge, 2, (lv_color_t[]){LV_COLOR_RED, LV_COLOR_BLUE});
lv_gauge_set_value(gauge, 0, 30);  // First needle
lv_gauge_set_value(gauge, 1, 70);  // Second needle

// Critical value (changes color)
lv_gauge_set_critical_value(gauge, 80);
```

---

## Arcs

```c
lv_obj_t *arc = lv_arc_create(parent, NULL);
lv_obj_set_size(arc, 150, 150);

// Set angles (0° = 3 o'clock, clockwise)
lv_arc_set_bg_angles(arc, 0, 360);     // Background arc
lv_arc_set_angles(arc, 0, 180);         // Foreground arc (indicator)

// Set rotation (start angle offset)
lv_arc_set_rotation(arc, 270);  // Start from top

// Set value (if using as slider)
lv_arc_set_range(arc, 0, 100);
lv_arc_set_value(arc, 50);

// Make non-adjustable (display only)
lv_arc_set_adjustable(arc, false);

// Animation (from demo)
static void arc_anim(lv_obj_t *arc, lv_anim_value_t value)
{
    lv_arc_set_end_angle(arc, value);
    
    static char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d", value);
    lv_obj_t *label = lv_obj_get_child(arc, NULL);
    lv_label_set_text(label, buf);
    lv_obj_align(label, arc, LV_ALIGN_CENTER, 0, 0);
}
```

---

## Progress Bars

```c
lv_obj_t *bar = lv_bar_create(parent, NULL);
lv_obj_set_size(bar, 200, 20);

// Set range and value
lv_bar_set_range(bar, 0, 100);
lv_bar_set_value(bar, 50, LV_ANIM_OFF);

// Animated value change
lv_bar_set_value(bar, 75, LV_ANIM_ON);

// Get current/max value
int16_t val = lv_bar_get_value(bar);
int16_t max = lv_bar_get_max_value(bar);

// Animated progress update (from demo)
void bar_anim(lv_task_t *t)
{
    static uint32_t x = 0;
    lv_obj_t *bar = t->user_data;
    
    static char buf[64];
    lv_snprintf(buf, sizeof(buf), "Loading %d/%d", x, lv_bar_get_max_value(bar));
    lv_obj_set_style_local_value_str(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, buf);
    
    lv_bar_set_value(bar, x, LV_ANIM_OFF);
    x++;
    if (x > lv_bar_get_max_value(bar)) x = 0;
}

lv_task_create(bar_anim, 100, LV_TASK_PRIO_LOW, bar);
```

---

## LEDs

```c
lv_obj_t *led = lv_led_create(parent, NULL);
lv_obj_set_size(led, 30, 30);

// States
lv_led_off(led);    // Dim
lv_led_on(led);     // Bright

// Set brightness (LV_LED_BRIGHT_MIN to LV_LED_BRIGHT_MAX)
lv_led_set_bright(led, 150);

// Example: Three LEDs with different brightness (from demo)
lv_obj_t *led1 = lv_led_create(parent, NULL);
lv_led_off(led1);  // Off

lv_obj_t *led2 = lv_led_create(parent, led1);  // Copy from led1
lv_led_set_bright(led2, (LV_LED_BRIGHT_MAX - LV_LED_BRIGHT_MIN) / 2 + LV_LED_BRIGHT_MIN);

lv_obj_t *led3 = lv_led_create(parent, led1);
lv_led_on(led3);  // Full brightness
```

---

## Calendars

```c
lv_obj_t *cal = lv_calendar_create(parent, NULL);
lv_obj_set_size(cal, 200, 200);

// Set current date
lv_calendar_date_t today = {
    .year = 2025,
    .month = 12,
    .day = 20
};
lv_calendar_set_today_date(cal, &today);
lv_calendar_set_showed_date(cal, &today);

// Highlight specific dates
static lv_calendar_date_t highlights[] = {
    {.year = 2025, .month = 12, .day = 25},  // Christmas
    {.year = 2025, .month = 12, .day = 31},  // New Year's Eve
};
lv_calendar_set_highlighted_dates(cal, highlights, 2);

// Event handler
void cal_event_cb(lv_obj_t *cal, lv_event_t e)
{
    if (e == LV_EVENT_VALUE_CHANGED) {
        lv_calendar_date_t *date = lv_calendar_get_pressed_date(cal);
        if (date) {
            printf("Selected: %d/%d/%d\n", date->year, date->month, date->day);
        }
    }
}
lv_obj_set_event_cb(cal, cal_event_cb);
```

---

## Rollers

```c
lv_obj_t *roller = lv_roller_create(parent, NULL);

// Set options (newline separated)
lv_roller_set_options(roller,
    "January\n"
    "February\n"
    "March\n"
    "April\n"
    "May\n"
    "June",
    LV_ROLLER_MODE_NORMAL);  // or LV_ROLLER_MODE_INFINITE

// Visible rows
lv_roller_set_visible_row_count(roller, 4);

// Set selected
lv_roller_set_selected(roller, 2, LV_ANIM_ON);  // Select "March"

// Get selected
uint16_t sel = lv_roller_get_selected(roller);

// Get selected text
char buf[32];
lv_roller_get_selected_str(roller, buf, sizeof(buf));

// Alignment
lv_roller_set_align(roller, LV_LABEL_ALIGN_CENTER);

// Auto-fit width
lv_roller_set_auto_fit(roller, true);
```

---

## Dropdowns

```c
lv_obj_t *dd = lv_dropdown_create(parent, NULL);
lv_obj_set_width(dd, 150);

// Set options
lv_dropdown_set_options(dd,
    "Option 1\n"
    "Option 2\n"
    "Option 3\n"
    "Option 4");

// Add options dynamically
lv_dropdown_add_option(dd, "New Option", LV_DROPDOWN_POS_LAST);

// Set selected
lv_dropdown_set_selected(dd, 1);

// Get selected
uint16_t sel = lv_dropdown_get_selected(dd);

// Get selected text
char buf[32];
lv_dropdown_get_selected_str(dd, buf, sizeof(buf));

// Drop direction
lv_dropdown_set_dir(dd, LV_DROPDOWN_DIR_DOWN);   // Default
lv_dropdown_set_dir(dd, LV_DROPDOWN_DIR_UP);
lv_dropdown_set_dir(dd, LV_DROPDOWN_DIR_LEFT);
lv_dropdown_set_dir(dd, LV_DROPDOWN_DIR_RIGHT);

// Max height of dropdown list
lv_dropdown_set_max_height(dd, 150);
```

---

## Lists

```c
lv_obj_t *list = lv_list_create(parent, NULL);
lv_obj_set_size(list, 180, 200);

// Add buttons with icons
lv_obj_t *btn;
btn = lv_list_add_btn(list, LV_SYMBOL_WIFI, "WiFi");
btn = lv_list_add_btn(list, LV_SYMBOL_BLUETOOTH, "Bluetooth");
btn = lv_list_add_btn(list, LV_SYMBOL_GPS, "GPS");
btn = lv_list_add_btn(list, LV_SYMBOL_SETTINGS, "Settings");

// Make button checkable
lv_btn_set_checkable(btn, true);

// Disable a button
lv_btn_set_state(btn, LV_BTN_STATE_DISABLED);

// Event handler
void list_btn_event_cb(lv_obj_t *btn, lv_event_t e)
{
    if (e == LV_EVENT_CLICKED) {
        const char *txt = lv_list_get_btn_text(btn);
        printf("Clicked: %s\n", txt);
    }
}

// Available symbols
// LV_SYMBOL_SAVE, LV_SYMBOL_CUT, LV_SYMBOL_COPY, LV_SYMBOL_PASTE
// LV_SYMBOL_OK, LV_SYMBOL_CLOSE, LV_SYMBOL_EDIT, LV_SYMBOL_TRASH
// LV_SYMBOL_WIFI, LV_SYMBOL_BLUETOOTH, LV_SYMBOL_GPS, LV_SYMBOL_USB
// LV_SYMBOL_SD_CARD, LV_SYMBOL_BATTERY_FULL, LV_SYMBOL_CHARGE
// LV_SYMBOL_PLAY, LV_SYMBOL_PAUSE, LV_SYMBOL_STOP, LV_SYMBOL_NEXT
// LV_SYMBOL_PREV, LV_SYMBOL_HOME, LV_SYMBOL_REFRESH, LV_SYMBOL_LOOP
```

---

## Tables

```c
lv_obj_t *table = lv_table_create(parent, NULL);

// Set dimensions
lv_table_set_col_cnt(table, 3);
lv_table_set_row_cnt(table, 5);

// Set column widths
lv_table_set_col_width(table, 0, 50);   // Column 0
lv_table_set_col_width(table, 1, 100);  // Column 1
lv_table_set_col_width(table, 2, 100);  // Column 2

// Set cell values
lv_table_set_cell_value(table, 0, 0, "#");     // Header
lv_table_set_cell_value(table, 0, 1, "Name");
lv_table_set_cell_value(table, 0, 2, "City");

lv_table_set_cell_value(table, 1, 0, "1");
lv_table_set_cell_value(table, 1, 1, "John");
lv_table_set_cell_value(table, 1, 2, "London");

// Cell click handler
void table_event_cb(lv_obj_t *table, lv_event_t e)
{
    if (e == LV_EVENT_CLICKED) {
        uint16_t row, col;
        lv_res_t r = lv_table_get_pressed_cell(table, &row, &col);
        if (r == LV_RES_OK) {
            printf("Clicked cell: row %d, col %d\n", row, col);
        }
    }
}
lv_obj_set_event_cb(table, table_event_cb);
```

---

## Message Boxes

```c
// Simple message box
static const char *btns[] = {"OK", ""};
lv_obj_t *mbox = lv_msgbox_create(lv_scr_act(), NULL);
lv_msgbox_set_text(mbox, "This is a message!");
lv_msgbox_add_btns(mbox, btns);
lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, 0);

// With multiple buttons
static const char *btns2[] = {"Cancel", "OK", ""};
lv_obj_t *mbox = lv_msgbox_create(lv_scr_act(), NULL);
lv_msgbox_set_text(mbox, "Are you sure?");
lv_msgbox_add_btns(mbox, btns2);

// Pre-select a button
lv_obj_t *btnm = lv_msgbox_get_btnmatrix(mbox);
lv_btnmatrix_set_btn_ctrl(btnm, 1, LV_BTNMATRIX_CTRL_CHECK_STATE);

// Event handler
void mbox_event_cb(lv_obj_t *mbox, lv_event_t e)
{
    if (e == LV_EVENT_VALUE_CHANGED) {
        const char *txt = lv_msgbox_get_active_btn_text(mbox);
        if (strcmp(txt, "OK") == 0) {
            // OK clicked
        }
        lv_msgbox_start_auto_close(mbox, 0);  // Close immediately
    }
}
lv_obj_set_event_cb(mbox, mbox_event_cb);

// Auto-close after delay
lv_msgbox_start_auto_close(mbox, 2000);  // Close after 2 seconds
```

---

## Containers & Layouts

### Container

```c
lv_obj_t *cont = lv_cont_create(parent, NULL);
lv_obj_set_size(cont, 200, 150);

// Auto-fit to content
lv_cont_set_fit(cont, LV_FIT_TIGHT);
lv_cont_set_fit2(cont, LV_FIT_NONE, LV_FIT_TIGHT);  // width fixed, height fits
lv_cont_set_fit4(cont, LV_FIT_NONE, LV_FIT_NONE, LV_FIT_TIGHT, LV_FIT_TIGHT);

// Layout
lv_cont_set_layout(cont, LV_LAYOUT_OFF);           // No auto-layout
lv_cont_set_layout(cont, LV_LAYOUT_CENTER);        // Center children
lv_cont_set_layout(cont, LV_LAYOUT_COLUMN_LEFT);   // Column, left aligned
lv_cont_set_layout(cont, LV_LAYOUT_COLUMN_MID);    // Column, centered
lv_cont_set_layout(cont, LV_LAYOUT_COLUMN_RIGHT);  // Column, right aligned
lv_cont_set_layout(cont, LV_LAYOUT_ROW_TOP);       // Row, top aligned
lv_cont_set_layout(cont, LV_LAYOUT_ROW_MID);       // Row, centered
lv_cont_set_layout(cont, LV_LAYOUT_ROW_BOTTOM);    // Row, bottom aligned
lv_cont_set_layout(cont, LV_LAYOUT_PRETTY_TOP);    // Grid, top aligned
lv_cont_set_layout(cont, LV_LAYOUT_PRETTY_MID);    // Grid, centered
lv_cont_set_layout(cont, LV_LAYOUT_PRETTY_BOTTOM); // Grid, bottom aligned
lv_cont_set_layout(cont, LV_LAYOUT_GRID);          // Grid layout
```

### Page (Scrollable Container)

```c
lv_obj_t *page = lv_page_create(parent, NULL);
lv_obj_set_size(page, 200, 150);

// Set scrollable area layout
lv_page_set_scrl_layout(page, LV_LAYOUT_PRETTY_TOP);

// Add content (will be scrollable)
for (int i = 0; i < 10; i++) {
    lv_obj_t *btn = lv_btn_create(page, NULL);
    lv_obj_t *label = lv_label_create(btn, NULL);
    lv_label_set_text_fmt(label, "Button %d", i);
}

// Scroll propagation (scroll parent when edge reached)
lv_page_set_scroll_propagation(page, true);

// Focus an object in the page
lv_page_focus(page, some_child_obj, LV_ANIM_ON);
```

---

## Animations

```c
lv_anim_t a;
lv_anim_init(&a);

// Set target object
lv_anim_set_var(&a, obj);

// Set animation callback (what property to animate)
lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);

// Set values
lv_anim_set_values(&a, 0, 100);  // From 0 to 100

// Set duration
lv_anim_set_time(&a, 1000);  // 1 second

// Playback (reverse after completing)
lv_anim_set_playback_time(&a, 500);  // Reverse in 500ms
lv_anim_set_playback_delay(&a, 100); // Wait 100ms before reversing

// Repeat
lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
lv_anim_set_repeat_delay(&a, 200);  // Wait between repeats

// Start
lv_anim_start(&a);

// Common animation callbacks
lv_obj_set_x           // Move horizontally
lv_obj_set_y           // Move vertically
lv_obj_set_width       // Change width
lv_obj_set_height      // Change height
lv_arc_set_end_angle   // Animate arc
lv_bar_set_value       // Animate bar
lv_slider_set_value    // Animate slider
lv_gauge_set_value     // Animate gauge
lv_linemeter_set_value // Animate line meter
```

---

## Styles

```c
// Create style
static lv_style_t style;
lv_style_init(&style);

// Background
lv_style_set_bg_color(&style, LV_STATE_DEFAULT, LV_COLOR_BLUE);
lv_style_set_bg_opa(&style, LV_STATE_DEFAULT, LV_OPA_COVER);

// Border
lv_style_set_border_color(&style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
lv_style_set_border_width(&style, LV_STATE_DEFAULT, 2);
lv_style_set_radius(&style, LV_STATE_DEFAULT, 10);

// Text
lv_style_set_text_color(&style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
lv_style_set_text_font(&style, LV_STATE_DEFAULT, &lv_font_montserrat_16);

// Padding
lv_style_set_pad_top(&style, LV_STATE_DEFAULT, 10);
lv_style_set_pad_bottom(&style, LV_STATE_DEFAULT, 10);
lv_style_set_pad_left(&style, LV_STATE_DEFAULT, 10);
lv_style_set_pad_right(&style, LV_STATE_DEFAULT, 10);

// Margin
lv_style_set_margin_top(&style, LV_STATE_DEFAULT, 5);

// Apply to object
lv_obj_add_style(obj, LV_OBJ_PART_MAIN, &style);

// Local style (object-specific)
lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
```

### States

| State | Description |
|-------|-------------|
| LV_STATE_DEFAULT | Normal state |
| LV_STATE_CHECKED | Toggled/checked |
| LV_STATE_FOCUSED | Has input focus |
| LV_STATE_EDITED | Being edited |
| LV_STATE_HOVERED | Mouse hover |
| LV_STATE_PRESSED | Being pressed |
| LV_STATE_DISABLED | Disabled |

---

## Events

### Event Callback

```c
void event_cb(lv_obj_t *obj, lv_event_t event)
{
    switch (event) {
        case LV_EVENT_CLICKED:
            // Object was clicked
            break;
        case LV_EVENT_VALUE_CHANGED:
            // Value changed (slider, checkbox, etc.)
            break;
        case LV_EVENT_PRESSED:
            // Touch down
            break;
        case LV_EVENT_RELEASED:
            // Touch up
            break;
        case LV_EVENT_LONG_PRESSED:
            // Long press detected
            break;
        case LV_EVENT_FOCUSED:
            // Got input focus
            break;
        case LV_EVENT_DEFOCUSED:
            // Lost input focus
            break;
    }
}

lv_obj_set_event_cb(obj, event_cb);
```

### Common Events

| Event | Description |
|-------|-------------|
| LV_EVENT_PRESSED | Touch/click started |
| LV_EVENT_PRESSING | Continuous press |
| LV_EVENT_PRESS_LOST | Moved outside object |
| LV_EVENT_SHORT_CLICKED | Quick click |
| LV_EVENT_LONG_PRESSED | Long press (>400ms) |
| LV_EVENT_LONG_PRESSED_REPEAT | Repeat while held |
| LV_EVENT_CLICKED | Click completed |
| LV_EVENT_RELEASED | Touch released |
| LV_EVENT_VALUE_CHANGED | Value changed |
| LV_EVENT_FOCUSED | Got focus |
| LV_EVENT_DEFOCUSED | Lost focus |

---

## Theme Switching

```c
// Switch between light and dark theme (from demo)
void color_chg_event_cb(lv_obj_t *sw, lv_event_t e)
{
    if (e == LV_EVENT_VALUE_CHANGED) {
        uint32_t flag = LV_THEME_MATERIAL_FLAG_LIGHT;
        if (lv_switch_get_state(sw)) {
            flag = LV_THEME_MATERIAL_FLAG_DARK;
        }
        
        LV_THEME_DEFAULT_INIT(
            lv_theme_get_color_primary(),
            lv_theme_get_color_secondary(),
            flag,
            lv_theme_get_font_small(),
            lv_theme_get_font_normal(),
            lv_theme_get_font_subtitle(),
            lv_theme_get_font_title()
        );
    }
}

// Create theme toggle switch
lv_obj_t *sw = lv_switch_create(lv_scr_act(), NULL);
lv_obj_set_event_cb(sw, color_chg_event_cb);
lv_obj_set_style_local_value_str(sw, LV_SWITCH_PART_BG, LV_STATE_DEFAULT, "Dark");
```

---

## Task Handler and Tick

### Main Loop Pattern

```c
// In guiTask()
const esp_timer_create_args_t periodic_timer_args = {
    .callback = &lv_tick_task,
    .name = "periodic_gui"
};
esp_timer_handle_t periodic_timer;
ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

// Main loop
while (1) {
    vTaskDelay(1);
    if (xSemaphoreTake(xGuiSemaphore, (TickType_t)10) == pdTRUE) {
        lv_task_handler();  // Process LVGL tasks
        xSemaphoreGive(xGuiSemaphore);
    }
}

// Tick callback
static void lv_tick_task(void *arg) {
    (void)arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}
```

### Thread Safety

Always use the mutex when calling LVGL functions from other tasks:

```c
extern SemaphoreHandle_t xGuiSemaphore;

void update_label_from_other_task(const char *text)
{
    if (xSemaphoreTake(xGuiSemaphore, portMAX_DELAY) == pdTRUE) {
        lv_label_set_text(my_label, text);
        xSemaphoreGive(xGuiSemaphore);
    }
}
```
