cmake_minimum_required (VERSION 3.0.0)

project (NOAA-Demodulator)
file(GLOB_RECURSE NOAA-Demodulator_CPPS src/*.cpp src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)

add_executable(NOAA-Demodulator ${NOAA-Demodulator_CPPS})

target_include_directories(NOAA-Demodulator PUBLIC src)

target_link_libraries(NOAA-Demodulator PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(NOAA-Demodulator PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()
target_link_libraries(NOAA-Demodulator PUBLIC PNG::PNG)