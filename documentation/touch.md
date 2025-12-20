# Touch Controller Reference (XPT2046)

## Controller Specifications

| Property | Value |
|----------|-------|
| Controller | XPT2046 |
| Type | 4-wire Resistive Touch |
| Interface | GPIO Bit-Bang SPI |
| Resolution | 12-bit ADC |
| Channels | X, Y, Pressure |
| IRQ | Active Low |

---

## Pin Configuration

| Signal | GPIO | Direction | Notes |
|--------|------|-----------|-------|
| MISO | 39 | Input | SENSOR_VN, input-only pin |
| MOSI | 32 | Output | Data to XPT2046 |
| CLK | 25 | Output | SPI clock |
| CS | 33 | Output | Chip select (active low) |
| IRQ | 36 | Input | SENSOR_VP, touch interrupt, input-only |

**Note**: This implementation uses GPIO bit-banging, NOT hardware SPI.

---

## Configuration (xpt2046.h)

```c
// Pin definitions
#define XPT2046_IRQ     36
#define XPT2046_MOSI    32
#define XPT2046_MISO    39
#define XPT2046_CLK     25
#define XPT2046_CS      33

// Calibration values
#define XPT2046_AVG     4       // Number of samples to average
#define XPT2046_X_MIN   200     // Minimum raw X value
#define XPT2046_Y_MIN   120     // Minimum raw Y value
#define XPT2046_X_MAX   1900    // Maximum raw X value
#define XPT2046_Y_MAX   1900    // Maximum raw Y value

// Axis configuration
#define XPT2046_X_INV   1       // Invert X axis
#define XPT2046_Y_INV   1       // Invert Y axis
#define XPT2046_XY_SWAP 0       // Swap X and Y axes
```

---

## sdkconfig Touch Settings

```kconfig
CONFIG_LVGL_TOUCH_CONTROLLER=1
CONFIG_LVGL_TOUCH_CONTROLLER_XPT2046=y
CONFIG_LVGL_TOUCH_DRIVER_PROTOCOL_SPI=y
CONFIG_LVGL_TOUCH_CONTROLLER_SPI_HSPI=y
CONFIG_LVGL_TOUCH_SPI_MISO=19
CONFIG_LVGL_TOUCH_SPI_MOSI=23
CONFIG_LVGL_TOUCH_SPI_CLK=18
CONFIG_LVGL_TOUCH_SPI_CS=5
CONFIG_LVGL_TOUCH_PIN_IRQ=25
CONFIG_LVGL_TOUCH_X_MIN=200
CONFIG_LVGL_TOUCH_Y_MIN=120
CONFIG_LVGL_TOUCH_X_MAX=1900
CONFIG_LVGL_TOUCH_Y_MAX=1900
```

**Note**: The actual driver uses hardcoded values from xpt2046.h, not sdkconfig.

---

## Driver Initialization

```c
// In xpt2046.c
void xpt2046_init(void)
{
    // Configure IRQ pin as input
    gpio_config_t irq_config = {
        .pin_bit_mask = BIT64(XPT2046_IRQ),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&irq_config);
    
    // Configure MISO pin as input
    gpio_config_t miso_config = {
        .pin_bit_mask = BIT64(XPT2046_MISO),
        .mode = GPIO_MODE_INPUT,
        // ...
    };
    gpio_config(&miso_config);
    
    // Configure MOSI, CLK, CS as outputs
    gpio_pad_select_gpio(XPT2046_MOSI);
    gpio_set_direction(XPT2046_MOSI, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(XPT2046_CLK);
    gpio_set_direction(XPT2046_CLK, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(XPT2046_CS);
    gpio_set_direction(XPT2046_CS, GPIO_MODE_OUTPUT);
}
```

---

## Touch Reading Process

```c
bool xpt2046_read(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    static int16_t last_x = 0, last_y = 0;
    bool valid = true;
    
    // Check if touch is active (IRQ low)
    uint8_t irq = gpio_get_level(XPT2046_IRQ);
    
    if (irq == 0) {
        uint16_t ux, uy;
        if (TP_Read_XY2(&ux, &uy) == 1) {
            // Apply calibration and mapping
            int16_t x = (ux - XPT2046_X_MIN) * LV_HOR_RES_MAX / 
                        (XPT2046_X_MAX - XPT2046_X_MIN);
            int16_t y = (uy - XPT2046_Y_MIN) * LV_VER_RES_MAX / 
                        (XPT2046_Y_MAX - XPT2046_Y_MIN);
            
            // Apply inversion if configured
            if (XPT2046_X_INV) x = LV_HOR_RES_MAX - x;
            if (XPT2046_Y_INV) y = LV_VER_RES_MAX - y;
            
            last_x = x;
            last_y = y;
            data->state = LV_INDEV_STATE_PR;
        }
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
    
    data->point.x = last_x;
    data->point.y = last_y;
    
    return false;  // No more data to read
}
```

---

## XPT2046 Commands

| Command | Binary | Hex | Description |
|---------|--------|-----|-------------|
| Read X | 10010000 | 0x90 | Read X position |
| Read Y | 11010000 | 0xD0 | Read Y position |

---

## Touch Input Registration with LVGL

```c
// In guiTask()
#if CONFIG_LVGL_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;  // Callback function
    indev_drv.type = LV_INDEV_TYPE_POINTER; // Touch is a pointer device
    lv_indev_drv_register(&indev_drv);
#endif
```

---

## Calibration Guide

### Finding Calibration Values

1. Temporarily modify the code to print raw values:
```c
ESP_LOGI(TAG, "Raw X: %d, Y: %d", ux, uy);
```

2. Touch the four corners and record values:
   - Top-left: Note minimum X, minimum Y
   - Bottom-right: Note maximum X, maximum Y

3. Update xpt2046.h with your values:
```c
#define XPT2046_X_MIN   <your_min_x>
#define XPT2046_Y_MIN   <your_min_y>
#define XPT2046_X_MAX   <your_max_x>
#define XPT2046_Y_MAX   <your_max_y>
```

### Axis Adjustment

If touch coordinates are:
- **Mirrored horizontally**: Set `XPT2046_X_INV = 1`
- **Mirrored vertically**: Set `XPT2046_Y_INV = 1`
- **X and Y swapped**: Set `XPT2046_XY_SWAP = 1`

---

## Averaging/Filtering

The driver uses averaging to reduce noise:

```c
#define XPT2046_AVG 4  // Number of samples to average

int16_t avg_buf_x[XPT2046_AVG];
int16_t avg_buf_y[XPT2046_AVG];

static void xpt2046_avg(int16_t *x, int16_t *y)
{
    // Shift buffer and add new sample
    // Return average of all samples
}
```

---

## Supported Touch Controllers

| Controller | Interface | Notes |
|------------|-----------|-------|
| XPT2046 | SPI | **Current configuration** (resistive) |
| FT6X06 | I2C | Capacitive touch |
| STMPE610 | SPI/I2C | Resistive touch |
| FT81X | SPI | Integrated display+touch |
| RA8875 | SPI | Integrated display+touch |
