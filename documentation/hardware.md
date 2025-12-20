# Hardware & Pinout Reference

## ESP32 Module Overview

This board uses an ESP32-WROOM module with the following key specifications:
- **CPU**: Dual-core Xtensa LX6 @ 240 MHz
- **RAM**: 520 KB SRAM
- **Flash**: External SPI Flash
- **GPIO**: 40 pins total, see mappings below

---

## SPI Bus Configuration

The board uses two SPI buses for different peripherals:

### HSPI (SPI2_HOST = 1) - Display Only
| Signal | GPIO | Function |
|--------|------|----------|
| CS     | 15   | Chip Select |
| SCLK   | 14   | Serial Clock |
| MISO   | 12   | Master In Slave Out (optional) |
| MOSI   | 13   | Master Out Slave In |
| DC     | 2    | Data/Command Select |
| Reset  | 4    | Hardware Reset |

### VSPI (SPI3_HOST = 2) - SD Card, Flash, Ethernet
| Signal | GPIO | Function |
|--------|------|----------|
| CS0    | 5    | Default Chip Select |
| SCLK   | 18   | Serial Clock |
| MISO   | 19   | Master In Slave Out |
| MOSI   | 23   | Master Out Slave In |
| QUADWP | 22   | Quad SPI Write Protect |
| QUADHD | 21   | Quad SPI Hold |

---

## Touch Controller Pins (XPT2046 - GPIO Bit-Bang)

The touch controller uses GPIO bit-banging, NOT hardware SPI:

| Signal | GPIO | Notes |
|--------|------|-------|
| MISO   | 39   | SENSOR_VN - Input only |
| MOSI   | 32   | Output |
| CLK    | 25   | Output |
| CS     | 33   | Chip Select |
| IRQ    | 36   | SENSOR_VP - Input only, touch interrupt |

**Important**: GPIO 36 and 39 are input-only pins on ESP32.

---

## Complete Module Pinout

### Left Side (Pins 2-14)
| Function | ESP32 Pin | GPIO | Notes |
|----------|-----------|------|-------|
| 3V3      | 3V3       | -    | Power supply |
| EN       | EN        | -    | Reset button (active low) |
| TP_IRQ   | SENSOR_VP | 36   | Touch interrupt, ADC1_CH0, input only |
| TP_MISO  | SENSOR_VN | 39   | Touch data out, ADC1_CH3, input only |
| RLight   | IO34      | 34   | Light sensor, ADC1_CH6, input only |
| IRRev    | IO35      | 35   | IR receiver, ADC1_CH7, input only |
| TP_MOSI  | IO32      | 32   | Touch data in, ADC1_CH4, TOUCH9 |
| TP_CS    | IO33      | 33   | Touch chip select, ADC1_CH5, TOUCH8 |
| TP_CLK   | IO25      | 25   | Touch clock, DAC_1, ADC2_CH8 |
| DHT11    | IO26      | 26   | Temperature sensor, DAC_2, ADC2_CH9 |
| TouchPad | IO27      | 27   | Capacitive touch, ADC2_CH7, TOUCH7 |
| TFT_CLK  | IO14      | 14   | Display SPI clock, HSPICLK |
| TFT_MISO | IO12      | 12   | Display SPI MISO, HSPIQ |

### Bottom Side (Pins 15-24)
| Function | ESP32 Pin | GPIO | Notes |
|----------|-----------|------|-------|
| GND      | GND       | -    | Ground |
| TFT_MOSI | IO13      | 13   | Display SPI MOSI, HSPID |
| SD2      | SD2       | -    | NC (Not Connected) |
| SD3      | SD3       | -    | NC |
| CMD      | CMD       | -    | NC |
| CLK      | CLK       | -    | NC |
| SD0      | SD0       | -    | NC |
| SD1      | SD1       | -    | NC |
| TFT_CS   | IO15      | 15   | Display chip select, HSPICS0 |
| TFT_DC   | IO2       | 2    | Display data/command, HSPIWP |

### Right Side (Pins 25-38)
| Function | ESP32 Pin | GPIO | Notes |
|----------|-----------|------|-------|
| Flash    | IO0       | 0    | Boot mode select button |
| TFT_Reset| IO4       | 4    | Display reset, HSPIHD |
| RGBLight | IO16      | 16   | RGB LED control, U2RXD |
| IRSend   | IO17      | 17   | IR transmitter, U2TXD |
| SD_CS    | IO5       | 5    | SD card chip select, VSPICS0 |
| SD_CLK   | IO18      | 18   | SD card clock, VSPICLK |
| SD_MISO  | IO19      | 19   | SD card MISO, VSPIQ |
| NC       | NC        | -    | Not Connected |
| IIC_SCL  | IO21      | 21   | I2C clock, VSPIHD |
| RXD0     | RXD0      | 3    | UART0 RX (programming) |
| TXD0     | TXD0      | 1    | UART0 TX (programming) |
| IIC_SDA  | IO22      | 22   | I2C data, VSPIWP |
| SD_MOSI  | IO23      | 23   | SD card MOSI, VSPID |
| GND      | GND       | -    | Ground |

---

## Peripheral Chip Selects

| Peripheral | CS GPIO | SPI Bus |
|------------|---------|---------|
| TFT Display | 15 | HSPI |
| XPT2046 Touch | 33 | GPIO bit-bang |
| SD Card | 5 | VSPI |
| SPI Flash | 21 | VSPI |
| ENC28J60 Ethernet | 5 | VSPI (shared with SD) |

---

## Special Pin Considerations

### Input-Only Pins
- GPIO 34, 35, 36, 39 are **input-only** and cannot be used as outputs
- These pins lack internal pull-up/pull-down resistors

### Strapping Pins
- GPIO 0: Boot mode (LOW = download, HIGH = normal boot)
- GPIO 2: Must be LOW or floating during boot
- GPIO 15: Must be HIGH during boot (has internal pull-up)

### ADC Limitations
- ADC2 cannot be used when WiFi is active
- For analog input with WiFi, use ADC1 (GPIOs 32-39)

---

## I2C Bus

| Signal | GPIO | Notes |
|--------|------|-------|
| SDA    | 22   | I2C Data |
| SCL    | 21   | I2C Clock |

Default I2C speed: 100 kHz (configurable up to 400 kHz)
