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

#include "config.h"
#include "utils/i18n.h"
#include "../stringutil.h"

#include "wx/wx.h"
#include "wx/bookctrl.h"
#include "preferencesdialog.h"

namespace gme{

IMPLEMENT_CLASS(PreferencesDialog, wxPropertySheetDialog)

BEGIN_EVENT_TABLE(PreferencesDialog, wxPropertySheetDialog)
END_EVENT_TABLE()

PreferencesDialog::PreferencesDialog(wxWindow *parent)
{
	Create(parent);
}

bool
PreferencesDialog::Create(wxWindow *parent)
{
	DECLARE_WXCONVERT;
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP|wxWS_EX_VALIDATE_RECURSIVELY);
	if (!wxPropertySheetDialog::Create(parent,
				wxID_ANY,
				gmeWXT("Preferences"),
				wxDefaultPosition, wxDefaultSize,
				wxDEFAULT_DIALOG_STYLE| (int)wxPlatform::IfNot(wxOS_WINDOWS_CE, wxRESIZE_BORDER))
	)
		return false;
	CreateButtons(wxOK|wxCANCEL);
	// add page
	wxBookCtrlBase* notebook = GetBookCtrl();

    wxPanel* loadSettings = CreateLoadSettingPage(notebook);
    wxPanel* testSettings = CreateTestPage(notebook);

    notebook->AddPage(loadSettings, gmeWXT("加载设定"), true);
    notebook->AddPage(testSettings, gmeWXT("Test"), false);

    LayoutDialog();
	this->ShowModal();
	return true;
}

wxPanel* 
PreferencesDialog::CreateLoadSettingPage(wxWindow* parent)
{
	DECLARE_WXCONVERT;
	wxPanel *loadPanel = new wxPanel(parent);
	wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
	
	// refresh setting
	{
		wxStaticBoxSizer *hsizer =
		  new wxStaticBoxSizer (new wxStaticBox (loadPanel, wxID_ANY, wxT("refresh Setting")), wxHORIZONTAL );
		wxCheckBox *forceCheck = new wxCheckBox( loadPanel, GID_SET_FORCEREFRESH, gmeWXT("&强制刷新"));
		forceCheck->SetValue(true);
		hsizer->Add(forceCheck, wxSizerFlags(1).Border());

		wxCheckBox *errorCheck = new wxCheckBox( loadPanel, GID_SET_REFRESH_WHEN_ERROR, gmeWXT("&错误时强制刷新"));
		errorCheck->SetValue(true);
		hsizer->Add(errorCheck, wxSizerFlags(1).Border());

		topSizer->Add(hsizer, wxSizerFlags().Expand().Border());
	}
	
	// import setting
	{
		//wxSizer *vSizer = new wxStaticBoxSizer(wxVERTICAL, this, gmeWXT("Import Setting"));
		wxSizer *vSizer = new wxStaticBoxSizer(new wxStaticBox (loadPanel, wxID_ANY, wxT("Import Setting")), wxHORIZONTAL );
		wxFlexGridSizer *hsizer = new wxFlexGridSizer (3, 5, 20);
		hsizer->AddGrowableCol(0);
		hsizer->AddGrowableCol(1);

		hsizer->Add(new wxCheckBox( loadPanel, GID_SET_ValidateDataStructure, gmeWXT("数据有效性校正")));
		hsizer->Add(new wxCheckBox( loadPanel, GID_SET_JoinIdenticalVertices, gmeWXT("合并相同顶点")));
		hsizer->Add(new wxCheckBox( loadPanel, GID_SET_RemoveRedundantMaterials, gmeWXT("移除冗余材质")));
		hsizer->Add(new wxCheckBox( loadPanel, GID_SET_ImproveCacheLocality, gmeWXT("提升缓冲效率")));
		hsizer->Add(new wxCheckBox( loadPanel, GID_SET_FixInfacingNormals, gmeWXT("校正反转法线")));
		hsizer->Add(new wxCheckBox( loadPanel, GID_SET_FindDegenerates, gmeWXT("退化校正")));
		hsizer->Add(new wxCheckBox( loadPanel, GID_SET_FindInvalidData, gmeWXT("无效数据校正")));
		hsizer->Add(new wxCheckBox( loadPanel, GID_SET_FlipUVs, gmeWXT("UV反转")));
		hsizer->Add(new wxCheckBox( loadPanel, GID_SET_OptimizeMeshes, gmeWXT("模型合并")));
		hsizer->Add(new wxCheckBox( loadPanel, GID_SET_Debone, gmeWXT("骨骼解绑")));
		
		vSizer->Add(hsizer, wxSizerFlags(1).Expand());
		topSizer->Add(vSizer, wxSizerFlags().Expand().Border());
	}

	// normals
	{
		const wxString normals[] =
		{
			gmeWXT("产生顶点法线"),
			gmeWXT("忽略法线"),
			gmeWXT("模型法线")
		};
		wxRadioBox *radio = new wxRadioBox(loadPanel, wxID_ANY, "Normals Setting",
                             wxDefaultPosition, wxDefaultSize,
                             WXSIZEOF(normals), normals,
                             1, wxRA_SPECIFY_ROWS);
		radio->SetSelection(0);
		topSizer->Add(radio, wxSizerFlags().Expand().Border());
	}
	
	// export setting
	{
		wxStaticBoxSizer *hsizer =
		  new wxStaticBoxSizer (new wxStaticBox (loadPanel, wxID_ANY, wxT("Export Setting")), wxHORIZONTAL );
		hsizer->Add(new wxCheckBox( loadPanel, GID_SET_EXPORT_NEWIMG, gmeWXT("导出新贴图")),wxSizerFlags(1).Border());
		hsizer->Add(new wxCheckBox( loadPanel, GID_SET_EXPORT_NEWMESH, gmeWXT("导出新模型")), wxSizerFlags(1).Border());
		topSizer->Add(hsizer, wxSizerFlags().Expand().Border());
	}

	loadPanel->SetSizerAndFit(topSizer);
	return loadPanel;
}

wxPanel* 
PreferencesDialog::CreateTestPage(wxWindow* parent)
{
	DECLARE_WXCONVERT;
	wxPanel *listPanel = new wxPanel(parent);
	wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
	topSizer->Add(new wxCheckBox( listPanel, ID_LIST_ENABLE, gmeWXT("&List Disable")));
	topSizer->Add(new wxCheckBox( listPanel, ID_LIST_TEST1, gmeWXT("&List Test1")));
	topSizer->Add(new wxCheckBox( listPanel, ID_LIST_TEST2, gmeWXT("&List Test2")));
	listPanel->SetSizerAndFit(topSizer);
	return listPanel;
}

}
