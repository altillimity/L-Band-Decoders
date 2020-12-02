cmake_minimum_required (VERSION 3.0.0)

project (MetOp-IASI-Decoder)
file(GLOB_RECURSE MetOp-IASI-Decoder_CPPS src/*.cpp)

add_executable(MetOp-IASI-Decoder ${MetOp-IASI-Decoder_CPPS})

target_include_directories(MetOp-IASI-Decoder PUBLIC src)

target_link_libraries(MetOp-IASI-Decoder PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(MetOp-IASI-Decoder PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()
target_link_libraries(MetOp-IASI-Decoder PUBLIC PNG::PNG)

find_library(CCSDS_LIBRARY ccsds REQUIRED)
target_link_libraries(MetOp-IASI-Decoder PUBLIC ${CCSDS_LIBRARY})

install(TARGETS MetOp-IASI-Decoder DESTINATION bin)
