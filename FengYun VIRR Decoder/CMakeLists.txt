cmake_minimum_required (VERSION 3.0.0)

project (FengYun-VIRR-Decoder)
file(GLOB_RECURSE FengYun-VIRR-Decoder_CPPS src/*.cpp)

add_executable(FengYun-VIRR-Decoder ${FengYun-VIRR-Decoder_CPPS})

target_include_directories(FengYun-VIRR-Decoder PUBLIC src)

target_link_libraries(FengYun-VIRR-Decoder PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(FengYun-VIRR-Decoder PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()
target_link_libraries(FengYun-VIRR-Decoder PUBLIC PNG::PNG)