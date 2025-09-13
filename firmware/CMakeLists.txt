cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)            # Use C11 standard for compilation
set(CMAKE_C_STANDARD_REQUIRED ON)   # Ensure that the C11 standard is enforced
set(CMAKE_C_EXTENSIONS ON)          # Enable compiler-specific extensions

# Enable compile command to ease indexing with e.g. clangd (useful for IDEs and code completion)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()
message("Build type: " ${CMAKE_BUILD_TYPE})

# Set the project name to the selected application
set(CMAKE_PROJECT_NAME "LumiBox") 

include("Core/cmake/gcc-arm-none-eabi.cmake")
# Enable CMake support for ASM and C languages
enable_language(C ASM)

project(${CMAKE_PROJECT_NAME})
add_executable(${CMAKE_PROJECT_NAME})

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/lumi_box_main.c
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/utils
)

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE)

add_subdirectory(Core)
add_subdirectory(drivers)

set(LV_CONF_PATH "${CMAKE_CURRENT_LIST_DIR}/config/lv_conf.h"
    CACHE FILEPATH "Path to LVGL configuration header")
add_subdirectory(lvgl)
add_subdirectory(gui)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    lvgl
)

### Commands
include(cmake/utils.cmake)
