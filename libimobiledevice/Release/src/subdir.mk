################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/afc.c \
../src/debugserver.c \
../src/device_link_service.c \
../src/diagnostics_relay.c \
../src/file_relay.c \
../src/heartbeat.c \
../src/house_arrest.c \
../src/idevice.c \
../src/installation_proxy.c \
../src/lockdown.c \
../src/misagent.c \
../src/mobile_image_mounter.c \
../src/mobilebackup.c \
../src/mobilebackup2.c \
../src/mobilesync.c \
../src/notification_proxy.c \
../src/property_list_service.c \
../src/restore.c \
../src/sbservices.c \
../src/screenshotr.c \
../src/service.c \
../src/syslog_relay.c \
../src/webinspector.c 

OBJS += \
./src/afc.o \
./src/debugserver.o \
./src/device_link_service.o \
./src/diagnostics_relay.o \
./src/file_relay.o \
./src/heartbeat.o \
./src/house_arrest.o \
./src/idevice.o \
./src/installation_proxy.o \
./src/lockdown.o \
./src/misagent.o \
./src/mobile_image_mounter.o \
./src/mobilebackup.o \
./src/mobilebackup2.o \
./src/mobilesync.o \
./src/notification_proxy.o \
./src/property_list_service.o \
./src/restore.o \
./src/sbservices.o \
./src/screenshotr.o \
./src/service.o \
./src/syslog_relay.o \
./src/webinspector.o 

C_DEPS += \
./src/afc.d \
./src/debugserver.d \
./src/device_link_service.d \
./src/diagnostics_relay.d \
./src/file_relay.d \
./src/heartbeat.d \
./src/house_arrest.d \
./src/idevice.d \
./src/installation_proxy.d \
./src/lockdown.d \
./src/misagent.d \
./src/mobile_image_mounter.d \
./src/mobilebackup.d \
./src/mobilebackup2.d \
./src/mobilesync.d \
./src/notification_proxy.d \
./src/property_list_service.d \
./src/restore.d \
./src/sbservices.d \
./src/screenshotr.d \
./src/service.d \
./src/syslog_relay.d \
./src/webinspector.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DHAVE_OPENSSL -D__LITTLE_ENDIAN__ -D__MICKEL_MERGE -DHAVE_VASPRINTF -DHAVE_ASPRINTF -I"../common" -I/usr/include/x86_64-linux-gnu -I"../../libusbmuxd" -I../ -I/usr/include/openssl -I"../../libplist/include" -I"../include" -O3 -Wall -c -fmessage-length=0 -fgnu89-inline -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


