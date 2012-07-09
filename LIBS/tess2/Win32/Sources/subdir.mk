################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/bucketalloc.c \
../Sources/dict.c \
../Sources/geom.c \
../Sources/mesh.c \
../Sources/priorityq.c \
../Sources/sweep.c \
../Sources/tess.c 

OBJS += \
./Sources/bucketalloc.o \
./Sources/dict.o \
./Sources/geom.o \
./Sources/mesh.o \
./Sources/priorityq.o \
./Sources/sweep.o \
./Sources/tess.o 

C_DEPS += \
./Sources/bucketalloc.d \
./Sources/dict.d \
./Sources/geom.d \
./Sources/mesh.d \
./Sources/priorityq.d \
./Sources/sweep.d \
./Sources/tess.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	i586-mingw32msvc-gcc -I"/home/arturo/Escritorio/openFrameworks/libs/tess2/include" -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


