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

#ifndef  GME_CMDIDS_H
#define  GME_CMDIDS_H

namespace gme{

namespace cmd{

enum{
    GID_BEGIN = wxID_HIGHEST+1,
    GID_REFRESH_OBJVIEW,
    GID_EXPORT,
    GID_IMPORT,
    GID_PANE_BEGIN,
    GID_PANE_OBJECTVIEW,
	GID_PANE_PROPVIEW,
    GID_PANE_END,
	GID_RENDER_START,
	GID_RENDER_STOP,
	GID_RENDER_PAUSE,
	GID_VM_BEGIN,
	GID_VM_ADJDOC,
	GID_VM_DOCSIZE,
	GID_VM_FULLWINDOW,
	GID_VM_SCALEWITHASPECT,
	GID_VM_END,
	GID_MAX
};

} //end namespace cmd
} //end namepsace gme


#endif //GME_CMDIDS_H
