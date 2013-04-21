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
# DocumentModel library
#
#############################################################################


set(DM_SRCS
    doc.cpp
    docio.cpp
    docimg.cpp
    doccamera.cpp
    docctl.cpp
    docobj.cpp
    docmat.cpp
    docprivate.cpp
    objectnode.cpp
    slgobject.cpp
    slgmaterial.cpp
    slgtexture.cpp
    setting.cpp
   )


add_library(dm STATIC ${DM_SRCS})
