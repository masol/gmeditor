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


set(OPENCTM_SRCS
    compressMG1.c
    compressMG2.c
    compressRAW.c
    openctm.c
    stream.c
    liblzma/Alloc.c
    liblzma/LzFind.c
    liblzma/LzmaDec.c
    liblzma/LzmaEnc.c
    liblzma/LzmaLib.c
   )
   
include_directories(${GME_SOURCE_DIR}/include/openctm ${GME_SOURCE_DIR}/src/openctm ${GME_SOURCE_DIR}/src/openctm/liblzma)

add_library(openctm STATIC ${OPENCTM_SRCS})
