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
//#include "renderview.h"
#include "objectview.h"
#include "stringutil.h"
#include "cmdids.h"
#include "dm/docio.h"
#include "dm/docobj.h"
#include "dm/docctl.h"
#include "dm/docimg.h"
#include <boost/locale.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/bind.hpp>
#include "propgrid.h"
#include "data/xpmres.h"
#include "glrenderview.h"
#include "filedialog.h"

namespace gme{

BEGIN_EVENT_TABLE(MainFrame, inherited)
	EVT_MENU(wxID_OPEN, MainFrame::onMenuFileOpen)
	EVT_MENU(wxID_SAVE, MainFrame::onMenuFileSave)
	EVT_UPDATE_UI(wxID_SAVE,MainFrame::onUpdateSaveAndEdit)
	EVT_MENU(cmd::GID_EXPORT, MainFrame::onMenuFileExport)
	EVT_MENU(cmd::GID_IMPORT, MainFrame::onMenuFileImport)
	EVT_MENU(cmd::GID_SAVE_IMAGE, MainFrame::onMenuFileSaveImage)
	EVT_UPDATE_UI_RANGE(cmd::GID_EXPORT,cmd::GID_IMPORT,MainFrame::onUpdateSaveAndEdit)
	EVT_MENU(wxID_EXIT, MainFrame::onMenuFileQuit)
	EVT_MENU(wxID_DELETE, MainFrame::onMenuEditDelete)
	EVT_MENU(wxID_ABOUT, MainFrame::onMenuHelpAbout)
	EVT_MENU(cmd::GID_RENDER_START,MainFrame::onRenderStart)
	EVT_MENU(cmd::GID_RENDER_STOP,MainFrame::onRenderStop)
	EVT_MENU(cmd::GID_RENDER_PAUSE,MainFrame::onRenderPause)
    EVT_MENU_RANGE(cmd::GID_PANE_BEGIN, cmd::GID_PANE_END,MainFrame::onViewPane)
    EVT_UPDATE_UI_RANGE(cmd::GID_PANE_BEGIN,cmd::GID_PANE_END,MainFrame::onUpdateViewPane)
    EVT_MENU_RANGE(cmd::GID_VM_BEGIN,cmd::GID_VM_END,MainFrame::onViewmodeChanged)
    EVT_UPDATE_UI_RANGE(cmd::GID_VM_BEGIN,cmd::GID_VM_END,MainFrame::onUpdateViewmode)

	EVT_UPDATE_UI(wxID_DELETE,MainFrame::onUpdateMenuEditDelete)
	EVT_UPDATE_UI(cmd::GID_RENDER_START,MainFrame::onUpdateRenderStart)
	EVT_UPDATE_UI(cmd::GID_RENDER_STOP,MainFrame::onUpdateRenderStop)
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


    m_renderView = new GlRenderView(this);
    m_mgr.AddPane(m_renderView, wxCENTER);

	int propFrameStyle = wxNO_BORDER | \
						 wxCLIP_CHILDREN;
	// create and show propery pane
	m_propFrame = new PropFrame(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, propFrameStyle);
//    m_propFrame->establishConnect(m_objectView);
	m_mgr.AddPane(m_propFrame, wxRIGHT, gmeWXT("属性设置"));
	//m_mgr.GetPane(m_propFrame).Hide();
    // tell the manager to "commit" all the changes just made
    m_mgr.Update();
    sv_getImageFilepath = boost::bind(&MainFrame::getImageFilepath,this,_1);
}

MainFrame::~MainFrame()
{
    ///@todo 设置sv_getImageFilepath到一个恒定false的函数。
}

boost::function<bool (std::string &)>   MainFrame::sv_getImageFilepath;

