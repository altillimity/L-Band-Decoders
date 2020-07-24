cmake_minimum_required (VERSION 3.0.0)

project (CCSDS-Demux)
file(GLOB_RECURSE CCSDS-Demux_CPPS src/*.cpp src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)

add_executable(CCSDS-Demux ${CCSDS-Demux_CPPS})

target_include_directories(CCSDS-Demux PUBLIC src)

target_link_libraries(CCSDS-Demux PUBLIC m)

target_link_libraries (CCSDS-Demux PUBLIC "fec")

find_package(Threads REQUIRED)
target_link_libraries(CCSDS-Demux PUBLIC Threads::Threads)
