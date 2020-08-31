cmake_minimum_required (VERSION 3.0.0)

project (METEOR-MSU-MR-Decoder)
file(GLOB_RECURSE METEOR-MSU-MR-Decoder_CPPS src/*.cpp src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)

add_executable(METEOR-MSU-MR-Decoder ${METEOR-MSU-MR-Decoder_CPPS})

target_include_directories(METEOR-MSU-MR-Decoder PUBLIC src)

target_link_libraries(METEOR-MSU-MR-Decoder PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(METEOR-MSU-MR-Decoder PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()
target_link_libraries(METEOR-MSU-MR-Decoder PUBLIC PNG::PNG)