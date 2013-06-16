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

#ifndef GME_PREFERENCESDIALOG_H
#define GME_PREFERENCESDIALOG_H

#include "wx/propdlg.h"

namespace gme{

class PreferencesDialog : wxPropertySheetDialog
{
DECLARE_CLASS(PreferencesDialog)
public:
	PreferencesDialog(wxWindow *parent);
	~PreferencesDialog(){};

private:
	bool Create(wxWindow *parent);
	wxPanel* CreateLoadSettingPage(wxWindow* parent);
    wxPanel* CreateTestPage(wxWindow* parent);

private:
	enum{
		GID_SET_BEGIN = 0,
		GID_SET_FORCEREFRESH,
		GID_SET_REFRESH_WHEN_ERROR,
		GID_SET_EXPORT_NEWIMG,
		GID_SET_EXPORT_NEWMESH,
		GID_SET_ValidateDataStructure,
		GID_SET_JoinIdenticalVertices,
		GID_SET_RemoveRedundantMaterials,
		GID_SET_ImproveCacheLocality,
		GID_SET_FixInfacingNormals,
		GID_SET_FindDegenerates,
		GID_SET_FindInvalidData,
		GID_SET_FlipUVs,
		GID_SET_OptimizeMeshes,
		GID_SET_Debone,
		GID_SET_END,
		ID_LIST_ENABLE,
		ID_LIST_TEST1,
		ID_LIST_TEST2
	};
DECLARE_EVENT_TABLE()
};

}

#endif // GME_PREFERENCESDIALOG_H
