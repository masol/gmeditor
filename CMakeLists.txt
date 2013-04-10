###########################################################################
#   Copyright (C) 2013 by sanpolo CO.LTD                                  #
#                                                                         #
#   This file is part of GMEditor.                                        #
#                                                                         #
#   GMEditor is free software; you can redistribute it and/or modify it   #
#   under the terms of the LGPL License.                                  #
#                                                                         #
#   GMEditor is distributed in the hope that it will be useful,but WITHOUT#
#   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY    #
#   or FITNESS FOR A PARTICULAR PURPOSE.                                  #
#                                                                         #
#   You should have received a copy of the LGPL License along with this   #
#   program.  If not, see <http://www.render001.com/gmeditor/licenses>.   #
#                                                                         #
#   GMEditor website: http://www.render001.com/gmeditor                   #
###########################################################################

MESSAGE(STATUS "CMAKE VERSION DETECTED " ${CMAKE_VERSION})

###########################################################################
#
# Check and configure cmake
#
###########################################################################
# Fresh start
# we use copy internal macro -- this is supported by cmake 2.8.
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)
#Remove the following when the version check is at least 2.8.4
SET(CMAKE_LEGACY_CYGWIN_WIN32 0)

project(gmeditor)

# Dont overconfigure
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limit configs" FORCE)

###########################################################################
#
# Include necessary submodules
#
###########################################################################

set(CMAKE_MODULE_PATH
	"${PROJECT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/cmake"
	"${PROJECT_SOURCE_DIR}/cmake/Utils"
	"${PROJECT_SOURCE_DIR}/cmake/Packages"
	"${PROJECT_SOURCE_DIR}/cmake/SpecializedConfig"
)

INCLUDE(Configuration)
INCLUDE(PlatformSpecific)

# Install CMake modules
#add_subdirectory(CMake)


SET(GME_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
SET(GME_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}")

SET(GME_INCLUDE_DIR "${GME_SOURCE_DIR}/include")
include_directories("${GME_INCLUDE_DIR}")
include_directories("${GME_BUILD_DIR}/include")

# When using single configuration generators like make
# cmake does need to know which of the possible configurations
# to generate
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)
SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "assure config" FORCE) # makes sure type is shown in cmake gui

message(STATUS "Building mode: " ${CMAKE_BUILD_TYPE})

# Find dependencies
include(Dependencies)

if (NOT Boost_FOUND)
	MESSAGE(FATAL_ERROR "--> Could not locate required Boost files - Please check ${BOOST_SEARCH_PATH}")
endif()

set(GME_VERSION_MAJOR 0)
set(GME_VERSION_MINOR 1)

# Configure a header file to pass some of the CMake settings
# to the source code
configure_file(
  "${GME_SOURCE_DIR}/include/config.h.in"
  "${GME_BUILD_DIR}/include/config.h"
  )

add_subdirectory(src/utils)
add_subdirectory(src/gme)

###########################################################################
#
# For non win32 we'll have to copy everything to a single dir
#
###########################################################################

INCLUDE(AssembleBinDirs)
