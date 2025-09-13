### Commands
# Add custom commands to convert ELF to BIN and HEX

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${CMAKE_PROJECT_NAME}.elf ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.hex
    COMMENT "Converting ELF to BIN and HEX..."
)

# Add custom command to run the size utility
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} -A -x ${CMAKE_PROJECT_NAME}.elf
    COMMENT "Running size utility..."
)

# Add a custom target to clean generated files
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.elf
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.hex
    COMMENT "Removing generated binary files"
)

add_custom_target(flash
    COMMAND st-flash --reset write ${CMAKE_PROJECT_NAME}.bin 0x8000000
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS ${CMAKE_PROJECT_NAME}
    COMMENT "Program flash ..."
)

add_custom_target(gdb
    COMMAND gdb-multiarch build/${CMAKE_PROJECT_NAME}.elf -ex "target remote localhost:3333"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Run GDB ..."
)

add_custom_target(openOCDServer
    COMMAND openocd -f interface/stlink-v2.cfg -f target/stm32f4x.cfg -c "adapter speed 1000"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Run openOCD for stm32f4"
)
