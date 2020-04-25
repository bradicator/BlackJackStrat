################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_UPPER_SRCS += \
../blackjack.C \
../blackjackmain.C 

OBJS += \
./blackjack.o \
./blackjackmain.o 

C_UPPER_DEPS += \
./blackjack.d \
./blackjackmain.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.C
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