void
MainFrame::createMenubar()
{
    DECLARE_WXCONVERT;

    wxMenuBar *pMenuBar = new wxMenuBar();

    {//File
        wxMenu *pFileMenu = new wxMenu();
        pFileMenu->Append(wxID_OPEN, gmeWXT("打开(&O)"), gmeWXT("打开已有场景"));
        pFileMenu->AppendSeparator();
        pFileMenu->Append(wxID_SAVE, gmeWXT("保存(&S)"), gmeWXT("保存现有场景"));
        pFileMenu->Append(cmd::GID_SAVE_IMAGE, gmeWXT("保存图片(&S)"), gmeWXT("保存当前渲染结果"));
        pFileMenu->Append(cmd::GID_EXPORT, gmeWXT("导出(&E)"), gmeWXT("导出现有场景"));
        pFileMenu->AppendSeparator();
        pFileMenu->Append(wxID_EXIT, gmeWXT("退出(&X)"), gmeWXT("退出gmeditor"));

        pMenuBar->Append(pFileMenu, gmeWXT("文件(&F)"));
    }

    {//Edit
        wxMenu *pEditMenu = new wxMenu();
		pEditMenu->Append(cmd::GID_IMPORT, gmeWXT("导入(&I)"), gmeWXT("从文件中导入模型到当前场景"));
        pEditMenu->Append(wxID_DELETE, gmeWXT("删除(&D)"), gmeWXT("删除选中模型"));
		pEditMenu->AppendSeparator();
		pEditMenu->Append(cmd::GID_RENDER_START,gmeWXT("开始渲染"),gmeWXT("开始渲染当前场景"));
		pEditMenu->Append(cmd::GID_RENDER_STOP,gmeWXT("结束渲染"),gmeWXT("结束当前场景的渲染"));
		pEditMenu->Append(cmd::GID_RENDER_PAUSE,gmeWXT("暂停渲染"),gmeWXT("暂停渲染当前场景"));

        pMenuBar->Append(pEditMenu, gmeWXT("编辑(&E)"));
    }

	{// view menu
		wxMenu *pViewMenu = new wxMenu();
		pViewMenu->AppendCheckItem(cmd::GID_PANE_OBJECTVIEW, gmeWXT("对象一览(&O)"), gmeWXT("显示/隐藏对象一览面板"));
		pViewMenu->AppendCheckItem(cmd::GID_PANE_PROPVIEW, gmeWXT("属性设置(&P)"), gmeWXT("显示/隐藏属性面板"));

		pViewMenu->AppendSeparator();
        wxMenu *pViewModeMenu = new wxMenu();
		pViewModeMenu->AppendRadioItem(cmd::GID_VM_ADJDOC, gmeWXT("校正文档(&A)"), gmeWXT("根据显示区域的大小自动设置渲染文档的尺寸。"));
		pViewModeMenu->AppendRadioItem(cmd::GID_VM_DOCSIZE, gmeWXT("实际尺寸(&P)"), gmeWXT("按照文档的实际尺寸显示。"));
		pViewModeMenu->AppendRadioItem(cmd::GID_VM_FULLWINDOW, gmeWXT("全屏缩放(&P)"), gmeWXT("自动缩放以充满全屏。"));
		pViewModeMenu->AppendRadioItem(cmd::GID_VM_SCALEWITHASPECT, gmeWXT("等比缩放(&P)"), gmeWXT("自动缩放到全屏并保持文档的横纵必不变。"));
        pViewMenu->AppendSubMenu(pViewModeMenu,gmeWXT("显示方式(&M)"),gmeWXT("控制主窗口如何匹配渲染图的尺寸。"));

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

	{//File Toolbar
		wxAuiToolBar *pFileTbr = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                                  wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW| wxAUI_TB_HORIZONTAL);

		wxBitmap bmpOpen(xpm::open);
		wxBitmap bmpSave(xpm::save);
		wxBitmap bmpExport(xpm::_export);

		pFileTbr->AddTool(wxID_OPEN,gmeWXT("File"),bmpOpen,gmeWXT("打开"));
		pFileTbr->AddSeparator();
		pFileTbr->AddTool(wxID_SAVE,gmeWXT("File"),bmpSave,gmeWXT("保存"));
		pFileTbr->AddTool(cmd::GID_EXPORT,gmeWXT("File"),bmpExport,gmeWXT("导出"));

		pFileTbr->Realize();

		m_mgr.AddPane(pFileTbr,wxAuiPaneInfo().Name(gmeWXT("filetoolbar")).Caption(gmeWXT("文件操作工具栏")).
                       ToolbarPane().Top());
	}

	{//Edit ToolBar
		wxAuiToolBar *pEditTbr = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
												  wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW| wxAUI_TB_HORIZONTAL);

		wxBitmap bmpImport(xpm::import);
		wxBitmap bmpDel(xpm::_delete);
		wxBitmap bmpStart(xpm::start);
		wxBitmap bmpStop(xpm::stop);
		wxBitmap bmpPause(xpm::pause);

		pEditTbr->AddTool(cmd::GID_IMPORT,gmeWXT("Edit"),bmpImport,gmeWXT("导入"));
		pEditTbr->AddTool(wxID_DELETE,gmeWXT("Edit"),bmpDel,gmeWXT("删除"));
		pEditTbr->AddSeparator();
		pEditTbr->AddTool(cmd::GID_RENDER_START,gmeWXT("RenderCtrl"),bmpStart,gmeWXT("开始渲染"));
		pEditTbr->AddTool(cmd::GID_RENDER_STOP,gmeWXT("RenderCtrl"),bmpStop,gmeWXT("结束渲染"));
		pEditTbr->AddTool(cmd::GID_RENDER_PAUSE,gmeWXT("RenderCtrl"),bmpPause,gmeWXT("暂停渲染"));
		pEditTbr->Realize();

		m_mgr.AddPane(pEditTbr,wxAuiPaneInfo().Name(gmeWXT("edittoolbar")).Caption(gmeWXT("编辑场景工具栏")).
			ToolbarPane().Top().Position(1));
	}

	{//Help Toolbar
		wxAuiToolBar *pHelpTbr = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                                  wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW| wxAUI_TB_HORIZONTAL);

		wxBitmap bmpHelp(xpm::help);

		pHelpTbr->AddTool(wxID_ABOUT,gmeWXT("Help"),bmpHelp,gmeWXT("关于(&A)"));

		pHelpTbr->Realize();

		m_mgr.AddPane(pHelpTbr,wxAuiPaneInfo().Name(gmeWXT("helptoolbar")).Caption(gmeWXT("帮助工具栏")).
			ToolbarPane().Top().Right());
	}

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
MainFrame::onViewmodeChanged(wxCommandEvent &event)
{
    this->m_renderView->setViewmodeFromCmd(event.GetId());
}


