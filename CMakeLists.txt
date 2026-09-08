cmake_minimum_required(VERSION 4.1)
project(ONVIF_SCANNER)

find_package(pugixml CONFIG REQUIRED)
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

set(CMAKE_CXX_STANDARD 20)

add_executable(example temp/example.cpp
        include/onvif/xml_utils.h)

add_library(ONVIF_SCANNER STATIC library.cpp
        include/onvif/scanner.h
        include/onvif/device.h
        include/onvif/discovery.h
        src/device.cpp
        src/scanner.cpp
        src/discovery.cpp
        src/soap_client.cpp
        src/ws_security.cpp
        include/onvif/xml_utils.h
        src/xml_utils.cpp
        temp/example.cpp)

target_include_directories(ONVIF_SCANNER PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(ONVIF_SCANNER PRIVATE pugixml::pugixml CURL::libcurl OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(example PRIVATE ONVIF_SCANNER)