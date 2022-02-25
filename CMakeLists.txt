#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.9 FATAL_ERROR)
project(glass)

#-----------------------
# Set compile definitions G4_V1, G4_V2, G4_V3 to Geant version from the Farm environment variable
# We need this in RunManager.cc to hide part of the code from being compiled on the older Geant4 version

execute_process(COMMAND geant4-config --version OUTPUT_VARIABLE G4V)
if (G4V MATCHES "([0-9]+)\\.([0-9]+)\\.([0-9]+)")
	MESSAGE(STATUS "Determined Geant4 version: ${G4V}")

	STRING(REGEX REPLACE "^([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\1;\\2;\\3" RESULT "${G4V}")

	# MESSAGE("${RESULT}")
	list(GET RESULT 0 V1)
	list(GET RESULT 1 V2)

	MESSAGE(STATUS "Setting C++ compile definitions: V1=${V1}, V2=${V2}")

	add_compile_definitions(G4_V1=${V1})
	add_compile_definitions(G4_V2=${V2})
else()
	MESSAGE(WARNING "Geant4 version cannot be determined")
endif()

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Find ROOT (required package)
# Petr Stepanov: Find ROOT with RooFit https://root.cern/manual/integrate_root_into_my_cmake_project/
#
find_package(ROOT REQUIRED COMPONENTS RooFit RooFitCore Gui)

# Manually append extra ROOT libraries (why missing?)
#  list(APPEND LIB_NAMES "ROOT::Gui")
#  list(APPEND LIB_NAMES "ROOT::RooFit")
#  list(APPEND LIB_NAMES "ROOT::RooFitCore")
#  list(APPEND LIB_NAMES "ROOT::Html")
#  list(APPEND LIB_NAMES "ROOT::Minuit")
#  list(APPEND LIB_NAMES "ROOT::Fumili")

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIR})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Attach ROOT and Geant Sources
# https://stackoverflow.com/questions/9581358/using-cmake-to-index-source-files-of-an-external-library-with-eclipse

# Function searches for the C++ source files in the given folder
# Next creates a subfolder in the project source folder and symlinks there all found sources
function(attachExternalSources librarySourceLocation folderName)
	# Create folder for Geant4 sources
	file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/${folderName})

	message(STATUS "Searching for C++ sources in\n   \"${librarySourceLocation}\"")
	FILE(GLOB_RECURSE libSources
		${librarySourceLocation}/*.c
		${librarySourceLocation}/*.cpp
		${librarySourceLocation}/*.cxx
		${librarySourceLocation}/*.cc
	)

	message(STATUS "Symlinking sources into\n   \"${CMAKE_SOURCE_DIR}/${folderName}\"\n   Please wait...")
	foreach(source ${libSources})
		# Obtain source filename
		get_filename_component(source_filename ${source} NAME)

		# Create symlink unless it already exists
		set(symlink "${CMAKE_SOURCE_DIR}/${folderName}/${source_filename}")
		if(NOT EXISTS ${symlink})
			execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${symlink})
		endif()
	endforeach()

	# Scan all the symlinks created under the project folder and disable their compilation
	FILE(GLOB sources_symlinks ${CMAKE_SOURCE_DIR}/${folderName}/*)
	SET_SOURCE_FILES_PROPERTIES(${sources_symlinks} PROPERTIES HEADER_FILE_ONLY TRUE)
endfunction()

if(DEFINED ROOT_SOURCE_LOCATION)
	# Expand ROOT source path variables (wildcard)
	FILE(GLOB ROOT_SOURCE_LOCATIONS_EXPAND "${ROOT_SOURCE_LOCATION}")
	# Get last GLOB element
	list(GET ROOT_SOURCE_LOCATIONS_EXPAND -1 ROOT_SOURCE_LOCATION_EXPAND)
	message(STATUS "Found CERN ROOT source code folder:\n   \"${ROOT_SOURCE_LOCATION_EXPAND}\"")
	attachExternalSources(${ROOT_SOURCE_LOCATION_EXPAND} "root-sources")
else()
	message(STATUS "CERN ROOT source folder location not provided")
endif()

if(DEFINED GEANT4_SOURCE_LOCATION)
	# Expand GEANT4 source path variables (wildcard)
	FILE(GLOB GEANT4_SOURCE_LOCATIONS_EXPAND "${GEANT4_SOURCE_LOCATION}")
	# Get last GLOB element
	list(GET GEANT4_SOURCE_LOCATIONS_EXPAND -1 GEANT4_SOURCE_LOCATION_EXPAND)
	message(STATUS "Found GEANT4 source code folder:\n   \"${GEANT4_SOURCE_LOCATION_EXPAND}\"")
	attachExternalSources(${GEANT4_SOURCE_LOCATION_EXPAND} "geant4-sources")
else()
	message(STATUS "Geant4 source folder location not provided")
endif()

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(glass glass.cc ${sources} ${headers})
target_link_libraries(glass ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})


#----------------------------------------------------------------------------
# Add workaround to support std::string_view
# https://crascit.com/2015/03/28/enabling-cxx11-in-cmake/
# target_compile_features(glass PUBLIC cxx_std_17)
# set_property(TARGET glass PROPERTY CXX_STANDARD 14)
target_compile_features(glass PUBLIC cxx_std_14)

#----------------------------------------------------------------------------
# Build the draw scripts (basically for Eclipse to treat them as a part of project)
#
add_subdirectory (draw)
#----------------------------------------------------------------------------

function(copyFiles sourceFiles)
  foreach(sourceFile ${sourceFiles})
    # Replace source path with destination path
    string(REGEX REPLACE "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}" destFile ${sourceFile})
    # Copy file to build folder
    configure_file(
      ${sourceFile} ${destFile}
      COPYONLY
    )
  endforeach()
endfunction()

#----------------------------------------------------------------------------
# Copy all scripts to the build directory. Easier to pass macros to executable.
file(GLOB_RECURSE glass_MACROS ${PROJECT_SOURCE_DIR}/macros/*.mac)
message(STATUS "Found list of Geant4 Macros: ${glass_MACROS}")
copyFiles("${glass_MACROS}")

#----------------------------------------------------------------------------
# Copy data analysis ROOT scripts to the build directory.

file(GLOB_RECURSE glass_DRAW ${PROJECT_SOURCE_DIR}/draw/*.root)
message(STATUS "Found list of ROOT Scripts: ${glass_DRAW}")
copyFiles("${glass_DRAW}")

#----------------------------------------------------------------------------
# Copy Slurm scripts to the build directory.

file(GLOB_RECURSE glass_SLURM ${PROJECT_SOURCE_DIR}/slurm/*.sh)
message(STATUS "Found list of ROOT Scripts: ${glass_SLURM}")
copyFiles("${glass_SLURM}")

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS glass DESTINATION bin)
