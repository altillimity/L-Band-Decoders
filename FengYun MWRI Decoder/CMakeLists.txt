cmake_minimum_required (VERSION 3.0.0)

project (FengYun-MWRI-Decoder)
file(GLOB_RECURSE FengYun-MWRI-Decoder_CPPS src/*.cpp)

add_executable(FengYun-MWRI-Decoder ${FengYun-MWRI-Decoder_CPPS})

target_include_directories(FengYun-MWRI-Decoder PUBLIC src)

target_link_libraries(FengYun-MWRI-Decoder PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(FengYun-MWRI-Decoder PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()
target_link_libraries(FengYun-MWRI-Decoder PUBLIC PNG::PNG)

find_library(CCSDS_LIBRARY ccsds REQUIRED)
target_link_libraries(FengYun-MWRI-Decoder PUBLIC ${CCSDS_LIBRARY})

install(TARGETS FengYun-MWRI-Decoder DESTINATION bin)
