################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Engine.c \
../src/LISAreceiver.c \
../src/LISAtransmitter.c \
../src/cr_startup_lpc175x_6x.c \
../src/crp.c \
../src/queue.c \
../src/scrambler.c \
../src/sysinit.c 

OBJS += \
./src/Engine.o \
./src/LISAreceiver.o \
./src/LISAtransmitter.o \
./src/cr_startup_lpc175x_6x.o \
./src/crp.o \
./src/queue.o \
./src/scrambler.o \
./src/sysinit.o 

C_DEPS += \
./src/Engine.d \
./src/LISAreceiver.d \
./src/LISAtransmitter.d \
./src/cr_startup_lpc175x_6x.d \
./src/crp.d \
./src/queue.d \
./src/scrambler.d \
./src/sysinit.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M3 -D__USE_LPCOPEN -D__USE_CMSIS_DSPLIB=CMSIS_DSPLIB_CM3 -D__LPC17XX__ -I"/Users/dev/Documents/LPCXpresso_8.2.0/workspace/lpc_board_nxp_lpcxpresso_1769/inc" -I"/Users/dev/Documents/LPCXpresso_8.2.0/workspace/lpc_chip_175x_6x/inc" -I"/Users/dev/Documents/LPCXpresso_8.2.0/workspace/CMSIS_DSPLIB_CM3/inc" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


