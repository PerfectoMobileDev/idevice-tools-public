################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../common/debug.c \
../common/socket.c \
../common/thread.c \
../common/userpref.c \
../common/utils.c 

OBJS += \
./common/debug.o \
./common/socket.o \
./common/thread.o \
./common/userpref.o \
./common/utils.o 

C_DEPS += \
./common/debug.d \
./common/socket.d \
./common/thread.d \
./common/userpref.d \
./common/utils.d 


# Each subdirectory must supply rules for building sources it contributes
common/%.o: ../common/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DHAVE_OPENSSL -D__LITTLE_ENDIAN__ -D__MICKEL_MERGE -DHAVE_VASPRINTF -DHAVE_ASPRINTF -I"../common" -I/usr/include/x86_64-linux-gnu -I"../../libusbmuxd" -I../ -I/usr/include/openssl -I"../../libplist/include" -I"../include" -O3 -Wall -c -fmessage-length=0 -fgnu89-inline -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


