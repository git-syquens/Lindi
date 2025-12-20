# LCD Display Reference (ILI9341)

## Display Specifications

| Property | Value |
|----------|-------|
| Controller | ILI9341 |
| Resolution | 320 x 240 pixels |
| Color Depth | 16-bit (RGB565) |
| Interface | SPI (4-wire) |
| SPI Bus | HSPI (SPI2_HOST) |
| Max SPI Clock | 40 MHz (configured at 20 MHz with divider 2) |

---

## Pin Configuration

| Signal | GPIO | sdkconfig Key |
|--------|------|---------------|
| MOSI | 13 | CONFIG_LVGL_DISP_SPI_MOSI |
| MISO | 12 | (optional, not typically used) |
| CLK | 14 | CONFIG_LVGL_DISP_SPI_CLK |
| CS | 15 | CONFIG_LVGL_DISP_SPI_CS |
| DC | 2 | CONFIG_LVGL_DISP_SPI_DC |
| Reset | 4 | CONFIG_LVGL_DISP_SPI_RST |
| Backlight | - | Not connected (always on) |

---

## SPI Configuration (sdkconfig)

```kconfig
CONFIG_LVGL_TFT_DISPLAY_CONTROLLER_ILI9341=y
CONFIG_LVGL_TFT_DISPLAY_PROTOCOL_SPI=y
CONFIG_LVGL_TFT_DISPLAY_SPI_HSPI=y
CONFIG_LVGL_DISPLAY_WIDTH=320
CONFIG_LVGL_DISPLAY_HEIGHT=240
CONFIG_LVGL_TFT_USE_CUSTOM_SPI_CLK_DIVIDER=y
CONFIG_LVGL_TFT_SPI_CLK_DIVIDER_2=y
CONFIG_LVGL_TFT_CUSTOM_SPI_CLK_DIVIDER=2
CONFIG_LVGL_DISP_SPI_MOSI=13
CONFIG_LVGL_DISP_SPI_CLK=14
CONFIG_LVGL_DISP_SPI_CS=15
```

---

## Driver Initialization

The display driver is initialized in `lvgl_helpers.c`:

```c
void lvgl_driver_init(void)
{
    // Initialize SPI bus for display
    lvgl_spi_driver_init(TFT_SPI_HOST,      // HSPI_HOST
                         DISP_SPI_MISO,      // GPIO 12 (or -1)
                         DISP_SPI_MOSI,      // GPIO 13
                         DISP_SPI_CLK,       // GPIO 14
                         SPI_BUS_MAX_TRANSFER_SZ,
                         1,                   // DMA channel
                         -1, -1);            // No QUADWP/QUADHD
    
    // Add display device to SPI bus
    disp_spi_add_device(TFT_SPI_HOST);
    
    // Initialize ILI9341 controller
    disp_driver_init();
    
    // Initialize touch controller
    touch_driver_init();
}
```

---

## Display Buffer Configuration

LVGL uses double buffering for smooth rendering:

```c
// Buffer size calculation (in pixels)
#define DISP_BUF_SIZE  (LV_HOR_RES_MAX * 40)  // 320 * 40 = 12,800 pixels

// In guiTask():
static lv_color_t buf1[DISP_BUF_SIZE];
static lv_color_t buf2[DISP_BUF_SIZE];
static lv_disp_buf_t disp_buf;

lv_disp_buf_init(&disp_buf, buf1, buf2, DISP_BUF_SIZE);
```

---

## Display Driver Registration

```c
lv_disp_drv_t disp_drv;
lv_disp_drv_init(&disp_drv);
disp_drv.flush_cb = disp_driver_flush;  // Callback to send pixels to display
disp_drv.buffer = &disp_buf;
lv_disp_drv_register(&disp_drv);
```

---

## ILI9341 Initialization Sequence

The driver sends these commands during initialization (from `ili9341.c`):

| Command | Description |
|---------|-------------|
| 0xCF | Power Control B |
| 0xED | Power On Sequence Control |
| 0xE8 | Driver Timing Control A |
| 0xCB | Power Control A |
| 0xEA | Driver Timing Control B |
| 0xC0 | Power Control 1 |
| 0xC1 | Power Control 2 |
| 0xC5 | VCOM Control 1 |
| 0xC7 | VCOM Control 2 |
| 0x36 | Memory Access Control (orientation) |
| 0x3A | Pixel Format (16-bit RGB565) |
| 0xB1 | Frame Rate Control |
| 0xB6 | Display Function Control |
| 0x26 | Gamma Set |
| 0xE0 | Positive Gamma Correction |
| 0xE1 | Negative Gamma Correction |
| 0x11 | Sleep Out |
| 0x29 | Display On |

---

## Changing Display Orientation

The ILI9341 supports 4 orientations via the Memory Access Control register (0x36):

```c
// In ili9341.c - ili9341_set_orientation()
static void ili9341_set_orientation(uint8_t orientation)
{
    const char *orientation_str[] = {
        "PORTRAIT", "PORTRAIT_INVERTED", "LANDSCAPE", "LANDSCAPE_INVERTED"
    };
    
    uint8_t data[] = {0x48, 0x88, 0x28, 0xE8};  // MADCTL values
    ili9341_send_cmd(0x36);
    ili9341_send_data(&data[orientation], 1);
}
```

| Orientation | MADCTL | Width | Height |
|-------------|--------|-------|--------|
| PORTRAIT | 0x48 | 240 | 320 |
| PORTRAIT_INVERTED | 0x88 | 240 | 320 |
| LANDSCAPE | 0x28 | 320 | 240 |
| LANDSCAPE_INVERTED | 0xE8 | 320 | 240 |

---

## Performance Notes

- **FPS Range**: 14-25 FPS with ILI9341 @ 240x320
- **SPI Clock**: 40 MHz max, using divider 2 = 20 MHz
- **DMA**: Enabled for asynchronous transfers
- **Double Buffer**: Reduces screen tearing

---

## Supported Display Controllers

The lvgl_esp32_drivers supports multiple controllers:

| Controller | Resolution | Notes |
|------------|------------|-------|
| ILI9341 | 240x320 | **Current configuration** |
| ILI9481 | 320x480 | |
| ILI9486 | 320x480 | |
| ILI9488 | 320x480 | 3-7 FPS |
| ST7789 | 240x320 | |
| ST7735S | 128x160 | |
| SSD1306 | 128x64 | Monochrome OLED |
| SH1107 | 64x128 | Monochrome OLED |

To change controller, update sdkconfig via `idf.py menuconfig`:
```
Component config → LVGL TFT Display controller → Display controller model
```
