################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Engine.axf

# Tool invocations
Engine.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"/Users/dev/Documents/LPCXpresso_8.2.0/workspace/lpc_board_nxp_lpcxpresso_1769/Debug" -L"/Users/dev/Documents/LPCXpresso_8.2.0/workspace/lpc_chip_175x_6x/Debug" -L"/Users/dev/Documents/LPCXpresso_8.2.0/workspace/CMSIS_DSPLIB_CM3/lib" -Xlinker -Map="Engine.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m3 -mthumb -T "Engine_Debug.ld" -o "Engine.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) Engine.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "Engine.axf"; # arm-none-eabi-objcopy -v -O binary "Engine.axf" "Engine.bin" ; # checksum -p LPC1769 -d "Engine.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
