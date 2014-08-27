cmake_minimum_required(VERSION 2.6.2)

project(cbpp)

include_directories(
    .
)

set(CMAKE_CXXFLAGS "${CMAKE_CXXFLAGS}  -g -O0 -Wall")

# Linker options
set(CMAKE_LDFLAGS "${CMAKE_LDFLAGS}  ")

# Library path
set(CMAKE_LDFLAGS "${CMAKE_LDFLAGS} -L\".\" ")

set(SRCS
    ../lib/ComputeParser.c
    src/fcpp.c
    src/Base.cpp
    src/cbpp.cpp
    src/ComputeProcessor.cpp
    src/PrologueTransform.cpp
    src/TextureTransform.cpp
)

add_executable(cbpp ${SRCS})

