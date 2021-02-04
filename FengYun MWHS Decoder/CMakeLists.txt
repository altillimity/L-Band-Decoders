cmake_minimum_required (VERSION 3.0.0)

project (FengYun-MWHS-Decoder)
file(GLOB_RECURSE FengYun-MWHS-Decoder_CPPS src/*.cpp)

add_executable(FengYun-MWHS-Decoder ${FengYun-MWHS-Decoder_CPPS})

target_include_directories(FengYun-MWHS-Decoder PUBLIC src)

target_link_libraries(FengYun-MWHS-Decoder PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(FengYun-MWHS-Decoder PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()
target_link_libraries(FengYun-MWHS-Decoder PUBLIC PNG::PNG)

find_library(CCSDS_LIBRARY ccsds REQUIRED)
target_link_libraries(FengYun-MWHS-Decoder PUBLIC ${CCSDS_LIBRARY})

install(TARGETS FengYun-MWHS-Decoder DESTINATION bin)
