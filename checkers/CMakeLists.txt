# Generated with ChatGPT

cmake_minimum_required(VERSION 3.10)

# Project name and version
project(CheckersClient VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# Define the directories
set(SRCDIR src)
set(BUILDDIR ${CMAKE_BINARY_DIR}/build)
set(BINDIR ${SRCDIR}/bin)

# Create the build directories
file(MAKE_DIRECTORY ${BUILDDIR}/client)
file(MAKE_DIRECTORY ${BUILDDIR}/checkers)
file(MAKE_DIRECTORY ${BUILDDIR}/bin)

# Define the source files
file(GLOB CLIENT_SRCS "${SRCDIR}/client/*.cpp")
file(GLOB CHECKERS_SRCS "${SRCDIR}/checkers/*.cpp")
set(BINDIR_SRCS "${BINDIR}/checkers_client.cpp")

set(SRCS ${CLIENT_SRCS} ${CHECKERS_SRCS} ${BINDIR_SRCS})

# Define the headers for formatting
file(GLOB CLIENT_HEADERS "${SRCDIR}/client/*.h")
file(GLOB CHECKERS_HEADERS "${SRCDIR}/checkers/*.h")
file(GLOB INCLUDE_HEADERS "${SRCDIR}/include/*.h")

set(HEADERS ${CLIENT_HEADERS} ${CHECKERS_HEADERS} ${INCLUDE_HEADERS})

# Add the executable
add_executable(checkers_client ${SRCS})

# Set the output directory for the binary
set_target_properties(checkers_client PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILDDIR}/bin)

# Copy the binary to the project's root directory after build
add_custom_command(TARGET checkers_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:checkers_client> ${CMAKE_SOURCE_DIR})

# Format target
add_custom_target(format
    COMMAND clang-format -i --style=llvm ${SRCS} ${HEADERS})

# Clean target
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${BUILDDIR})
