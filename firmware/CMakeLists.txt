cmake_minimum_required(VERSION 3.22)
project(WaterLevelSensor_STM32 C ASM)
set(MCU_FLAGS "-mcpu=cortex-m0plus -mthumb")
set(CMAKE_C_FLAGS "${MCU_FLAGS} -Wall -Os -ffunction-sections -fdata-sections -std=gnu11")
set(CMAKE_ASM_FLAGS "${MCU_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${MCU_FLAGS} -T${CMAKE_SOURCE_DIR}/Linker/STM32G071KBTx_FLASH.ld -Wl,--gc-sections -Wl,-Map=${PROJECT_NAME}.map --specs=nano.specs --specs=nosys.specs -lc -lm")
add_definitions(-DSTM32G071xx)
include_directories(Core/Inc)
file(GLOB_RECURSE SOURCES "Core/Src/*.c" "Startup/*.s")
add_executable(${PROJECT_NAME}.elf ${SOURCES})
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND arm-none-eabi-objcopy -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND arm-none-eabi-size $<TARGET_FILE:${PROJECT_NAME}.elf>)