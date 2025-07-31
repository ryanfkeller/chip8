cmake_minimum_required(VERSION 3.10)
project(chip8)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

find_package(SDL2 REQUIRED)

add_executable(chip8
    src/cpu.cpp
    src/emulator.cpp
    src/main.cpp
    src/peripherals.cpp
    src/ram.cpp
    src/timer.cpp
)

target_include_directories(chip8 PRIVATE
    src/
    ${SDL2_INCLUDE_DIRS}
)

target_compile_options(chip8 PRIVATE -Wall -Wextra -pedantic)
target_link_libraries(chip8 PRIVATE ${SDL2_LIBRARIES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(chip8 PRIVATE DEBUG)
    target_compile_options(chip8 PRIVATE -g -O0)
else()
    target_compile_definitions(chip8 PRIVATE NDEBUG)
    target_compile_options(chip8 PRIVATE -O2)
endif()

# Install rules
install(TARGETS chip8
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
# Packaging rules
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_NAME "chip8-sdl2")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION "A simple CHIP-8 emulator using SDL2")