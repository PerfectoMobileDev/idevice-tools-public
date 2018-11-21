################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../command.cpp \
../idevice_installer_server.cpp \
../installer_utils.cpp 

OBJS += \
./command.o \
./idevice_installer_server.o \
./installer_utils.o 

CPP_DEPS += \
./command.d \
./idevice_installer_server.d \
./installer_utils.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -DHAVE_LIBIMOBILEDEVICE_1_1 -DHAVE_LIBIMOBILEDEVICE_1_1_5 -I"../../../pmasserver" -I"../../../dumputils" -I"../../../vendors/socket" -I"../../../libzip" -I"../../../libplist/include" -I"../../../libimobiledevice/include" -I"../../../ideviceutils" -O3 -g -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