void
MainFrame::onUpdateViewmode(wxUpdateUIEvent& event)
{
    event.Check(this->m_renderView->isCurrentViewmodeFromCmd(event.GetId()));
}

wxAuiPaneInfo&
MainFrame::getPaneFromCmdID(int cmdid)
{
    wxWindow    *pWindow = NULL;
    switch(cmdid)
    {
    case cmd::GID_PANE_OBJECTVIEW:
        pWindow = m_objectView;
        break;
    case cmd::GID_PANE_PROPVIEW:
        pWindow = m_propFrame;
        break;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code");
    }
    return m_mgr.GetPane(pWindow);
}

void
MainFrame::onMenuFileSaveImage(wxCommandEvent &event)
{
    SaveImageDialog dialog(this);
    if(dialog.ShowModal() == wxID_OK)
	{
		gme::DocImg img;
        img.saveImage(dialog.GetPath());
	}
}

void
MainFrame::onMenuFileImport(wxCommandEvent &event)
{
    ImportDialog    dialog(this);
    if(dialog.ShowModal() == wxID_OK)
	{
		gme::DocObj	obj;
		gme::ObjectNode *pParent = NULL;
		if(obj.getSelection().size())
		{
            pParent = obj.getRootObject().findObject(obj.getSelection().back(),NULL);
		}
        obj.importObject(dialog.GetPath(),pParent);
	}
}

bool
MainFrame::getImageFilepath(std::string &result)
{
    bool    bHasResult = false;
    OpenImageDialog dialog(this);
	if ( dialog.ShowModal() == wxID_OK )
	{
        result = dialog.GetPath();
        bHasResult = true;
	}
    return bHasResult;
}


void
MainFrame::onMenuFileOpen(wxCommandEvent &event)
{
	OpenSceneDialog dialog(this);
	if ( dialog.ShowModal() == wxID_OK )
	{
        gme::DocIO  dio;
        m_filepath = dialog.GetPath();
		dio.loadScene(m_filepath);
	}
}

void
MainFrame::onMenuFileExport(wxCommandEvent &event)
{
    SaveSceneDialog dialog(this);
	if ( dialog.ShowModal() == wxID_OK )
	{
	    gme::DocIO  dio;
	    dio.exportScene(dialog.GetPath(),true);
	}
}


void
MainFrame::onMenuFileSave(wxCommandEvent &event)
{
	gme::DocIO  dio;
	if(m_filepath.empty() || !boost::iends_with(m_filepath,".sps"))
	{
        SaveSceneDialog dialog(this);
        m_filepath.clear();
        if ( dialog.ShowModal() == wxID_OK )
        {
            m_filepath = dialog.GetPath();
        }

	}
	if(!m_filepath.empty())
	{
	   dio.exportScene(m_filepath,false);
	}
}

void
MainFrame::onMenuFileQuit(wxCommandEvent &event)
{
	Close(false);
}

void
MainFrame::onMenuEditDelete(wxCommandEvent &event)
{
    DocObj   dobj;
    dobj.deleteAllSelection();
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
MainFrame::onViewPane(wxCommandEvent &event)
{
    wxAuiPaneInfo&  pi = this->getPaneFromCmdID(event.GetId());
    if(pi.IsShown())
    {
        pi.Hide();
    }else{
        pi.Show();
    }
	m_mgr.Update();
}

void
MainFrame::onUpdateViewPane(wxUpdateUIEvent &event)
{
    event.Check(this->getPaneFromCmdID(event.GetId()).IsShown());
}


void
MainFrame::onRenderStart(wxCommandEvent &event)
{
	DocCtl dctl;
	if(!dctl.isRuning())
	{
		dctl.start();
	}
}

void
MainFrame::onRenderStop(wxCommandEvent &event)
{
	DocCtl dctl;
	if(dctl.isRuning())
	{
		dctl.stop();
	}
}

void
MainFrame::onRenderPause(wxCommandEvent &event)
{


}

void
MainFrame::onUpdateRenderStart(wxUpdateUIEvent& event)
{
	DocCtl dctl;
	event.Enable(!dctl.isRuning()&&!m_filepath.empty());
}

void
MainFrame::onUpdateRenderStop(wxUpdateUIEvent& event)
{
	DocCtl dctl;
	event.Enable(dctl.isRuning());
}

void
MainFrame::onUpdateMenuEditDelete(wxUpdateUIEvent& event)
{
    DocObj  obj;
    //@FIXME: need to implement obj.canDeleteItem. to remove last object cause slg crash.
    event.Enable(obj.getSelection().size() > 0);
}

void
MainFrame::onUpdateSaveAndEdit(wxUpdateUIEvent &event)
{
//	event.Enable(!m_filepath.empty());
}


} //namespace gme

