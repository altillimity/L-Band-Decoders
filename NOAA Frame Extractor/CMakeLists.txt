cmake_minimum_required (VERSION 3.0.0)

project (NOAA-Frame-Extractor)
file(GLOB_RECURSE NOAA-Frame-Extractor_CPPS src/*.cpp src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)

add_executable(NOAA-Frame-Extractor ${NOAA-Frame-Extractor_CPPS})

target_include_directories(NOAA-Frame-Extractor PUBLIC src)

target_link_libraries(NOAA-Frame-Extractor PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(NOAA-Frame-Extractor PUBLIC Threads::Threads)

if(WIN32 AND NOT MINGW)
    find_package(PNG CONFIG REQUIRED)
else()
    find_package(PNG REQUIRED)
endif()
target_link_libraries(NOAA-Frame-Extractor PUBLIC PNG::PNG)

install(TARGETS NOAA-Frame-Extractor DESTINATION bin)