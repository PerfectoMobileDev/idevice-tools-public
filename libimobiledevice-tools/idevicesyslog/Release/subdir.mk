################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../idevicesyslog.cpp 

OBJS += \
./idevicesyslog.o 

CPP_DEPS += \
./idevicesyslog.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -DHAVE_OPENSSL -D__LITTLE_ENDIAN__ -Dplist_STATIC -DVERSION="$(HSS_VERSION)" -I"../../../pmasserver" -I"../../../libplist/include" -I"../../../libimobiledevice/include" -I"../../../vendors/socket" -I"../../../ideviceutils" -I"../../../libusbmuxd" -O3 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


