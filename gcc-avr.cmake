
find_program(AVR_CC avr-gcc REQUIRED)
find_program(AVR_CXX avr-g++ REQUIRED)
find_program(AVR_OBJCOPY avr-objcopy REQUIRED)
find_program(AVR_SIZE_TOOL avr-size REQUIRED)
find_program(AVR_OBJDUMP avr-objdump REQUIRED)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)
set(CMAKE_C_COMPILER ${AVR_CC})
set(CMAKE_CXX_COMPILER ${AVR_CXX})
set(CMAKE_ASM_COMPILER ${AVR_CC})

set(AVR_UPLOADTOOL_BASE_OPTIONS -p ${AVR_MCU_TYPE} -c ${AVR_PROGRAMMER})

if(AVR_UPLOADTOOL_BAUDRATE)
    set(AVR_UPLOADTOOL_BASE_OPTIONS ${AVR_UPLOADTOOL_BASE_OPTIONS} -b ${AVR_UPLOADTOOL_BAUDRATE})
endif()

function(add_avr_executable EXECUTABLE_NAME)

   set(elf_file ${EXECUTABLE_NAME}.elf)
   set(hex_file ${EXECUTABLE_NAME}.hex)
   set(lst_file ${EXECUTABLE_NAME}.lst)
   set(map_file ${EXECUTABLE_NAME}.map)
   set(eeprom_image ${EXECUTABLE_NAME}-eeprom.hex)

   add_compile_options(
      -DF_CPU=${AVR_MCU_SPEED}
      -mmcu=${AVR_MCU_TYPE}
      -fpack-struct
      -fshort-enums
      -Wall
      -Werror
      -pedantic
      -pedantic-errors
      -funsigned-char
      -funsigned-bitfields
      -ffunction-sections
      -c
      -std=gnu99
      -Os
   )

   add_link_options(
      -mmcu=${AVR_MCU_TYPE} 
      -Wl,--gc-sections 
      -mrelax 
      -Wl,-Map,${map_file}
   )

   add_executable(${elf_file} EXCLUDE_FROM_ALL ${ARGN})

   add_custom_command(
      OUTPUT ${hex_file}
      COMMAND
         ${AVR_OBJCOPY} -j .text -j .data -O ihex ${elf_file} ${hex_file}
      COMMAND
         ${AVR_SIZE_TOOL} ${AVR_SIZE_ARGS} ${elf_file}
      DEPENDS ${elf_file}
   )

   add_custom_command(
      OUTPUT ${lst_file}
      COMMAND
         ${AVR_OBJDUMP} -d ${elf_file} > ${lst_file}
      DEPENDS ${elf_file}
   )

   add_custom_target(
      ${EXECUTABLE_NAME}
      ALL
      DEPENDS ${hex_file} ${lst_file} ${eeprom_image}
   )

   set_target_properties(
      ${EXECUTABLE_NAME}
      PROPERTIES
         OUTPUT_NAME "${elf_file}"
   )

   get_directory_property(clean_files ADDITIONAL_MAKE_CLEAN_FILES)
   set_directory_properties(
      PROPERTIES
         ADDITIONAL_MAKE_CLEAN_FILES "${map_file}"
   )

   add_custom_target(
      upload
      ${AVR_UPLOADTOOL} ${AVR_UPLOADTOOL_BASE_OPTIONS} ${AVR_UPLOADTOOL_OPTIONS}
         -U flash:w:${hex_file}
         -P ${AVR_UPLOADTOOL_PORT}
         -B ${AVR_UPLOADTOOL_BIT_RATE}
      DEPENDS ${hex_file}
      COMMENT "Uploading ${hex_file} to ${AVR_MCU_TYPE} using ${AVR_PROGRAMMER}"
   )

   add_custom_command(
      OUTPUT ${eeprom_image}
      COMMAND
         ${AVR_OBJCOPY} -j .eeprom --set-section-flags=.eeprom=alloc,load
            --change-section-lma .eeprom=0 --no-change-warnings
            -O ihex ${elf_file} ${eeprom_image}
      DEPENDS ${elf_file}
   )

   add_custom_target(
      upload_eeprom
      ${AVR_UPLOADTOOL} ${AVR_UPLOADTOOL_BASE_OPTIONS} ${AVR_UPLOADTOOL_OPTIONS}
         -U eeprom:w:${eeprom_image}
         -P ${AVR_UPLOADTOOL_PORT}
         -B ${AVR_UPLOADTOOL_BIT_RATE}
      DEPENDS ${eeprom_image}
      COMMENT "Uploading ${eeprom_image} to ${AVR_MCU_TYPE} using ${AVR_PROGRAMMER}"
   )

