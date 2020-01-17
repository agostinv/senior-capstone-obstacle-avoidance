################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LIBJPEG/App/libjpeg.c 

OBJS += \
./LIBJPEG/App/libjpeg.o 

C_DEPS += \
./LIBJPEG/App/libjpeg.d 


# Each subdirectory must supply rules for building sources it contributes
LIBJPEG/App/libjpeg.o: ../LIBJPEG/App/libjpeg.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../LIBJPEG/Target -I../FATFS/App -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/LibJPEG/include -I../Drivers/CMSIS/Include -I../Utilities/JPEG -I../LIBJPEG/App -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../FATFS/Target -I../Middlewares/Third_Party/FatFs/src -I../Drivers/STM32H7xx_HAL_Driver/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"LIBJPEG/App/libjpeg.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

