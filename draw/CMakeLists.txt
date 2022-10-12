# CMake file mostly written from this example:
# https://cliutils.gitlab.io/modern-cmake/chapters/packages/ROOT.html
# https://cliutils.gitlab.io/modern-cmake/examples/root-dict/

# Assert CMake version
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# This is your project statement. You should always list languages
project(draw LANGUAGES CXX)

# ROOT 6.10+ supports config file discovery
find_package(ROOT REQUIRED COMPONENTS RooFit RooFitCore Gui)

# TODO: try just this
# find_package(ROOT CONFIG REQUIRED)

# CMake module which loads useful macros and functions (such as ROOT_GENERATE_DICTIONARY)
# include(${ROOT_USE_FILE})

# For setting project in IDE via CMake generators
include_directories(${ROOT_INCLUDE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Find draw scripts to be built (each will be individual executable)
file(GLOB DRAW_SCRIPTS CONFIGURE_DEPENDS
                  ${PROJECT_SOURCE_DIR}/*.cpp
                  ${PROJECT_SOURCE_DIR}/*.cxx
                  ${PROJECT_SOURCE_DIR}/*.cc
                  ${PROJECT_SOURCE_DIR}/*.c)
# message(STATUS "Found list of ROOT draw scripts:")
# message(STATUS "${DRAW_SCRIPTS}")

#SET(DRAW_SCRIPTS ${PROJECT_SOURCE_DIR}/tof.cpp)

# Find other sources in project
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
                  ${PROJECT_SOURCE_DIR}/src/*.cpp
                  ${PROJECT_SOURCE_DIR}/src/*.cxx
                  ${PROJECT_SOURCE_DIR}/src/*.cc
                  ${PROJECT_SOURCE_DIR}/src/*.c)
# message(STATUS "Found list of dependent C++ source files:")
# message(STATUS "${SOURCES}")

# Find headers in project
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS 
                  ${PROJECT_SOURCE_DIR}/include/*.hpp
                  ${PROJECT_SOURCE_DIR}/include/*.hxx
                  ${PROJECT_SOURCE_DIR}/include/*.hh
                  ${PROJECT_SOURCE_DIR}/include/*.h)
# message(STATUS "Found list of dependent C++ header files:")
# message(STATUS "${HEADERS}")                  

# Compose list of ROOT libraries with "ROOT::" prefix
  set(LIB_NAMES "")
  FOREACH(X IN LISTS ROOT_LIBRARIES)
    get_filename_component(FILENAME ${X} NAME)
    string(REGEX REPLACE "lib([a-zA-Z0-9]+).so" "ROOT::\\1" FILENAME2 ${FILENAME})
    list(APPEND LIB_NAMES "${FILENAME2}")
  ENDFOREACH()

# Manually append extra ROOT libraries (why missing?)
  list(APPEND LIB_NAMES "ROOT::Gui")
  list(APPEND LIB_NAMES "ROOT::RooFit")
  list(APPEND LIB_NAMES "ROOT::RooFitCore")
  list(APPEND LIB_NAMES "ROOT::Html")
  list(APPEND LIB_NAMES "ROOT::Minuit")
  list(APPEND LIB_NAMES "ROOT::Fumili")

# Find my custom CanvasHelper shared library and add it to the . 
# First parameter here is result of found library - its location. Refer to CMake docs how to find in Env Variable
find_library(CH_LIBRARY_LOCATION CanvasHelper PATHS ENV LD_LIBRARY_PATH)
message(STATUS "Found CanvasHelper library: ${CH_LIBRARY_LOCATION}")
list(APPEND LIB_NAMES "${CH_LIBRARY_LOCATION}")

# Find my custom CanvasHelper shared library and add it to the . 
# First parameter here is result of found library - its location. Refer to CMake docs how to find in Env Variable
find_library(RU_LIBRARY_LOCATION RootUtils PATHS ENV LD_LIBRARY_PATH)
message(STATUS "Found CanvasHelper library: ${RU_LIBRARY_LOCATION}")
list(APPEND LIB_NAMES "${RU_LIBRARY_LOCATION}")

# Build every ROOT script individually
foreach(SCRIPT ${DRAW_SCRIPTS})
	
  # Compose the executable name - remove script extension
  get_filename_component(EXECUTABLE "${SCRIPT}" NAME_WE)

  # Generate dictionary (one for all draw entry points)
  root_generate_dictionary(D_${EXECUTABLE} ${HEADERS} LINKDEF ${PROJECT_SOURCE_DIR}/LinkDef.h)

  # Create target that generates shared library from provided source files and dictionary
  add_library(SO_${EXECUTABLE} SHARED ${SCRIPT} ${SOURCES} D_${EXECUTABLE}.cxx)

  # Ensure that properties such as required include directories and C++ standard are propagated to our libraries or executables
  target_link_libraries(SO_${EXECUTABLE} ${LIB_NAMES})

  add_executable (${EXECUTABLE} ${SCRIPT})
  target_link_libraries(${EXECUTABLE} SO_${EXECUTABLE})

endforeach()