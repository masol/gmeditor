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
#ifdef PROPERTY_HAS_DELETE_PROPERTY
#else
	GID_REFRESH_MATPROP,
#endif
    GID_EXPORT,
    GID_IMPORT,
    GID_SAVE_IMAGE,
    GID_SAVE_MATERIAL,
    GID_EXPORT_MATERIAL,
    GID_IMPORT_MAEERIAL,
    GID_IMPORT_GLUE_MATERIAL,
    GID_EXPORT_GLUE_MATERIAL,
	GID_PREFERENCES,
    GID_PANE_BEGIN,     //切换辅助pane.
    GID_PANE_OBJECTVIEW,
	GID_PANE_PROPVIEW,
	GID_PANE_CAMVIEW,
	GID_PANE_SYSLOG,
    GID_PANE_END,
	GID_RENDER_START,
	GID_RENDER_STOP,
	GID_RENDER_PAUSE,
	GID_VM_BEGIN,       //glviewmode显示模式。
	GID_VM_ADJDOC,
	GID_VM_DOCSIZE,
	GID_VM_FULLWINDOW,
	GID_VM_SCALEWITHASPECT,
	GID_VM_END,
	GID_MD_START,   //glviewmode编辑模式。
	GID_MD_LOCK,
	GID_MD_PANE,
	GID_MD_ROTATE,
	GID_MD_ROTATE_AROUND_FOCUS,
	GID_MD_ZOOM,
	GID_MD_SELECT,
	GID_MD_SETSUNLIGHT,
	GID_MD_END,
	GID_AUTO_TARGET,
	GID_AUTO_FOCUS,
	GID_SB_SE_TIME, //状态栏设置时间终止条件。
	GID_SB_SE_PASS, //状态栏设置PASS终止条件。
	GID_SB_SE_CONVERGENCE, //状态栏设置CONVERGENCE终止条件。
	GID_SB_SE_CLEARALL,     //清空终止条件。
	GID_SB_RI_BEGIN,
	GID_SB_RI_NATIVE,
	GID_SB_RI_CONTRIBUTE,
	GID_SB_RI_TOTAL,
	GID_SB_RI_END,
    GID_IMM_REFRESH,
    GID_SET_BEGIN,
    GID_SET_FORCEREFRESH,
    GID_SET_ValidateDataStructure,
    GID_SET_GenSmoothNormals,
    GID_SET_JoinIdenticalVertices,
    GID_SET_RemoveRedundantMaterials,
    GID_SET_ImproveCacheLocality,
    GID_SET_FixInfacingNormals,
    GID_SET_FindDegenerates,
    GID_SET_FindInvalidData,
    GID_SET_FlipUVs,
    GID_SET_OptimizeMeshes,
    GID_SET_Debone,
    GID_SET_IgnoreNormals,
    GID_SET_CommonNormals,
    GID_SET_END,
	GID_LOG_BEGIN,
	GID_LOG_CLOSE,
	GID_LOG_TRACE,
	GID_LOG_DEBUG,
	GID_LOG_VERBOSE,
	GID_LOG_MESSAGE,
	GID_LOG_STATUS,
	GID_LOG_WARNING,
	GID_LOG_ERROR,
	GID_LOG_SYSERROR,
	GID_LOG_FATALERROR,
	GID_LOG_END,
    GID_CLEAR_LOG,
	GID_VIEWALL,
	GID_SETTARGET,
	GID_VIEWSELECTION,
	GID_VIEWSKYLIGHTDIR,
	GID_CAM_NEWFROMCURRENT,
	GID_CAM_DELETE,
	GID_MAX
};

} //end namespace cmd
} //end namepsace gme


#endif //GME_CMDIDS_H
