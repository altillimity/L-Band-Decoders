cmake_minimum_required(VERSION 3.0.0)
project(NOAA-AMSU-Decoder VERSION 0.1.0)

file(GLOB_RECURSE NOAA-AMSU-Decoder_CPPS src/*.cpp)

add_executable(NOAA-AMSU-Decoder ${NOAA-AMSU-Decoder_CPPS})

target_include_directories(NOAA-AMSU-Decoder PUBLIC src)

find_package(Threads REQUIRED)
target_link_libraries(NOAA-AMSU-Decoder PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()

target_link_libraries(NOAA-AMSU-Decoder PUBLIC PNG::PNG)

install(TARGETS NOAA-AMSU-Decoder DESTINATION bin)