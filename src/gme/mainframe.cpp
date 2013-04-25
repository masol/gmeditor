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
//  program.  iiiiIf not, see <http://www.render001.com/gmeditor/licenses>.     //
//                                                                          //
//  GMEditor website: http://www.render001.com/gmeditor                     //
//////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include "mainframe.h"
#include "renderview.h"
#include "objectview.h"
#include "stringutil.h"
#include "cmdids.h"
#include "dm/docio.h"
#include "dm/docobj.h"
#include <boost/locale.hpp>
#include "propgrid.h"
#include "data/xpmres.h"

namespace gme{

BEGIN_EVENT_TABLE(MainFrame, inherited)
	EVT_MENU(wxID_OPEN, MainFrame::onMenuFileOpen)
	EVT_MENU(wxID_SAVE, MainFrame::onMenuFileSave)
	EVT_MENU(cmd::GID_EXPORT, MainFrame::onMenuFileExport)
	EVT_MENU(cmd::GID_IMPORT, MainFrame::onMenuFileImport)
	EVT_MENU(wxID_EXIT, MainFrame::onMenuFileQuit)
	EVT_MENU(wxID_DELETE, MainFrame::onMenuEditDelete)
	EVT_MENU(wxID_ABOUT, MainFrame::onMenuHelpAbout)
	EVT_MENU(cmd::GID_PROP, MainFrame::onShowPropertyPane)
	EVT_SIZE(MainFrame::onSize)
	EVT_CLOSE(MainFrame::onClose)

END_EVENT_TABLE()

MainFrame::MainFrame(wxWindow* parent) : wxFrame(parent, -1, _("GMEditor"),
                  wxDefaultPosition, wxSize(800,600),
                  wxDEFAULT_FRAME_STYLE)
{
    m_pGauge = NULL;
    createMenubar();
	createToolbar();
    createStatusbar();

    DECLARE_WXCONVERT;

    // notify wxAUI which frame to use
    m_mgr.SetManagedWindow(this);

    // create several text controls
    //wxTextCtrl *text1 = new wxTextCtrl(this, -1, _("Pane 1 - sample text"),wxDefaultPosition, wxSize(200,150),wxNO_BORDER | wxTE_MULTILINE);

    wxTextCtrl *text2 = new wxTextCtrl(this, -1, gmeWXT("Pane 2 - sample text"),
                  wxDefaultPosition, wxSize(200,150),
                  wxNO_BORDER | wxTE_MULTILINE);
	wxLog::SetActiveTarget(new wxLogTextCtrl(text2));
    m_objectView = new ObjectView(this,wxID_ANY,wxDefaultPosition,wxSize(200,450));

    // add the panes to the manager
	wxMBConvUTF8	conv;
    m_mgr.AddPane(m_objectView, wxLEFT, gmeWXT("模型一览"));
    m_mgr.AddPane(text2, wxBOTTOM, gmeWXT("Pane Number Two"));


    RenderView        *mainView = new RenderView(this);
    m_mgr.AddPane(mainView, wxCENTER);

	int propFrameStyle = wxNO_BORDER | \
						 wxCLIP_CHILDREN;
	// create and show propery pane
	m_propFrame = new PropFrame(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, propFrameStyle);
	m_mgr.AddPane(m_propFrame, wxRIGHT, gmeWXT("属性设置"));
	m_mgr.GetPane(m_propFrame).Hide();
    // tell the manager to "commit" all the changes just made
    m_mgr.Update();
}

MainFrame::~MainFrame()
{
}

void
MainFrame::createMenubar()
{
    DECLARE_WXCONVERT;

    wxMenuBar *pMenuBar = new wxMenuBar();

    {//File
        wxMenu *pFileMenu = new wxMenu();
        pFileMenu->Append(wxID_OPEN, gmeWXT("打开(&O)"), gmeWXT("打开已有场景"));
        pFileMenu->Append(cmd::GID_IMPORT, gmeWXT("导入(&I)"), gmeWXT("从文件中导入模型到当前场景"));
        pFileMenu->AppendSeparator();
        pFileMenu->Append(wxID_SAVE, gmeWXT("保存(&S)"), gmeWXT("保存现有场景"));
        pFileMenu->Append(cmd::GID_EXPORT, gmeWXT("导出(&E)"), gmeWXT("导出现有场景"));
        pFileMenu->AppendSeparator();
        pFileMenu->Append(wxID_EXIT, gmeWXT("退出(&X)"), gmeWXT("退出gmeditor"));

        pMenuBar->Append(pFileMenu, gmeWXT("文件(&F)"));
    }

    {//Edit
        wxMenu *pEditMenu = new wxMenu();
        pEditMenu->Append(wxID_DELETE, gmeWXT("删除(&D)"), gmeWXT("删除选中模型"));

        pMenuBar->Append(pEditMenu, gmeWXT("编辑(&E)"));
    }

	{// view menu
		wxMenu *pViewMenu = new wxMenu();
		pViewMenu->Append(cmd::GID_PROP, gmeWXT("属性设置(&P)"), gmeWXT("显示属性面板"));

		pMenuBar->Append(pViewMenu, gmeWXT("视图(&V)"));
	}

	{//Help
		wxMenu *pHelpMenu = new wxMenu();
		pHelpMenu->Append(wxID_ABOUT,gmeWXT("关于(&A)"),gmeWXT("关于我们的信息"));

		pMenuBar->Append(pHelpMenu,gmeWXT("帮助(&H)"));
	}

	SetMenuBar(pMenuBar);
}

void
MainFrame::createToolbar()
{
	DECLARE_WXCONVERT;

	wxToolBar *pToolBar = CreateToolBar();

	wxBitmap bmpOpen(xpm::open);
	wxBitmap bmpSave(xpm::save);
	wxBitmap bmpHelp(xpm::help);
	wxBitmap bmpDel(xpm::_delete);

	pToolBar->AddTool(wxID_OPEN,bmpOpen,gmeWXT("打开&O"),gmeWXT("打开已有场景"));
	pToolBar->AddTool(cmd::GID_IMPORT,bmpOpen,gmeWXT("导入(&I)"),gmeWXT("从文件中导入模型到当前场景"));
	pToolBar->AddSeparator();


	pToolBar->AddTool(wxID_SAVE,bmpSave,gmeWXT("保存&S"),gmeWXT("保存已有场景"));
	pToolBar->AddTool(cmd::GID_EXPORT,bmpSave,gmeWXT("导出(&E)"),gmeWXT("导出现有场景"));
	pToolBar->AddSeparator();

	pToolBar->AddTool(wxID_DELETE,bmpDel,gmeWXT("删除(&D)"),gmeWXT("删除选中的模型"));

	pToolBar->AddStretchableSpace();
    pToolBar->AddTool(wxID_ABOUT,bmpHelp,gmeWXT("关于&A"),gmeWXT("关于我们"));

	pToolBar->Realize();
}

void
MainFrame::createStatusbar()
{
    DECLARE_WXCONVERT;
    wxStatusBar *pStatusBar = CreateStatusBar(SFP_TOTOAL);
    int w[SFP_TOTOAL] = {-10,-7,-1,-1,-1};
    pStatusBar->SetStatusWidths(SFP_TOTOAL,w);
    m_pGauge = new wxGauge(pStatusBar, wxID_ANY, 100);
    m_pGauge->SetValue(50);
    updateProgressbar();
	SetStatusText(gmeWXT("就绪"), 0);
}


/** @brief 这里是真正的退出实现。所有清理工作在这里执行。
**/
void
MainFrame::onClose(wxCloseEvent& event)
{
    // deinitialize the frame manager
    m_mgr.UnInit();
	Destroy();
	event.Skip(false);
}

void
MainFrame::onSize(wxSizeEvent& event)
{
    //adjust status bar size.
    updateProgressbar();
}

void
MainFrame::onMenuFileImport(wxCommandEvent &event)
{
	wxFileDialog *OpenDialog= new wxFileDialog(this, _T("Choose a file"), _(""), _(""), _("*.*"), wxFD_OPEN);
	if ( OpenDialog->ShowModal() == wxID_OK )
	{
        gme::DocIO  dio;
		gme::DocObj	obj;
		std::string id;
		gme::ObjectNode *pParent = NULL;
		if(this->m_objectView->getSelection(id))
            pParent = obj.getRootObject().findObject(id);
		if(dio.importScene(boost::locale::conv::utf_to_utf<char>(OpenDialog->GetPath().ToStdWstring()),pParent))
        {
            if(id.length())
                this->m_objectView->refresh(id);
            else
                this->m_objectView->refreshAll();
		}
	}
	OpenDialog->Close(); // Or OpenDialog->Destroy() ?
}


void
MainFrame::onMenuFileOpen(wxCommandEvent &event)
{
	wxFileDialog *OpenDialog= new wxFileDialog(this, _T("Choose a file"), _(""), _(""), _("*.*"), wxFD_OPEN);
	if ( OpenDialog->ShowModal() == wxID_OK )
	{
        gme::DocIO  dio;
        dio.loadScene(boost::locale::conv::utf_to_utf<char>(OpenDialog->GetPath().ToStdWstring()));
        this->m_objectView->refreshAll();
	}
	OpenDialog->Close(); // Or OpenDialog->Destroy() ?
}

void
MainFrame::onMenuFileExport(wxCommandEvent &event)
{
    DECLARE_WXCONVERT;

	wxFileDialog *SaveDialog= new wxFileDialog(this, gmeWXT("选择导出文件"), _(""), _(""), _("*.*"), wxFD_SAVE);
	if ( SaveDialog->ShowModal() == wxID_OK )
	{
	    gme::DocIO  dio;
	    dio.exportScene(boost::locale::conv::utf_to_utf<char>(SaveDialog->GetPath().ToStdWstring()),true);
	}
	SaveDialog->Close();
}


void
MainFrame::onMenuFileSave(wxCommandEvent &event)
{
	wxFileDialog *SaveDialog= new wxFileDialog(this, _T("Choose a file"), _(""), _(""), _("*.*"), wxFD_SAVE);
	if ( SaveDialog->ShowModal() == wxID_OK )
	{
	    gme::DocIO  dio;
	    dio.exportScene(boost::locale::conv::utf_to_utf<char>(SaveDialog->GetPath().ToStdWstring()),false);
	}
	SaveDialog->Close();
}

void
MainFrame::onMenuFileQuit(wxCommandEvent &event)
{
	Close(false);
}

void
MainFrame::onMenuEditDelete(wxCommandEvent &event)
{
    DocIO   dio;
    std::string id;
    if(this->m_objectView->getSelection(id))
    {
        dio.deleteModel(id);
    }

	this->m_objectView->delSelection();
}


void
MainFrame::onMenuHelpAbout(wxCommandEvent &event)
{
	DECLARE_WXCONVERT;

	(void)wxMessageBox(gmeWXT("这是一个ABOUT消息框"),
                       gmeWXT("About us"),
                       wxOK | wxICON_INFORMATION);
}

void
MainFrame::onShowPropertyPane(wxCommandEvent &event)
{
	m_mgr.GetPane(m_propFrame).Show();

	m_mgr.Update();
}

} //namespace gme

