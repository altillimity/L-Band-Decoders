cmake_minimum_required(VERSION 3.0.0)
project(MHS_Decoder VERSION 0.1.0)

add_executable(MHS_Decoder src/main.cpp)

target_include_directories(MHS_Decoder PUBLIC src)

find_package(Threads REQUIRED)
target_link_libraries(MHS_Decoder PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()

target_link_libraries(MHS_Decoder PUBLIC PNG::PNG)

install(TARGETS MHS_Decoder DESTINATION bin)