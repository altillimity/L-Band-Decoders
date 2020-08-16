cmake_minimum_required (VERSION 3.0.0)

project (MetOp)
if(WIN32)
	file(GLOB_RECURSE MetOp_CPPS src/*.cpp src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)
else()
	file(GLOB_RECURSE MetOp_CPPS src/main.cpp src/viterbi.cpp src/viterbi_lib/metrics.c src/viterbi_lib/tab.c src/viterbi_lib/viterbi.c)
endif()

add_executable(MetOp ${MetOp_CPPS})

target_include_directories(MetOp PUBLIC src)

target_link_libraries(MetOp PUBLIC m)

find_package(Threads REQUIRED)
target_link_libraries(MetOp PUBLIC Threads::Threads)
