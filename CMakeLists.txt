## require a minimum version of CMake to avoid problems
cmake_minimum_required (VERSION 3.10)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

## in case clang is preferred
## set(CMAKE_C_COMPILER "clang")
## set(CMAKE_CXX_COMPILER "clang++")

## exporting 'compile_commands.json' for language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project (SDTP)

option(MODE_DEBUG "Whether to set deubgging mode" OFF)
option(MODE_PROFILE "Whether to set profiling" OFF)

if(NOT DEFINED cplex_location)
  set(cplex_location "/opt/ibm/ILOG/CPLEX_Studio129/")
endif(NOT DEFINED cplex_location)

if(MODE_PROFILE)
  set(profiling "-pg -ggdb3")
else()
  set(profiling "")
endif()

message(STATUS "Debug: ${MODE_DEBUG}")
message(STATUS "Profiling: ${MODE_PROFILE}")
message(STATUS "Using CPLEX folder: ${cplex_location}")

## adds the include directories of this project (i.e. ./include/*)
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories(BEFORE /usr/lib/ ${cplex_location}cplex/include/ ${cplex_location}concert/include/ ${cplex_location}cpoptimizer/include/)

## to find and add the Boost package program options (we may be able to remove this package later!)
find_package(Boost REQUIRED program_options regex)
include_directories(BEFORE ${Boost_INCLUDE_DIRS})

link_directories(${cplex_location}cplex/lib/x86-64_linux/static_pic ${cplex_location}concert/lib/x86-64_linux/static_pic ${cplex_location}cpoptimizer/lib/x86-64_linux/static_pic)

## set basic compilation flags
set(CXX_FLAGS "-Wall -Wno-array-bounds -O3 -std=c++17 -DIL_STD ${profiling}")

##combine all flags for the C++ compiler
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${CXX_FLAGS} -Wno-ignored-attributes")

## set source files needed

set( BaseSrcFiles
  ${PROJECT_SOURCE_DIR}/source/io/instance_io.cpp
  ${PROJECT_SOURCE_DIR}/source/io/result_io.cpp
  ${PROJECT_SOURCE_DIR}/source/solver/solver_factory.cpp
  ${PROJECT_SOURCE_DIR}/source/solver/solver_exec.cpp
  ${PROJECT_SOURCE_DIR}/source/solver/cra/comin_rizzi_alg.cpp
  ${PROJECT_SOURCE_DIR}/source/solver/cra/dijkstra_alg.cpp
  ${PROJECT_SOURCE_DIR}/source/solver/cra/bellman_ford_alg.cpp
  ${PROJECT_SOURCE_DIR}/source/util/heaps/sanders/knheap.C
  ${PROJECT_SOURCE_DIR}/source/solver/ilp/ilp.cpp
  ${PROJECT_SOURCE_DIR}/source/solver/cp/cp.cpp
  ${PROJECT_SOURCE_DIR}/source/solver/bfdc/bfdc_alg.cpp
  ${PROJECT_SOURCE_DIR}/source/solver/ult/apsp_alg.cpp
  ${PROJECT_SOURCE_DIR}/source/solver/ult/ult_alg.cpp
  ${PROJECT_SOURCE_DIR}/source/solver/rult/two_ways_bf_alg.cpp
  ${PROJECT_SOURCE_DIR}/source/solver/rult/red_ult_alg.cpp
  ${PROJECT_SOURCE_DIR}/source/solver/ka/dist_alg_bf.cpp
  ${PROJECT_SOURCE_DIR}/source/solver/ka/bellman_ford_alg.cpp
  ${PROJECT_SOURCE_DIR}/source/solver/ka/kumar.cpp
  ${PROJECT_SOURCE_DIR}/source/solver/ka/dist_alg_ja.cpp
  ${PROJECT_SOURCE_DIR}/source/solver/ka/dijkstra_alg.cpp
  ${PROJECT_SOURCE_DIR}/source/solver/ka/dinic_alg.cpp
  ${PROJECT_SOURCE_DIR}/source/validator/validator.cpp
  )

set( PerfSrcFiles
  ${PROJECT_SOURCE_DIR}/source/io/perf_values_io.cpp
  ${PROJECT_SOURCE_DIR}/source/solver/solver_perf.cpp
  )

## define the executable and the files to compile it
add_executable(sdtp ${BaseSrcFiles} "${PROJECT_SOURCE_DIR}/source/exec/sdtp.cpp")
add_executable(stest ${BaseSrcFiles} "${PROJECT_SOURCE_DIR}/source/exec/stest.cpp")
add_executable(sperf ${BaseSrcFiles} ${PerfSrcFiles} "${PROJECT_SOURCE_DIR}/source/exec/sperf.cpp")

## link external libraries (e.g., math (m) and the boost libs found before)
target_link_libraries(sdtp ${Boost_LIBRARIES} cp ilocplex concert cplex m pthread dl)
target_link_libraries(stest ${Boost_LIBRARIES} cp ilocplex concert cplex m pthread dl)
target_link_libraries(sperf ${Boost_LIBRARIES} cp ilocplex concert cplex m pthread dl)
