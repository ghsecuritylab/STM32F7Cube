################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
C://projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/USB_Device/AUDIO_Standalone/SW4STM32/startup_stm32f746xx.s 

C_SRCS += \
C:/projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/stm32746g_audio.c \
C:/projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/system.c \
C://projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/wm8994.c 

CPP_SRCS += \
C:/projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/USB_Device/AUDIO_Standalone/Src/main.cpp \
C:/projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/stm32746g_lcd.cpp \
C:/projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/utils.cpp 

OBJS += \
./app/main.o \
./app/startup_stm32f746xx.o \
./app/stm32746g_audio.o \
./app/stm32746g_lcd.o \
./app/system.o \
./app/utils.o \
./app/wm8994.o 

C_DEPS += \
./app/stm32746g_audio.d \
./app/system.d \
./app/wm8994.d 

CPP_DEPS += \
./app/main.d \
./app/stm32746g_lcd.d \
./app/utils.d 


# Each subdirectory must supply rules for building sources it contributes
app/main.o: C:/projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/USB_Device/AUDIO_Standalone/Src/main.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU G++ Compiler'
	@echo $(PWD)
	arm-none-eabi-g++ -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -std=c++0x -DUSE_HAL_DRIVER -DSTM32F746xx -DUSE_STM32746G_DISCO -I"C:/projects/STM32F7Cube/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/projects/STM32F7Cube/Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Inc" -I"C:/projects/STM32F7Cube/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/projects/STM32F7Cube/Drivers/BSP/STM32746G-Discovery" -I"C:/projects/STM32F7Cube/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/projects/STM32F7Cube/Drivers/CMSIS/Include" -I"C:/projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/USB_Device/AUDIO_Standalone/Src"  -O2 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

app/startup_stm32f746xx.o: C://projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/USB_Device/AUDIO_Standalone/SW4STM32/startup_stm32f746xx.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

app/stm32746g_audio.o: C:/projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/stm32746g_audio.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -std=c11 -DUSE_HAL_DRIVER -DSTM32F746xx -DUSE_STM32746G_DISCO -I"C:/projects/STM32F7Cube/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/projects/STM32F7Cube/Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Inc" -I"C:/projects/STM32F7Cube/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/projects/STM32F7Cube/Drivers/BSP/STM32746G-Discovery" -I"C:/projects/STM32F7Cube/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/projects/STM32F7Cube/Drivers/CMSIS/Include" -I"C:/projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/USB_Device/AUDIO_Standalone/Src"  -O2 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

app/stm32746g_lcd.o: C:/projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/stm32746g_lcd.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU G++ Compiler'
	@echo $(PWD)
	arm-none-eabi-g++ -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -std=c++0x -DUSE_HAL_DRIVER -DSTM32F746xx -DUSE_STM32746G_DISCO -I"C:/projects/STM32F7Cube/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/projects/STM32F7Cube/Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Inc" -I"C:/projects/STM32F7Cube/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/projects/STM32F7Cube/Drivers/BSP/STM32746G-Discovery" -I"C:/projects/STM32F7Cube/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/projects/STM32F7Cube/Drivers/CMSIS/Include" -I"C:/projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/USB_Device/AUDIO_Standalone/Src"  -O2 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

app/system.o: C:/projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/system.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -std=c11 -DUSE_HAL_DRIVER -DSTM32F746xx -DUSE_STM32746G_DISCO -I"C:/projects/STM32F7Cube/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/projects/STM32F7Cube/Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Inc" -I"C:/projects/STM32F7Cube/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/projects/STM32F7Cube/Drivers/BSP/STM32746G-Discovery" -I"C:/projects/STM32F7Cube/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/projects/STM32F7Cube/Drivers/CMSIS/Include" -I"C:/projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/USB_Device/AUDIO_Standalone/Src"  -O2 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

app/utils.o: C:/projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/utils.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU G++ Compiler'
	@echo $(PWD)
	arm-none-eabi-g++ -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -std=c++0x -DUSE_HAL_DRIVER -DSTM32F746xx -DUSE_STM32746G_DISCO -I"C:/projects/STM32F7Cube/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/projects/STM32F7Cube/Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Inc" -I"C:/projects/STM32F7Cube/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/projects/STM32F7Cube/Drivers/BSP/STM32746G-Discovery" -I"C:/projects/STM32F7Cube/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/projects/STM32F7Cube/Drivers/CMSIS/Include" -I"C:/projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/USB_Device/AUDIO_Standalone/Src"  -O2 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

app/wm8994.o: C://projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/wm8994.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -std=c11 -DUSE_HAL_DRIVER -DSTM32F746xx -DUSE_STM32746G_DISCO -I"C:/projects/STM32F7Cube/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/projects/STM32F7Cube/Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Inc" -I"C:/projects/STM32F7Cube/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/projects/STM32F7Cube/Drivers/BSP/STM32746G-Discovery" -I"C:/projects/STM32F7Cube/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/projects/STM32F7Cube/Drivers/CMSIS/Include" -I"C:/projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/USB_Device/AUDIO_Standalone/Src"  -O2 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


