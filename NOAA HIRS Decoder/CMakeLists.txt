cmake_minimum_required(VERSION 3.0.0)
project(NOAA-HIRS-Decoder VERSION 0.1.0)

file(GLOB_RECURSE NOAA-HIRS-Decoder_CPPS src/*.cpp)

add_executable(NOAA-HIRS-Decoder ${NOAA-HIRS-Decoder_CPPS})

target_include_directories(NOAA-HIRS-Decoder PUBLIC src)

find_package(Threads REQUIRED)
target_link_libraries(NOAA-HIRS-Decoder PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()

target_link_libraries(NOAA-HIRS-Decoder PUBLIC PNG::PNG)

install(TARGETS NOAA-HIRS-Decoder DESTINATION bin)