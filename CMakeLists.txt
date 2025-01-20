cmake_minimum_required(VERSION 3.5.1)

project(. VERSION "0.1" LANGUAGES C)

# Collect all source files
file(GLOB SRC "*.c" "*.h")
list(SORT SRC)

# Conditionally build either a static or shared library based on BUILD_SHARED_LIBS
if(BUILD_SHARED_LIBS)
    add_library(xdiff SHARED ${SRC})
    set_target_properties(xdiff PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 1)
else()
    add_library(xdiff STATIC ${SRC})
endif()