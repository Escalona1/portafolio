cmake_minimum_required(VERSION 3.14)
project(recipient)

set(SODIUM_DISABLE_TESTS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
    src/sha256.cpp
    src/recipient.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..)

add_executable(${PROJECT_NAME} ${SOURCES})
add_executable(decrypt src/decrypt.c src/crypto.c src/util.c)

include_directories(include)

target_link_libraries(decrypt PRIVATE sodium)
