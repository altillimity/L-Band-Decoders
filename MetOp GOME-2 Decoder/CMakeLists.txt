cmake_minimum_required (VERSION 3.0.0)

project (MetOp-GOME-2-Decoder)
file(GLOB_RECURSE MetOp-GOME-2-Decoder_CPPS src/*.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MetOp-GOME-2-Decoder ${MetOp-GOME-2-Decoder_CPPS})

target_include_directories(MetOp-GOME-2-Decoder PUBLIC src)

target_link_libraries(MetOp-GOME-2-Decoder PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(MetOp-GOME-2-Decoder PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()
target_link_libraries(MetOp-GOME-2-Decoder PUBLIC PNG::PNG)

find_library(CCSDS_LIBRARY ccsds REQUIRED)
target_link_libraries(MetOp-GOME-2-Decoder PUBLIC ${CCSDS_LIBRARY})

install(TARGETS MetOp-GOME-2-Decoder DESTINATION bin)
