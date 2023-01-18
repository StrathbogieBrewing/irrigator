cmake_minimum_required(VERSION 2.8)

set(AVR_MCU_TYPE atmega8)
set(AVR_H_FUSE 0xDA)
set(AVR_L_FUSE 0xFF)
# set(AVR_EX_FUSE 0×05)
set(AVR_MCU_SPEED "1000000UL")

### printf support
#set(EXTRA_FLAGS "-Wl,-u,vfprintf -lprintf_min") 

set(AVR_UPLOADTOOL avrdude)
set(AVR_PROGRAMMER usbasp)
#set(AVR_PROGRAMMER arduino)
set(AVR_UPLOADTOOL_PORT usb)
#set(AVR_UPLOADTOOL_PORT /dev/ttyACM0)
#set(AVR_UPLOADTOOL_BITRATE 57600)
set(AVR_UPLOADTOOL_BIT_RATE 4)

INCLUDE(gcc-avr.cmake)

project(irrigator C ASM)

include(FetchContent)
FetchContent_Declare(
   cobsm
   GIT_REPOSITORY https://github.com/StrathbogieBrewing/cobsm.git
   GIT_TAG        main
)
FetchContent_MakeAvailable(cobsm)
include_directories(${cobsm_SOURCE_DIR}/include)



add_avr_executable(
   ${PROJECT_NAME}
   src/main.c
   ${cobsm_SOURCE_DIR}/src/cobsm.c
)

# avr_target_include_directories(
#    ${PROJECT_NAME}
#    ${cobsm_SOURCE_DIR}/include
# )



