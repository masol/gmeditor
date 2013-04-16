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

include(FindPkgMacros)
getenv_path(GMEDITOR_DEP_DIRS)

#######################################################################
# Core dependencies
#######################################################################

# Find threading library
FIND_PACKAGE(Threads REQUIRED)

# Find Boost
set(Boost_USE_STATIC_LIBS       ON)
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
set(BOOST_ROOT                  "${BOOST_SEARCH_PATH}")
#set(Boost_DEBUG                 ON)
set(Boost_MINIMUM_VERSION       "1.44.0")

set(Boost_ADDITIONAL_VERSIONS "1.53.0" "1.47.0" "1.46.1" "1.46" "1.46.0" "1.45" "1.45.0" "1.44" "1.44.0")

set(GME_BOOST_COMPONENTS thread filesystem system program_options regex)
find_package(Boost ${Boost_MINIMUM_VERSION} COMPONENTS ${GME_BOOST_COMPONENTS})
if (NOT Boost_FOUND)
        # Try again with the other type of libs
        if(Boost_USE_STATIC_LIBS)
                set(Boost_USE_STATIC_LIBS)
        else()
                set(Boost_USE_STATIC_LIBS OFF)
        endif()
        find_package(Boost ${Boost_MINIMUM_VERSION} COMPONENTS ${GME_BOOST_COMPONENTS})
endif()

if (Boost_FOUND)
	include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
	# Don't use old boost versions interfaces
	ADD_DEFINITIONS(-DBOOST_FILESYSTEM_NO_DEPRECATED)
endif ()


#set(wxWidgets_USE_STATIC 1)
find_package(wxWidgets 2.8 COMPONENTS aui stc html adv core base REQUIRED)
if (wxWidgets_FOUND)
    set(wxUSE_UNICODE_DEFINE "wxUSE_UNICODE=1")
#    set(wxUSE_STATIC_DEFINE "wxUSE_STATIC=1")    
 	IF(MSVC)
 		include_directories( ${wxWidgets_ROOT_DIR}/include/msvc )
 	ENDIF(MSVC)    
   	include_directories(SYSTEM ${wxWidgets_INCLUDE_DIRS})
	
	if(WIN32)
		set(wxWidgets_LIBRARY_DIRS "${wxWidgets_ROOT_DIR}/lib/vc_lib")
	endif()
	link_directories(${wxWidgets_LIBRARY_DIRS})

	STRING(REGEX REPLACE ";" ";-D" wxWidgets_DEFINITIONS "${wxWidgets_DEFINITIONS}")
	SET(wxWidgets_DEFINITIONS "-D${wxWidgets_DEFINITIONS}")	
#    Message(STATUS "wxWidgets_DEFINITIONS : ${wxWidgets_DEFINITIONS}")
#    Message(STATUS "wxWidgets_LIBRARY : ${wxWidgets_LIBRARIES}")
#    mark_as_advanced(wxWidgets_LIBRARIES)
	# Don't use old boost versions interfaces
	ADD_DEFINITIONS(${wxWidgets_DEFINITIONS})
	ADD_DEFINITIONS(-D${wxUSE_UNICODE_DEFINE})
#	ADD_DEFINITIONS(-D${wxUSE_STATIC_DEFINE})
endif ()

# Find FreeImage
find_package(FreeImage)

if (FreeImage_FOUND)
	include_directories(SYSTEM ${FreeImage_INCLUDE_DIRS})
endif ()

find_package(Eigen)

if (EIGEN_FOUND)
	include_directories(${EIGEN_INCLUDE_DIR})
endif ()

find_package(OpenGL)

if (OPENGL_FOUND)
	include_directories(SYSTEM ${OPENGL_INCLUDE_PATH})
endif()


set(OPENCL_ROOT                  "${OPENCL_SEARCH_PATH}")
find_package(OpenCL)
# OpenCL
if (OPENCL_FOUND)
	include_directories(SYSTEM ${OPENCL_INCLUDE_DIR} ${OPENCL_C_INCLUDE_DIR})
endif ()


find_package(SLG)

if (SLG_FOUND)
    include_directories(${SLG_INCLUDE_DIR})
endif()


