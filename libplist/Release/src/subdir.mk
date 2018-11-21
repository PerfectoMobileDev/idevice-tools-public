################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/base64.c \
../src/bplist.c \
../src/bytearray.c \
../src/hashtable.c \
../src/plist.c \
../src/ptrarray.c \
../src/xplist.c 

CPP_SRCS += \
../src/Array.cpp \
../src/Boolean.cpp \
../src/Data.cpp \
../src/Date.cpp \
../src/Dictionary.cpp \
../src/Integer.cpp \
../src/Key.cpp \
../src/Node.cpp \
../src/Real.cpp \
../src/String.cpp \
../src/Structure.cpp \
../src/Uid.cpp 

OBJS += \
./src/Array.o \
./src/Boolean.o \
./src/Data.o \
./src/Date.o \
./src/Dictionary.o \
./src/Integer.o \
./src/Key.o \
./src/Node.o \
./src/Real.o \
./src/String.o \
./src/Structure.o \
./src/Uid.o \
./src/base64.o \
./src/bplist.o \
./src/bytearray.o \
./src/hashtable.o \
./src/plist.o \
./src/ptrarray.o \
./src/xplist.o 

C_DEPS += \
./src/base64.d \
./src/bplist.d \
./src/bytearray.d \
./src/hashtable.d \
./src/plist.d \
./src/ptrarray.d \
./src/xplist.d 

CPP_DEPS += \
./src/Array.d \
./src/Boolean.d \
./src/Data.d \
./src/Date.d \
./src/Dictionary.d \
./src/Integer.d \
./src/Key.d \
./src/Node.d \
./src/Real.d \
./src/String.d \
./src/Structure.d \
./src/Uid.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I"../include" -I"../../libcnary/include" -I/usr/include/libxml2 -I"../src" -O3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I"../include" -I/usr/include/libxml2 -I"../../libcnary/include" -I/usr/include/x86_64-linux-gnu -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


