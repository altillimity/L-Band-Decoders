cmake_minimum_required (VERSION 3.0.0)

project (MetOp-MHS-Decoder)
file(GLOB_RECURSE MetOp-MHS-Decoder_CPPS src/*.cpp)

add_executable(MetOp-MHS-Decoder ${MetOp-MHS-Decoder_CPPS})

target_include_directories(MetOp-MHS-Decoder PUBLIC src)

target_link_libraries(MetOp-MHS-Decoder PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(MetOp-MHS-Decoder PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()
target_link_libraries(MetOp-MHS-Decoder PUBLIC PNG::PNG)

find_library(CCSDS_LIBRARY ccsds REQUIRED)
target_link_libraries(MetOp-MHS-Decoder PUBLIC ${CCSDS_LIBRARY})

install(TARGETS MetOp-MHS-Decoder DESTINATION bin)
