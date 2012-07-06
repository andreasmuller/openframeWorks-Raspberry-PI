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
	/home/arturo/Descargas/android-ndk-r5b/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/arm-linux-androideabi-gcc -DANDROID -I"/home/arturo/Escritorio/openFrameworks/libs/tess2/include" -Os -g -Wall -c -fmessage-length=0 -nostdlib --sysroot=/home/arturo/Descargas/android-ndk-r5b/platforms/android-8/arch-arm/ -fno-short-enums -I"/home/arturo/Descargas/android-ndk-r5b//platforms/android-8/arch-arm/usr/include" -I"/home/arturo/Descargas/android-ndk-r5b/sources/cxx-stl/gnu-libstdc++/include/" -I"/home/arturo/Descargas/android-ndk-r5b/sources/cxx-stl/gnu-libstdc++/libs/armeabi/include" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


