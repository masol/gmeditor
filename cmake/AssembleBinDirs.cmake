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
# Binary samples directory
#
###########################################################################

IF (WIN32)
	
  # For MSVC moving exe files gets done automatically
  # If there is someone compiling on windows and
  # not using msvc (express is free) - feel free to implement

ELSE (WIN32)
	set(CMAKE_INSTALL_PREFIX .)

ENDIF(WIN32)