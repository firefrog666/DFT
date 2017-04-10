################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Graph.cpp \
../src/ILP.cpp \
../src/ILPgen.cpp \
../src/dateJournal_goodluck.cpp \
../src/testbench.cpp \
../src/toolBox.cpp \
../src/writeILP.cpp 

OBJS += \
./src/Graph.o \
./src/ILP.o \
./src/ILPgen.o \
./src/dateJournal_goodluck.o \
./src/testbench.o \
./src/toolBox.o \
./src/writeILP.o 

CPP_DEPS += \
./src/Graph.d \
./src/ILP.d \
./src/ILPgen.d \
./src/dateJournal_goodluck.d \
./src/testbench.d \
./src/toolBox.d \
./src/writeILP.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


