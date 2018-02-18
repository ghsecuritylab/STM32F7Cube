################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/projects/STM32F7Cube/Drivers/BSP/Components/ft5336/ft5336.c \
C:/projects/STM32F7Cube/Drivers/BSP/STM32746G-Discovery/stm32746g_discovery.c \
C:/projects/STM32F7Cube/Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_sdram.c \
C:/projects/STM32F7Cube/Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_ts.c 

OBJS += \
./Drivers/ft5336.o \
./Drivers/stm32746g_discovery.o \
./Drivers/stm32746g_discovery_sdram.o \
./Drivers/stm32746g_discovery_ts.o 

C_DEPS += \
./Drivers/ft5336.d \
./Drivers/stm32746g_discovery.d \
./Drivers/stm32746g_discovery_sdram.d \
./Drivers/stm32746g_discovery_ts.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/ft5336.o: C:/projects/STM32F7Cube/Drivers/BSP/Components/ft5336/ft5336.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -std=c11 -DUSE_HAL_DRIVER -DSTM32F746xx -DUSE_STM32746G_DISCO -DUSE_IOEXPANDER -I"C:/projects/STM32F7Cube/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/projects/STM32F7Cube/Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Inc" -I"C:/projects/STM32F7Cube/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/projects/STM32F7Cube/Drivers/BSP/STM32746G-Discovery" -I"C:/projects/STM32F7Cube/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/projects/STM32F7Cube/Drivers/CMSIS/Include" -I"C:/projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/USB_Device/AUDIO_Standalone/Src"  -O3 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Drivers/stm32746g_discovery.o: C:/projects/STM32F7Cube/Drivers/BSP/STM32746G-Discovery/stm32746g_discovery.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -std=c11 -DUSE_HAL_DRIVER -DSTM32F746xx -DUSE_STM32746G_DISCO -DUSE_IOEXPANDER -I"C:/projects/STM32F7Cube/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/projects/STM32F7Cube/Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Inc" -I"C:/projects/STM32F7Cube/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/projects/STM32F7Cube/Drivers/BSP/STM32746G-Discovery" -I"C:/projects/STM32F7Cube/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/projects/STM32F7Cube/Drivers/CMSIS/Include" -I"C:/projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/USB_Device/AUDIO_Standalone/Src"  -O3 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Drivers/stm32746g_discovery_sdram.o: C:/projects/STM32F7Cube/Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_sdram.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -std=c11 -DUSE_HAL_DRIVER -DSTM32F746xx -DUSE_STM32746G_DISCO -DUSE_IOEXPANDER -I"C:/projects/STM32F7Cube/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/projects/STM32F7Cube/Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Inc" -I"C:/projects/STM32F7Cube/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/projects/STM32F7Cube/Drivers/BSP/STM32746G-Discovery" -I"C:/projects/STM32F7Cube/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/projects/STM32F7Cube/Drivers/CMSIS/Include" -I"C:/projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/USB_Device/AUDIO_Standalone/Src"  -O3 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Drivers/stm32746g_discovery_ts.o: C:/projects/STM32F7Cube/Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_ts.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -std=c11 -DUSE_HAL_DRIVER -DSTM32F746xx -DUSE_STM32746G_DISCO -DUSE_IOEXPANDER -I"C:/projects/STM32F7Cube/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/projects/STM32F7Cube/Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Inc" -I"C:/projects/STM32F7Cube/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/projects/STM32F7Cube/Drivers/BSP/STM32746G-Discovery" -I"C:/projects/STM32F7Cube/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/projects/STM32F7Cube/Drivers/CMSIS/Include" -I"C:/projects/STM32F7Cube/Projects/STM32746G-Discovery/Applications/USB_Device/AUDIO_Standalone/Src"  -O3 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


