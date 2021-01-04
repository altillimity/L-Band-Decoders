cmake_minimum_required (VERSION 3.0.0)

project (MetOp-HIRS-Decoder)
file(GLOB_RECURSE MetOp-HIRS-Decoder_CPPS src/*.cpp)

add_executable(MetOp-HIRS-Decoder ${MetOp-HIRS-Decoder_CPPS})

target_include_directories(MetOp-HIRS-Decoder PUBLIC src)

target_link_libraries(MetOp-HIRS-Decoder PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(MetOp-HIRS-Decoder PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()
target_link_libraries(MetOp-HIRS-Decoder PUBLIC PNG::PNG)

find_library(CCSDS_LIBRARY ccsds REQUIRED)
target_link_libraries(MetOp-HIRS-Decoder PUBLIC ${CCSDS_LIBRARY})

install(TARGETS MetOp-HIRS-Decoder DESTINATION bin)