endfunction(add_avr_executable)



# function(avr_target_include_directories EXECUTABLE_TARGET)
#     if(NOT ARGN)
#         message(FATAL_ERROR "No include directories to add to ${EXECUTABLE_TARGET}.")
#     endif()

#     get_target_property(TARGET_LIST ${EXECUTABLE_TARGET} OUTPUT_NAME)
#     set(extra_args ${ARGN})

#     target_include_directories(${TARGET_LIST} ${extra_args})
# endfunction()

##########################################################################
# avr_target_compile_definitions
#
# Calls target_compile_definitions with AVR target names
##########################################################################

# function(avr_target_compile_definitions EXECUTABLE_TARGET)
#     if(NOT ARGN)
#         message(FATAL_ERROR "No compile definitions to add to ${EXECUTABLE_TARGET}.")
#     endif()

#     get_target_property(TARGET_LIST ${EXECUTABLE_TARGET} OUTPUT_NAME)
#     set(extra_args ${ARGN})

#    target_compile_definitions(${TARGET_LIST} ${extra_args})
# endfunction()

# function(avr_generate_fixed_targets)
#    # get status
#    add_custom_target(
#       get_status
#       ${AVR_UPLOADTOOL} ${AVR_UPLOADTOOL_BASE_OPTIONS} -P ${AVR_UPLOADTOOL_PORT} -B ${AVR_UPLOADTOOL_BIT_RATE} -n -v
#       COMMENT "Get status from ${AVR_MCU_TYPE}"
#    )
   
#    # get fuses
#    add_custom_target(
#       get_fuses
#       ${AVR_UPLOADTOOL} ${AVR_UPLOADTOOL_BASE_OPTIONS} -P ${AVR_UPLOADTOOL_PORT} -B ${AVR_UPLOADTOOL_BIT_RATE} -n
#          -U lfuse:r:-:b
#          -U hfuse:r:-:b
#       COMMENT "Get fuses from ${AVR_MCU_TYPE}"
#    )
   
#    # set fuses
#    add_custom_target(
#       set_fuses
#       ${AVR_UPLOADTOOL} ${AVR_UPLOADTOOL_BASE_OPTIONS} -P ${AVR_UPLOADTOOL_PORT} -B ${AVR_UPLOADTOOL_BIT_RATE}
#          -U lfuse:w:${AVR_L_FUSE}:m
#          -U hfuse:w:${AVR_H_FUSE}:m
#          COMMENT "Setup: High Fuse: ${AVR_H_FUSE} Low Fuse: ${AVR_L_FUSE}"
#    )
   
#    # get oscillator calibration
#    add_custom_target(
#       get_calibration
#          ${AVR_UPLOADTOOL} ${AVR_UPLOADTOOL_BASE_OPTIONS} -P ${AVR_UPLOADTOOL_PORT} -B ${AVR_UPLOADTOOL_BIT_RATE}
#          -U calibration:r:${AVR_MCU_TYPE}_calib.tmp:r
#          COMMENT "Write calibration status of internal oscillator to ${AVR_MCU_TYPE}_calib.tmp."
#    )
   
#    # set oscillator calibration
#    add_custom_target(
#       set_calibration
#       ${AVR_UPLOADTOOL} ${AVR_UPLOADTOOL_BASE_OPTIONS} -P ${AVR_UPLOADTOOL_PORT} -B ${AVR_UPLOADTOOL_BIT_RATE}
#          -U calibration:w:${AVR_MCU_TYPE}_calib.hex
#          COMMENT "Program calibration status of internal oscillator from ${AVR_MCU_TYPE}_calib.hex."
#    )
# endfunction()

##########################################################################
# Bypass the link step in CMake's "compiler sanity test" check
#
# CMake throws in a try_compile() target test in some generators, but does
# not know that this is a cross compiler so the executable can't link.
# Change the target type:
#
# https://stackoverflow.com/q/53633705
##########################################################################

