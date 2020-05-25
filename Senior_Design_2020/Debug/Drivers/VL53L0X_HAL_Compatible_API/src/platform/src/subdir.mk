################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Drivers/VL53L0X_HAL_Compatible_API/src/platform/src/vl53l0x_i2c_comms.cpp \
../Drivers/VL53L0X_HAL_Compatible_API/src/platform/src/vl53l0x_platform.cpp 

OBJS += \
./Drivers/VL53L0X_HAL_Compatible_API/src/platform/src/vl53l0x_i2c_comms.o \
./Drivers/VL53L0X_HAL_Compatible_API/src/platform/src/vl53l0x_platform.o 

CPP_DEPS += \
./Drivers/VL53L0X_HAL_Compatible_API/src/platform/src/vl53l0x_i2c_comms.d \
./Drivers/VL53L0X_HAL_Compatible_API/src/platform/src/vl53l0x_platform.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/VL53L0X_HAL_Compatible_API/src/platform/src/vl53l0x_i2c_comms.o: ../Drivers/VL53L0X_HAL_Compatible_API/src/platform/src/vl53l0x_i2c_comms.cpp
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Drivers/VL53L0X_HAL_Compatible_API/src -I../Drivers/CMSIS/Include -I../Utilities/JPEG -I../Drivers/VL53L0X_HAL_Compatible_API/src/core -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/VL53L0X_HAL_Compatible_API/src/platform -I../Drivers/STM32H7xx_HAL_Driver/Inc -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"Drivers/VL53L0X_HAL_Compatible_API/src/platform/src/vl53l0x_i2c_comms.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/VL53L0X_HAL_Compatible_API/src/platform/src/vl53l0x_platform.o: ../Drivers/VL53L0X_HAL_Compatible_API/src/platform/src/vl53l0x_platform.cpp
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Drivers/VL53L0X_HAL_Compatible_API/src -I../Drivers/CMSIS/Include -I../Utilities/JPEG -I../Drivers/VL53L0X_HAL_Compatible_API/src/core -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/VL53L0X_HAL_Compatible_API/src/platform -I../Drivers/STM32H7xx_HAL_Driver/Inc -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"Drivers/VL53L0X_HAL_Compatible_API/src/platform/src/vl53l0x_platform.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

