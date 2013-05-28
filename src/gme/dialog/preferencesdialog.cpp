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
	wxStaticBoxSizer *hsizer =
      new wxStaticBoxSizer (new wxStaticBox (loadPanel, wxID_ANY, wxT("Load Setting")), wxHORIZONTAL );
	{
		hsizer->Add(new wxCheckBox( loadPanel, ID_LOAD_ENABLE, gmeWXT("&Disable")));
		hsizer->Add(new wxCheckBox( loadPanel, ID_LOAD_TEST1, gmeWXT("&Test1")));
		hsizer->Add(new wxCheckBox( loadPanel, ID_LOAD_TEST2, gmeWXT("&Test2")));
	}
	topSizer->Add(hsizer);
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