# set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# # I target a recent cmake, it shouldn't be a problem on a dev machine
# cmake_minimum_required(VERSION 3.11)
# # Project name
# project("My Firmware")

# # Product filename
# set(PRODUCT_NAME my_firmware)

# ## AVR Chip Configuration
# # 8Mhz, this should match the crystal on your board,
# # I use 8Mhz and 3.3V for the lowest power consumption
# set(F_CPU 8000000UL)
# # CPU, you can find the list here:
# # https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html
# set(MCU atmega328p)
# # Default Baudrate for UART, read avr include/util/setbaud.h for usage
# set(BAUD 9600)
# # The programmer to use, read avrdude manual for list
# set(PROG_TYPE avrispmkII)

# # AVR Fuses, must be in concordance with your hardware and F_CPU
# # http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega328p
# set(E_FUSE 0xfd)
# set(H_FUSE 0xda)
# set(L_FUSE 0xfd)
# set(LOCK_BIT 0xff)


# # Use AVR GCC toolchain
# set(CMAKE_SYSTEM_NAME Generic)
# set(CMAKE_CXX_COMPILER avr-g++)
# set(CMAKE_C_COMPILER avr-gcc)
# set(CMAKE_ASM_COMPILER avr-gcc)


# # Pass defines to compiler
# add_definitions(
#     -DF_CPU=${F_CPU}
#     -DBAUD=${BAUD}
# )
# # mmcu MUST be passed to bot the compiler and linker, this handle the linker
# set(CMAKE_EXE_LINKER_FLAGS -mmcu=${MCU})

# add_compile_options(
#     -mmcu=${MCU} # MCU
#     -std=gnu99 # C99 standard
#     -Os # optimize
#     -Wall # enable warnings
#     -Wno-main
#     -Wundef
#     -pedantic
#     -Wstrict-prototypes
#     -Werror
#     -Wfatal-errors
#     -Wl,--relax,--gc-sections
#     -g
#     -gdwarf-2
#     -funsigned-char # a few optimizations
#     -funsigned-bitfields
#     -fpack-struct
#     -fshort-enums
#     -ffunction-sections
#     -fdata-sections
#     -fno-split-wide-types
#     -fno-tree-scev-cprop
# )
# file(GLOB SRC_FILES "src/*.c") # Load all files in src folder

# # Create one target
# add_executable(${PRODUCT_NAME} ${SRC_FILES})

# # Rename the output to .elf as we will create multiple files
# set_target_properties(${PRODUCT_NAME} PROPERTIES OUTPUT_NAME ${PRODUCT_NAME}.elf)

# # Strip binary for upload
# add_custom_target(strip ALL avr-strip ${PRODUCT_NAME}.elf DEPENDS ${PRODUCT_NAME})

# # Transform binary into hex file, we ignore the eeprom segments in the step
# add_custom_target(hex ALL avr-objcopy -R .eeprom -O ihex ${PRODUCT_NAME}.elf ${PRODUCT_NAME}.hex DEPENDS strip)
# # Transform binary into hex file, this is the eeprom part (empty if you don't
# # use eeprom static variables)
# add_custom_target(eeprom avr-objcopy -j .eeprom  --set-section-flags=.eeprom="alloc,load"  --change-section-lma .eeprom=0 -O ihex ${PRODUCT_NAME}.elf ${PRODUCT_NAME}.eep DEPENDS strip)

# # Upload the firmware with avrdude
# add_custom_target(upload avrdude  -c ${PROG_TYPE} -p ${MCU} -U flash:w:${PRODUCT_NAME}.hex DEPENDS hex)

# # Upload the eeprom with avrdude
# add_custom_target(upload_eeprom avrdude -c ${PROG_TYPE} -p ${MCU}  -U eeprom:w:${PRODUCT_NAME}.eep DEPENDS eeprom)

# # Burn fuses
# add_custom_target(fuses avrdude -c ${PROG_TYPE} -p ${MCU}  -U lfuse:w:${L_FUSE}:m -U hfuse:w:${H_FUSE}:m -U efuse:w:${E_FUSE}:m -U lock:w:${LOCK_BIT}:m )

# # Clean extra files
# set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PRODUCT_NAME}.hex;${PRODUCT_NAME}.eeprom;${PRODUCT_NAME}.lst")