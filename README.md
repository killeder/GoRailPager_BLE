# GoRailPager_BLE
> A device for receiving and repeating POCSAG formatted ChinaRail "LBJ" message which has the meaning of train arrival alert. The project is based on TI CC1101 RF solution and STM32F070F6P6 microcontroller. First, the apparatus receive and decode the 2-FSK modulated digital baseband raw data using CC1101. Besides, the programs parse the raw data according to the POCSAG formate regulations. Finally, repeate the LBJ message via Bluetooth 4.0 smart (BLE) interface to data receiving terminals (e.g. mobile phones) for further post processing or storge. This project is the practical and extended version of project "CC1101_Rx_LBJ".(See git@github.com:XieYingnan-Hideo/CC1101_Rx_LBJ.git) For the hardware design, such as schematic diagram and printed circuit board layout of this device, please visit the [repository](https://lceda.cn/Xie-Yingnan/gorailpager_ble) hosted by lceda.cn.

## Basic infomation
- Project Name: GoRailPager_BLE
- Revise version: V1.0
- Created date: 2021-08-20
- Author: Xie Yingnan (xieyingnan1994@163.com)

## Hardware requirements
### 0. CPU
ARM Cortex-M0 STM32F070F6P6 (Package:TSSOP-20)
### 1.TI CC1101 RF solutioin(Operated on STM32F070F6 SPI1)
**fCLK = 6MHz**

| Function | MOSI   | MISO    |SCLK    | CS         | IRQ(GDO2) |
| :------: | :----: | :-----: | :----: | :--------: | :--------:|
| Pin      |  PA7   | PA6   | PA5   | PA4       | PB1       |

### 2.BLE interface module HC-08

|  Function | HC-08 Rx (USART2 Tx) | HC-08 Tx (USART2 Rx) |
| :-------: | :-----:| :--------: |
| Pin       | PA2    | PA3       |

**Pattern: 9600 bps, No parity, 8 bits, 1 stopbits**

### 3.Serial Port(For debug info print)
|  Function |UART1 Tx |  UART1 Rx  |
| :-------: | :-----: | :--------: |
| Pin       | PA9     | PA10       |

**Pattern: 115200 bps, No parity, 8 bits, 1 stopbits**

### 4.Serial-Wire Debug(SWD)
|  Function |  SWDIO  | SWCLK |
| :-------: | :-----: | :--------: |
| Pin       | PA13    | PA14      |

### 5.Indicators (Status LED)

|  Function | Pin on MCU  |
| :------------: | :------------: |
| STATUS_LED | PA1  |

### 6.ADC input for battery voltage check

|  Function | Pin on MCU  | Function |
| :------------: | :------------: | :------------: |
| BATT_CHK | PA0  | ADC input of Vbat/2 |

## Details
### 1. About the Rx procedure and cofiguration of CC1101
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
### 2. About the communication protocals between PC and Microcontroller
>The communication between PC and MCU is done via serialport (parrtern: `115200,N,8,1`), for the ending of each command string, both `'\r'` or `'\n'` as well as `'\r'+'\n'` is OK. Here listed below are the command strobs, note that they are case sensitive. The messages during self-check after power-up as well as POCSAG decoding will also be printed via serialport.

|  Command | Function  | Expected ack from MCU|
| :------------: | :------------: | :------------: |
| $NAME | Show BLE name  | (e.g.)BLE name:LBJ_Pager|
| $NAME=xxx | Set BLE name to "xxx"(max. length is 12)  |OK|
| $BATT | Show battery voltage in mV and level  |(e.g.)Vbat:3750mV,Level2|
| $V | Show firmware version  |version string|
| ? | Show command help  |command help string|

### 3. About the communication protocals between Microcontroller and HC-08 BLE module
>The communication between MCU and BLE module is based on serialport (pattern: `9600,N,8,1`). Before BLE connected to data receiving terminals(e.g. mobile phone), protocals are `AT+something`, as listed below.

|  Command | Function  | Expected ack from HC-08|
| :------------: | :------------: | :------------: |
| $AT | Check HC-08 module presence  | OK|
| $AT+NAME | Get BLE name  | (e.g.)LBJ_Pager|
| $AT+NAME=xxx
 | Set BLE name to "xxx"  | OKsetNAME|

>After BLE connected, we can transmit the raw data what we want the data terminal be received directly through the serial port, for this pattern of data transfer, please view next section for details.

### 4. About the data transfer between HC-08 BLE module and BLE termial (MobilePhone)

|  Content | Function  | Example |  Direction |
| :------------: | :------------: | :------------: | :------------: |
| ${train direction:X,S},{train code},{speed},{miles},{RSSI in dBm},{RSSI level}$ |LBJ alerting message   | `$S,4230,104,168.0,-105.0,2$ $X,47963,45,22.5,-85.5,3$`|HC08 -> Terminal(MobilePhone)|
| #VBAT:{battery voltage in mV},{battery level}# | Report battery voltage and level  | `#VBAT:3756,3#`|HC08 -> Terminal(MobilePhone)|
| #BATT_ALM:{battery voltage in mV}# | Low battery alarm  | `#BATT_ALM:3456#`|HC08 -> Terminal(MobilePhone)|
| #VBAT_CHK# | Check battery voltage  | `#VBAT_CHK#`|Terminal(MobilePhone) -> HC08|


------------

End of file (Edited by XieYingnan)
