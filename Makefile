# ------------------------------------------------
# Generic Cross-Compile Makefile
# (based on arm-none-eabi-gcc)
# ChangeLog :
#   2021-06-28 - first version
#	2021-08-20 - Change MCU from STM32F030K6 to STM32F070F6
# Author : Xie Yingnan (xieyingnan1994@163.com)
# ------------------------------------------------

######################################
# target
######################################
TARGET = GoRailPager_BLE


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og


#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
	CMSIS/system_stm32f0xx.c \
	App_User/main.c \
	App_User/Transfer_Utils.c \
	App_User/Radio_Utils.c \
	App_User/Timebase_Utils.c \
	App_User/Battery_Utils.c \
	Sys_Utils/delay.c \
	Sys_Utils/POCSAG_ParseLBJ.c \
	Hardware/HW_LED.c \
	Hardware/HW_RADIO_CC1101.c \
	Hardware/HW_TIM_Timebase.c \
	Hardware/HW_USART.c \
	Hardware/HW_USART_DMA.c \
	Hardware/HW_ADConvert.c \
	FirmwareLibs/stm32f0xx_exti.c \
	FirmwareLibs/stm32f0xx_gpio.c \
	FirmwareLibs/stm32f0xx_misc.c \
	FirmwareLibs/stm32f0xx_rcc.c \
	FirmwareLibs/stm32f0xx_spi.c \
	FirmwareLibs/stm32f0xx_syscfg.c \
	FirmwareLibs/stm32f0xx_tim.c \
	FirmwareLibs/stm32f0xx_usart.c \

# ASM sources
ASM_SOURCES =  \
CMSIS/startup_stm32f070x6.s \

#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m0

# fpu
# NONE for Cortex-M0/M0+/M3

# float-abi


# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_STDPERIPH_DRIVER \
-DSTM32F070x6 \
-DPOCSAG_DEBUG_MSG_ON


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-ICMSIS/ \
-IApp_User/ \
-ISys_Utils/ \
-IHardware/ \
-IFirmwareLibs/ \

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F070x6_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@
	
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
	rm -rf $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***