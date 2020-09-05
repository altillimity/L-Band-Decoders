cmake_minimum_required (VERSION 3.0.0)

project (METEOR-Demodulator)
file(GLOB_RECURSE METEOR-Demodulator_CPPS src/*.cpp src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)

add_executable(METEOR-Demodulator ${METEOR-Demodulator_CPPS})

target_include_directories(METEOR-Demodulator PUBLIC src)

target_link_libraries(METEOR-Demodulator PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(METEOR-Demodulator PUBLIC Threads::Threads)