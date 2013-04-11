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

#
# Find the SLG includes and library
#
# This module defines
# SLG_INCLUDE_DIR, where to find slg.h, etc.
# SLG_LIBRARY, the libraries to link against to use slg.
# LUXRAYS_LIBRARY, the libraries to link against to use luxrays.
# SLG_FOUND, If false, do not try to use slg.

# also defined, but not for general use are
# SLG_LIBRARY_DBG, where to find the slg library in debug mode.
# LUXRAYS_LIBRARY_DBG, where to find the luxrays library in debug mode.
# SLG_LIBRARY_REL, where to find the slg library in release mode.
# LUXRAYS_LIBRARY_REL, where to find the luxrays library in release mode.

FIND_PATH(SLG_INCLUDE_DIR slg/slg.h
  $ENV{GMEDITOR_DEP_DIRS}/luxrays/include
  /usr/local/include
  /usr/include
)

# With Win32, important to have both
IF(WIN32)
  FIND_LIBRARY(SLG_LIBRARY_REL smallluxgpu
               ${SLG_INCLUDE_DIR}/../build/lib/Release
               ${SLG_INCLUDE_DIR}/../lib               
               /usr/local/lib
               /usr/lib)
  FIND_LIBRARY(SLG_LIBRARY_DBG smallluxgpu
               ${SLG_INCLUDE_DIR}/../build/lib/Debug
               ${SLG_INCLUDE_DIR}/../lib               
               /usr/local/lib
               /usr/lib)

  FIND_LIBRARY(LUXRAYS_LIBRARY_REL luxrays
               ${SLG_INCLUDE_DIR}/../build/lib/Release
               ${SLG_INCLUDE_DIR}/../lib               
               /usr/local/lib
               /usr/lib)
  FIND_LIBRARY(LUXRAYS_LIBRARY_DBG luxrays
               ${SLG_INCLUDE_DIR}/../build/lib/Debug
               ${SLG_INCLUDE_DIR}/../lib               
               /usr/local/lib
               /usr/lib)
ELSE(WIN32)
  # On unix system, debug and release have the same name
  FIND_LIBRARY(SLG_LIBRARY_REL smallluxgpu
               ${SLG_INCLUDE_DIR}/../build/lib
               ${SLG_INCLUDE_DIR}/../lib               
               /usr/local/lib
               /usr/lib)
  FIND_LIBRARY(SLG_LIBRARY_DBG smallluxgpu
               ${SLG_INCLUDE_DIR}/../build/lib
               ${SLG_INCLUDE_DIR}/../lib               
               /usr/local/lib
               /usr/lib)

  FIND_LIBRARY(LUXRAYS_LIBRARY_REL luxrays
               ${SLG_INCLUDE_DIR}/../build/lib
               ${SLG_INCLUDE_DIR}/../lib               
               /usr/local/lib
               /usr/lib)
  FIND_LIBRARY(LUXRAYS_LIBRARY_DBG luxrays
               ${SLG_INCLUDE_DIR}/../build/lib
               ${SLG_INCLUDE_DIR}/../lib               
               /usr/local/lib
               /usr/lib)
ENDIF(WIN32)

IF(SLG_INCLUDE_DIR)
	SET(SLG_FOUND "YES")
	SET(SLG_LIBRARY
		optimized ${SLG_LIBRARY_REL}
		debug ${SLG_LIBRARY_DBG})
	SET(LUXRAYS_LIBRARY
		optimized ${LUXRAYS_LIBRARY_REL}
		debug ${LUXRAYS_LIBRARY_DBG})
ENDIF(SLG_INCLUDE_DIR)

MARK_AS_ADVANCED(SLG_FOUND)

