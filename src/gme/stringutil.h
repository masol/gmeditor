//////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2013 by sanpolo CO.LTD                                    //
//                                                                          //
//  This file is part of GMEditor                                           //
//                                                                          //
//  GMEditor is free software; you can redistribute it and/or modify it     //
//  under the terms of the LGPL License.                                    //
//                                                                          //
//  GMEditor is distributed in the hope that it will be useful,but WITHOUT  //
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY      //
//  or FITNESS FOR A PARTICULAR PURPOSE.                                    //
//                                                                          //
//  You should have received a copy of the LGPL License along with this     //
//  program.  If not, see <http://www.render001.com/gmeditor/licenses>.     //
//                                                                          //
//  GMEditor website: http://www.render001.com/gmeditor                     //
//////////////////////////////////////////////////////////////////////////////

#ifndef  GME_STRINGUTIL_H
#define  GME_STRINGUTIL_H

//#ifdef  WIN32

#define DECLARE_WXCONVERT           wxMBConvUTF8	gme_wx_utf8_conv
#define gmeWXT(s)                   wxString(__(s),gme_wx_utf8_conv)

//#else
//
//#define DECLARE_WXCONVERT
//#define gmeWXT(s)                   __(s)
//
//#endif

#endif  //GME_STRINGUTIL_H
