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

###########################################################################
#
# Configuration
#
# Use cmake "-DGME_CUSTOM_CONFIG=YouFileCName" To define your personal settings
# where YouFileCName.cname must exist in one of the cmake include directories
# best to use cmake/SpecializedConfig/
#
# To not load defaults before loading custom values define
# -DGME_NO_DEFAULT_CONFIG=true
#
# WARNING: These variables will be cached like any other
#
###########################################################################

IF (NOT GME_NO_DEFAULT_CONFIG)

  # Disable Boost automatic linking
  # ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB)

  IF (WIN32)

    MESSAGE(STATUS "Using default WIN32 Configuration settings")

    IF(MSVC)

      STRING(REGEX MATCH "(Win64)" _carch_x64 ${CMAKE_GENERATOR})
      IF(_carch_x64)
        SET(WINDOWS_ARCH "x64")
      ELSE(_carch_x64)
        SET(WINDOWS_ARCH "x86")
      ENDIF(_carch_x64)
      MESSAGE(STATUS "Building for target ${WINDOWS_ARCH}")

      SET(MSVC_RT_DEBUG_FIX 1)

    ELSE(MSVC)

    ENDIF(MSVC)

    IF(DEFINED ENV{GME_DEP_DIRS})
        SET(GME_DEP_DIRS $ENV{GMEDITOR_DEP_DIRS})
        MESSAGE(STATUS "gmeditor build environment variables found")
        SET(BOOST_SEARCH_PATH         "${GME_DEP_DIRS}/boost_1_53_0")
        SET(FREEIMAGE_SEARCH_PATH "$ENV{LUX_${WINDOWS_ARCH}_FREEIMAGE_ROOT}/FreeImage")
    ENDIF(DEFINED ENV{GME_DEP_DIRS})

  ELSE(WIN32)
    IF(DEFINED ENV{GME_DEP_DIRS})
        SET(GME_DEP_DIRS $ENV{GMEDITOR_DEP_DIRS})
        MESSAGE(STATUS "gmeditor build environment variables found")
    ENDIF(DEFINED ENV{GME_DEP_DIRS})
  ENDIF(WIN32)

ELSE(NOT GME_NO_DEFAULT_CONFIG)

  MESSAGE(STATUS "GME_NO_DEFAULT_CONFIG defined - not using default configuration values.")

ENDIF(NOT GME_NO_DEFAULT_CONFIG)

# Setup libraries output directory
SET (LIBRARY_OUTPUT_PATH
   ${PROJECT_BINARY_DIR}/lib
   CACHE PATH
   "Single Directory for all Libraries"
   )

# Setup binaries output directory
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
	${PROJECT_BINARY_DIR}/bin
   CACHE PATH
   "Single Directory for all binaries"
	)

#
# Overwrite defaults with Custom Settings
#

IF (GME_CUSTOM_CONFIG)
	MESSAGE(STATUS "Using custom build config: ${GME_CUSTOM_CONFIG}")
	INCLUDE(${GME_CUSTOM_CONFIG})
ENDIF (GME_CUSTOM_CONFIG)


