cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/stm32f407vetx_flash.ld)
target_link_options(stm32cubemx INTERFACE "-T${LINKER_SCRIPT}")

