# set up the project
# ============================================
cmake_minimum_required(VERSION 3.10)


set(CMAKE_BUILD_TYPE None)
message("cmake version ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}")
message("cmake install prefix: ${CMAKE_INSTALL_PREFIX}")
message("source directory: ${CMAKE_CURRENT_SOURCE_DIR}")

project(G4XS VERSION 1.0
        LANGUAGES CXX
        DESCRIPTION "Pulling cross section data directly from Geant4 data")


set(CMAKE_CXX_STANDARD 14) # same as typically used for SuperSim


# for IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


#list(APPEND CMAKE_PREFIX_PATH "$ENV{G4INSTALL}")

# ============================================
# Geant4

find_package(Geant4 REQUIRED)

if (Geant4_FOUND)
	message(STATUS "Libraries for Geant4 found")
        message(STATUS "Input directory: " ${Geant4_DIR})
	message(STATUS "Package Version: ${Geant4_VERSION}")
        message(STATUS "Include DIRS: ${Geant4_INCLUDE_DIRS}")
else()
	message(FATAL_ERROR "Failed to find Geant4.")
endif()

include("${Geant4_USE_FILE}")


# ============================================
# configure executables to all be put in base build directory
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# ============================================
# add executables
add_executable(G4totals G4totals.cc MinimalUserInitialization.cc)
add_executable(G4diffs G4diffs.cc MinimalUserInitialization.cc)

target_include_directories(G4totals PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(G4diffs PUBLIC "${CMAKE_SOURCE_DIR}/include")

target_include_directories(G4totals PUBLIC "${CMAKE_SOURCE_DIR}")
target_include_directories(G4diffs PUBLIC "${CMAKE_SOURCE_DIR}")

target_link_libraries(G4totals PUBLIC "${Geant4_LIBRARIES}")
target_link_libraries(G4diffs PUBLIC "${Geant4_LIBRARIES}")

