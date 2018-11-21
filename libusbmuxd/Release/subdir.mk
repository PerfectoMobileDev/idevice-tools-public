################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../sock_stuff.c \
../utils.c 

CPP_SRCS += \
../libusbmuxd.cpp 

OBJS += \
./libusbmuxd.o \
./sock_stuff.o \
./utils.o 

C_DEPS += \
./sock_stuff.d \
./utils.d 

CPP_DEPS += \
./libusbmuxd.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++0x -DHAVE_INOTIFY -DHAVE_PLIST -I"../../libplist/include" -O3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DHAVE_INOTIFY -DHAVE_PLIST -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


