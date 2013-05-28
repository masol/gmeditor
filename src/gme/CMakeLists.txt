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
# ICU Resource Bundle used by gme.
#
#############################################################################
set(GME_RESBUNDLE_INPUT_DIR ${GME_SOURCE_DIR}/i18n)

IF (WIN32)
        SET(GME_RESBUNDLE_OUTPUT_BASEDIR ${GME_BUILD_DIR}/bin/$(ConfigurationName)/i18n)
        SET(GME_RESBUNDLE_OUTPUT_DIR ${GME_RESBUNDLE_OUTPUT_BASEDIR}/zh_CN/LC_MESSAGES)
		STRING(REGEX REPLACE "/" "\\\\" GME_RESBUNDLE_OUTPUT_DIR ${GME_RESBUNDLE_OUTPUT_DIR})
        #STRING(REGEX REPLACE "/" "\\\\" GME_RESBUNDLE_OUTPUT ${GME_RESBUNDLE_OUTPUT})
		STRING(REGEX REPLACE "/" "\\\\" GME_RESBUNDLE_INPUT_DIR ${GME_RESBUNDLE_INPUT_DIR})
		#SET(RESBUNDLE_CREATE_OUTPUT_DIR if exist "${GME_RESBUNDLE_OUTPUT_DIR}" echo "dir exist" else mkdir -p "${GME_RESBUNDLE_OUTPUT_DIR}")
ELSE()
        SET(GME_RESBUNDLE_OUTPUT_BASEDIR ${GME_BUILD_DIR}/bin/i18n)
        SET(GME_RESBUNDLE_OUTPUT_DIR ${GME_RESBUNDLE_OUTPUT_BASEDIR}/zh_CN/LC_MESSAGES)
ENDIF(WIN32)

        SET(GME_RESBUNDLE_OUTPUT ${GME_RESBUNDLE_OUTPUT_DIR}/gme.mo)

add_custom_command(
		OUTPUT ${GME_RESBUNDLE_OUTPUT}
		# Creating target directory
		COMMAND ${CMAKE_COMMAND} -E make_directory ${GME_RESBUNDLE_OUTPUT_DIR}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${GME_RESBUNDLE_OUTPUT_BASEDIR}/en_US/LC_MESSAGES
		#COMMAND "${RESBUNDLE_CREATE_OUTPUT_DIR}"
		COMMAND msgfmt -c -o ${GME_RESBUNDLE_OUTPUT_BASEDIR}/zh_CN/LC_MESSAGES/gme.mo ${GME_RESBUNDLE_INPUT_DIR}/zh_CN.po
		COMMAND msgfmt -c -o ${GME_RESBUNDLE_OUTPUT_BASEDIR}/en_US/LC_MESSAGES/gme.mo ${GME_RESBUNDLE_INPUT_DIR}/en_US.po
		WORKING_DIRECTORY ${GME_BUILD_DIR}
	)

add_custom_target(gme_res DEPENDS ${GME_RESBUNDLE_OUTPUT})

#############################################################################
#
# GME binary
#
#############################################################################
set(wxWidgets_LIBRARIES_Cache ${wxWidgets_LIBRARIES})
set(GME_SRCS
	propgrid.cpp
    renderview.cpp
    objectview.cpp
    mainframe.cpp
	gmestatus.cpp
	main.cpp
	filedialog.cpp
	cameraview.cpp
	data/xpmres.cpp
	dialog/preferencesdialog.cpp
   )

set(GME_GL_SRC
    glrenderview.cpp
  	glrender/glrenderframe.cpp
  )

set(GME_PROP_SRC
    property/hdrpage.cpp
    property/materialpage.cpp
    property/envlightpage.cpp
	property/pgeditor.cpp
  )


if(wxWidgets_CXX_FLAGS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${wxWidgets_CXX_FLAGS}")
    Message(STATUS "wxWidgets_CXX_FLAGS=${wxWidgets_CXX_FLAGS}")
endif(wxWidgets_CXX_FLAGS)

add_executable(gmeditor ${GME_SRCS} ${GME_GL_SRC} ${GME_PROP_SRC})

IF (WIN32)
TARGET_LINK_LIBRARIES(gmeditor
 dm
 utils
 openctm
# ${GLEW_LIBRARY}
# ${GLUT_LIBRARY}
 # ${ICU_LIBRARIES}
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
#  ${GLEW_LIBRARY}
#  ${GLUT_LIBRARY}
  # ${ICU_LIBRARIES}
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

