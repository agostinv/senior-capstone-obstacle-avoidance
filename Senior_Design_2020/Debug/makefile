################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/VL53L0X_HAL_Compatible_API/src/platform/src/subdir.mk
-include Drivers/VL53L0X_HAL_Compatible_API/src/core/src/subdir.mk
-include Drivers/VL53L0X_HAL_Compatible_API/src/subdir.mk
-include Drivers/STM32H7xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Senior_Design_2020.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Senior_Design_2020.list \


# All Target
all: Senior_Design_2020.elf secondary-outputs

# Tool invocations
Senior_Design_2020.elf: $(OBJS) $(USER_OBJS) C:\Users\victo\STM32CubeIDE\workspace_1.1.0\Senior_Design_2020\STM32H743VITX_FLASH.ld
	arm-none-eabi-g++ -o "Senior_Design_2020.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m7 -T"C:\Users\victo\STM32CubeIDE\workspace_1.1.0\Senior_Design_2020\STM32H743VITX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Senior_Design_2020.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Senior_Design_2020.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Senior_Design_2020.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
