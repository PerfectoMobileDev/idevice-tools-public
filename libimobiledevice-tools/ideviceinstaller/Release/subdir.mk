################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../ideviceinstaller.cpp 

OBJS += \
./ideviceinstaller.o 

CPP_DEPS += \
./ideviceinstaller.d 


# Each subdirectory must supply rules for building sources it contributes
ideviceinstaller.o: ../ideviceinstaller.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++0x -DHAVE_LIBIMOBILEDEVICE_1_1 -DHAVE_LIBIMOBILEDEVICE_1_1_5 -I"../../../pmasserver" -I"../../../ideviceutils" -I"../../../dumputils" -I"../../../vendors/socket" -I"../../../libzip" -I"../../../libplist/include" -I"../../../libimobiledevice/include" -O3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"ideviceinstaller.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


