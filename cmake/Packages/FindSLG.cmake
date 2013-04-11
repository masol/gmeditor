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
# SLG_DEBUG_LIBRARY, where to find the slg library in debug mode.

FIND_PATH(SLG_INCLUDE_DIR slg/slg.h
  $ENV{GMEDITOR_DEP_DIRS}/luxrays/include
  /usr/local/include
  /usr/include
)

# With Win32, important to have both
IF(WIN32)
  FIND_LIBRARY(SLG_LIBRARY smallluxgpu
               ${SLG_INCLUDE_DIR}/../build/lib
               ${SLG_INCLUDE_DIR}/../lib               
               /usr/local/lib
               /usr/lib)
  FIND_LIBRARY(SLG_DEBUG_LIBRARY smallluxgpud
               ${SLG_INCLUDE_DIR}/../build/lib
               ${SLG_INCLUDE_DIR}/../lib               
               /usr/local/lib
               /usr/lib)
  FIND_LIBRARY(LUXRAYS_LIBRARY luxrays
               ${SLG_INCLUDE_DIR}/../build/lib
               ${SLG_INCLUDE_DIR}/../lib               
               /usr/local/lib
               /usr/lib)
ELSE(WIN32)
  # On unix system, debug and release have the same name
  FIND_LIBRARY(SLG_LIBRARY smallluxgpu
               ${SLG_INCLUDE_DIR}/../build/lib
               ${SLG_INCLUDE_DIR}/../lib               
               /usr/local/lib
               /usr/lib)
  FIND_LIBRARY(SLG_DEBUG_LIBRARY smallluxgpud
               ${SLG_INCLUDE_DIR}/../build/lib
               ${SLG_INCLUDE_DIR}/../lib               
               /usr/local/lib
               /usr/lib)
  FIND_LIBRARY(LUXRAYS_LIBRARY luxrays
               ${SLG_INCLUDE_DIR}/../build/lib
               ${SLG_INCLUDE_DIR}/../lib               
               /usr/local/lib
               /usr/lib)
ENDIF(WIN32)

IF(SLG_INCLUDE_DIR)
  IF(SLG_LIBRARY)
    SET(SLG_FOUND "YES")
    SET(SLG_LIBRARY ${SLG_LIBRARY} ${CMAKE_DL_LIBS})
    SET(SLG_DEBUG_LIBRARY ${SLG_DEBUG_LIBRARY} ${CMAKE_DL_LIBS})
  ENDIF(SLG_LIBRARY)
ENDIF(SLG_INCLUDE_DIR)

MARK_AS_ADVANCED(SLG_FOUND)

