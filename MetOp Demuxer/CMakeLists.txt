cmake_minimum_required (VERSION 3.0.0)

project (MetOp-Demux)
file(GLOB_RECURSE MetOp-Demux_CPPS src/*.cpp src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)

add_executable(MetOp-Demux ${MetOp-Demux_CPPS})

target_include_directories(MetOp-Demux PUBLIC src)

target_link_libraries(MetOp-Demux PUBLIC m)

target_link_libraries (MetOp-Demux PUBLIC "fec")

find_package(Threads REQUIRED)
target_link_libraries(MetOp-Demux PUBLIC Threads::Threads)
