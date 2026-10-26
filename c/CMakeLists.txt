cmake_minimum_required(VERSION 3.20)

project(HttpApi VERSION 1.0.0 DESCRIPTION "C implementation of a modular HTTP API" LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/civetweb/src/civetweb.c")
    message(FATAL_ERROR "CivetWeb is missing. Run scripts/setup-dependency.sh (MSYS2) or follow README.md.")
endif()

add_executable(c-api
    src/main.c
    src/config.c
    src/routes.c
    src/responses.c
    src/middleware.c
    src/versioning.c
    src/validation.c
    third_party/civetweb/src/civetweb.c
)

target_include_directories(c-api PRIVATE
    src
    third_party/civetweb/include
    third_party/civetweb/src
)

target_compile_definitions(c-api PRIVATE NO_SSL NO_CGI NO_FILES)

if(MSVC)
    target_compile_options(c-api PRIVATE /W4)
else()
    target_compile_options(c-api PRIVATE -Wall -Wextra)
endif()

if(WIN32)
    target_link_libraries(c-api PRIVATE ws2_32)
endif()