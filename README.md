# GoRailPager_7SegLED
> A device for receiving and showing POCSAG formatted ChinaRail "LBJ" message which has the meaning of train arrival alert. The project is based on TI CC1101 RF solution and STM32F030K6T6 microcontroller. It is the socalled "Lite" & “Retro” version of project "GoRail_Pager", the MQTT publishing and SDCard logging function is not implemented, while the MCU is changed from STM32F103C8 to STM32F030K6 and screen is changed from SSD1306 OLED to 2×8 digits 7-Seg LED driven by TM1640. First, the apparatus receive and decode the 2-FSK modulated digital baseband raw data using CC1101. Besides, the programs parse the raw data according to the POCSAG formate regulations. Finally, show the LBJ message on 7-Seg LED screens and do some post processes. This project is the practical and extended version of project "CC1101_Rx_LBJ".
(See git@github.com:XieYingnan-Hideo/CC1101_Rx_LBJ.git)

## Basic infomation
- Project Name: GoRailPager_7SegLED
- Revise version: V1.1
- Created date: 2021-02-12
- Author: Xie Yingnan (xieyingnan1994@163.com)

## What's new in version 1.1
- LED driver is changed from 74HC595 to TM1640 to enhance brightness
- Added a button to set brightness level of LED displayer

## Hardware requirements
### 0. CPU
ARM Cortex-M0 STM32F030K6T6
### 1.TI CC1101 RF solutioin(Operated on STM32F030 SPI1)
**fCLK = 6MHz**

| Function | MOSI   | MISO    |SCLK    | CS         | IRQ(GDO2) |
| :------: | :----: | :-----: | :----: | :--------: | :--------:|
| Pin      |  PA7   | PA6   | PA5   | PA4       | PB0       |

### 2.LED 7-Seg Displayer Driven By TM1640

|  Function | TM1640 Clock | TM1640 Data |
| :-------: | :-----:| :--------: |
| Pin       | PB4    | PB3       |

### 3.Serial Port(For debug info print)
|  Function |UART1 Tx |  UART1 Rx  |
| :-------: | :-----: | :--------: |
| Pin       | PA9     | PA10       |

**Pattern: 115200 bps, No parity, 8 bits, 1 stopbits**

### 4.Serial-Wire Debug(SWD)
|  Function |  SWDIO  | SWCLK |
| :-------: | :-----: | :--------: |
| Pin       | PA13    | PA14      |

### 5.Indicators(two LED, Buzzer)

|  Function | Pin on MCU  |
| :------------: | :------------: |
| STATUS | PB5  |
| DECODE | PB6  |
| Buzzer | PB7  |

### 6.Function Key

|  Function | Pin on MCU  | Function |
| :------------: | :------------: | :------------: |
| CONFIG_KEY | PA0  | Toggle beeper |
| MODE_KEY | PA1  | Set LED 7-seg displayer brightness level |

**When key is on, pin of mcu is connected to Vcc.

## Details
### 1.About the Rx procedure and cofiguration of CC1101
The train arrival alerting message is transmitted by locomotive on 821.2375MHz using 2-FSK modulated POCSAG formate. The FSK frequency deviation direction of LBJ systems on locomotive is obeying POCSAG standard. i.e. F+f signifies logical 0, and F-f signifies logical 1. It is exactly **reversed** to the FSK pattern that CC1101 holds. CC1101 regarding F+f as logical 1 and F-f as logical 0. So we need to do **bit reverse** operate for receiving LBJ message correctly. The implement method is: change sync code from `0x15D8` to its bit reversed value `0xEA27` and make raw data **bit reversed** before POCSAG parsing.
> Notes: value `0x15D8` is the lower 16bits of standard POCSAG sync code 
> `0x7CD215D8`, for CC1101 does not support 32-bit sync code, we only intersect its lower 16 bits.

According to the standard `TB/T3504-2018`, LBJ systems on locomotive publish two kinds of message on two POCSAG address: `1234000` and `1234008`. The former POCSAG message contains traincode, train speed and milesmark that the train just passed. The latter POCSAG message just for time sync utility. The structure of two messages are listed as following chart.

| Train arrival alert @ 1234000  |
| :------------: |
| 576bits preamble+32bit sync(0x7CD215D8)+1 Address codeword(4 bytes)+3 Message Codeword(12 bytes)+3 IDLE Codeword(12 bytes) |

| Train arrival alert @ 1234008  |
| :------------: |
| 576bits preamble+32bit sync(0x7CD215D8)+1 Address codeword(4 bytes)+1 Message Codeword(4 bytes)+5 IDLE Codeword(20 bytes) |

We can summerize that we only need to receive bytes size of 16(containing 1 Address codeword and 3 Message codeword), the packet size of CC1101 is set to 16, with fixed length mode.

| CC1101 receiving package pattern  |
| :------------: |
| 16 bytes of preamble+16bit sync code (0xEA27)+16bytes of payload(containing 1 address codeword and 3 message codeword) |

CC1101 shoud be configured an follow:

|  CRC Filtering | Package mode  |  Sync word |  Preamble length |  FSK Freq deviation |
| :------------: | :------------: | :------------: | :------------: | :------------: |
|  OFF |  Fixed length, 16bytes |  0xEA27 | 16 bytes  |  4.5kHz |

------------

End of file (Edited by XieYingnan)



