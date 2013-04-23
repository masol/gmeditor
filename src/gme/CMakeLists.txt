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

#############################################################################
#
# GME binary
#
#############################################################################

#Message(STATUS "wxWidgets_LIBRARY : ${wxWidgets_LIBRARIES}")

#@FIXME: wxWidgets_LIBRARIES will been overwrite by some instruction.
set(wxWidgets_LIBRARIES_Cache ${wxWidgets_LIBRARIES})
set(GME_SRCS
	propgrid.cpp
    renderview.cpp
    objectview.cpp
    mainframe.cpp
	main.cpp
   )

if(wxWidgets_CXX_FLAGS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${wxWidgets_CXX_FLAGS}")
    Message(STATUS "wxWidgets_CXX_FLAGS=${wxWidgets_CXX_FLAGS}")
endif(wxWidgets_CXX_FLAGS)

add_executable(gmeditor ${GME_SRCS})

IF (WIN32)
TARGET_LINK_LIBRARIES(gmeditor
 dm
 utils
 openctm
 ${ASSIMP_LIBRARIES}
 ${FreeImage_LIBRARIES}
 ${SLG_LIBRARY}
 ${LUXRAYS_LIBRARY}
 ${OPENGL_LIBRARY}
 ${OPENCL_LIBRARIES}
)
set_target_properties(gmeditor PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
ELSE(WIN32)
#Message(STATUS "OPENCL_LIBRARIES : ${OPENCL_INCLUDE_DIR}")
TARGET_LINK_LIBRARIES(gmeditor
  dm
  utils
  openctm
  ${ASSIMP_LIBRARIES}
  ${wxWidgets_LIBRARIES_Cache}
  ${FreeImage_LIBRARIES}
  ${SLG_LIBRARY}
  ${LUXRAYS_LIBRARY}
  ${Boost_LIBRARIES}
  ${OPENCL_LIBRARIES}
  ${OPENGL_gl_LIBRARY}
)
ENDIF(WIN32)

