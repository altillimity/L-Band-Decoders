cmake_minimum_required (VERSION 3.0.0)

project (MetOp-AVHRR-Decoder)
file(GLOB_RECURSE MetOp-AVHRR-Decoder_CPPS src/*.cpp src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)

add_executable(MetOp-AVHRR-Decoder ${MetOp-AVHRR-Decoder_CPPS})

target_include_directories(MetOp-AVHRR-Decoder PUBLIC src)

target_link_libraries(MetOp-AVHRR-Decoder PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(MetOp-AVHRR-Decoder PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()
target_link_libraries(MetOp-AVHRR-Decoder PUBLIC PNG::PNG)

install(TARGETS MetOp-AVHRR-Decoder DESTINATION bin)