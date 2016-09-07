################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include example/src/subdir.mk
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
all: misc_lcd_helloworld.axf

# Tool invocations
misc_lcd_helloworld.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\Diego\Documents\LPCXpresso_8.2.0_647\workspace\lpc_chip_43xx\Debug" -L"C:\Users\Diego\Documents\LPCXpresso_8.2.0_647\workspace\lpc_board_nxp_lpcxpresso_4337\Debug" -Xlinker -Map="misc_lcd_helloworld.map" -Xlinker --gc-sections -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -T "misc_lcd_helloworld_Debug.ld" -o "misc_lcd_helloworld.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) misc_lcd_helloworld.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "misc_lcd_helloworld.axf"; # arm-none-eabi-objcopy -v -O binary "misc_lcd_helloworld.axf" "misc_lcd_helloworld.bin" ; # checksum -p LPC4337 -d "misc_lcd_helloworld.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
