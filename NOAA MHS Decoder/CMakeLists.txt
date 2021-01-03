cmake_minimum_required(VERSION 3.0.0)
project(NOAA-MHS-Decoder VERSION 0.1.0)

add_executable(NOAA-MHS-Decoder src/main.cpp)

target_include_directories(NOAA-MHS-Decoder PUBLIC src)

find_package(Threads REQUIRED)
target_link_libraries(NOAA-MHS-Decoder PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()

target_link_libraries(NOAA-MHS-Decoder PUBLIC PNG::PNG)

install(TARGETS NOAA-MHS-Decoder DESTINATION bin)