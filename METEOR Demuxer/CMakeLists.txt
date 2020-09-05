cmake_minimum_required (VERSION 3.0.0)

project (METEOR-Demux)
file(GLOB_RECURSE METEOR-Demux_CPPS src/*.cpp src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)

add_executable(METEOR-Demux ${METEOR-Demux_CPPS})

target_include_directories(METEOR-Demux PUBLIC src)

target_link_libraries(METEOR-Demux PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(METEOR-Demux PUBLIC Threads::Threads)

install(TARGETS METEOR-Demux DESTINATION bin)