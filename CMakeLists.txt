cmake_minimum_required(VERSION 3.10)
project(chip8)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
find_package(SDL2 REQUIRED)
add_executable(chip8
    src/main.cpp
    src/cpu.h
    src/cpu.cpp
    src/display.cpp
    src/display.h
    src/emulator.cpp
    src/emulator.h
    src/ram.h
    src/ram.cpp
    src/utilities.h
)
target_include_directories(chip8 PRIVATE ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS})
target_link_libraries(chip8 ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_MIXER_LIBRARIES})
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(chip8 PRIVATE DEBUG)
else()
    target_compile_definitions(chip8 PRIVATE NDEBUG)
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