################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../cnary.c \
../iterator.c \
../list.c \
../node.c \
../node_iterator.c \
../node_list.c 

OBJS += \
./cnary.o \
./iterator.o \
./list.o \
./node.o \
./node_iterator.o \
./node_list.o 

C_DEPS += \
./cnary.d \
./iterator.d \
./list.d \
./node.d \
./node_iterator.d \
./node_list.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"../include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


