## require a minimum version of CMake to avoid problems
cmake_minimum_required (VERSION 3.10)

project (OVIG)
set(EXENAME "ovig")

## assuming GCC
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

## exporting 'compile_commands.json' for language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT DEFINED OSRM_DIR)
  set(OSRM_DIR "/usr/local/")
endif(NOT DEFINED OSRM_DIR)

message(STATUS "Using OSRM install directory as: ${OSRM_DIR}")

## adds the include directories of this project (i.e. ./include/*)
include_directories(BEFORE /usr/lib/ ${OSRM_DIR} ${OSRM_DIR}include/osrm/ ${CMAKE_CURRENT_SOURCE_DIR}/include/)

## to find and add the Boost package program options
find_package(Boost REQUIRED program_options system thread filesystem iostreams)
include_directories(BEFORE ${Boost_INCLUDE_DIRS})

## to get OSRM lib
set(LibOSRM_LIBRARY "${OSRM_DIR}lib/libosrm.a")
link_directories(${LibOSRM_LIBRARY})

## set basic compilation flags
set(CXX_FLAGS "-Wall -O3 -std=c++14")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${CXX_FLAGS}")

## set source files of OVIG
set( OvigSrcFiles
  ${PROJECT_SOURCE_DIR}/src/builder.cpp
  ${PROJECT_SOURCE_DIR}/src/configurations.cpp
  ${PROJECT_SOURCE_DIR}/src/cvrp_builder.cpp
  ${PROJECT_SOURCE_DIR}/src/distributer.cpp
  ${PROJECT_SOURCE_DIR}/src/geolocation.cpp
  ${PROJECT_SOURCE_DIR}/src/instance.cpp
  ${PROJECT_SOURCE_DIR}/src/instance_creator.cpp
  ${PROJECT_SOURCE_DIR}/src/main.cpp
  ${PROJECT_SOURCE_DIR}/src/pdptw_builder.cpp
  ${PROJECT_SOURCE_DIR}/src/router.cpp
  )

## define the executable and the files to compile it
add_executable(${EXENAME} ${OvigSrcFiles})

## link external libraries (e.g., math (m) and the boost libs found before)
target_link_libraries(${EXENAME} ${LibOSRM_LIBRARY} m pthread rt ${Boost_LIBRARIES})
