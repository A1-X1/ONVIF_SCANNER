cmake_minimum_required(VERSION 4.1)
project(ONVIF_SCANNER)

set(CMAKE_CXX_STANDARD 20)

add_library(ONVIF_SCANNER STATIC library.cpp
        include/onvif/scanner.h
        include/onvif/device.h
        include/onvif/types.h
        src/device.cpp
        src/types.cpp
        src/scanner.cpp
        temp/probe.cpp)
