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

#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif 

#include "config.h"
#include "mainframe.h"
//#include "renderview.h"
#include "objectview.h"
#include "utils/i18n.h"
#include "stringutil.h"
#include "cmdids.h"
#include "dm/docio.h"
#include "dm/docobj.h"
#include "dm/docctl.h"
#include "dm/docimg.h"
#include "dm/docsetting.h"
#include "dm/doccamera.h"
#include <boost/locale.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include "propgrid.h"
#include "data/xpmres.h"
#include "glrenderview.h"
#include "filedialog.h"
#include "gmestatus.h"
#include "dialog/preferencesdialog.h"
#include "cameraview.h"

#include "buildinfo.h"

namespace gme{


///@todo move the follow function to customize log window when it implement.
static void Log_Adapter(int level,const char* msgstr,const char* mask)
{
    wxMBConvUTF8	gme_wx_utf8_conv;
    wxString  msg(msgstr,gme_wx_utf8_conv);
    if(msg.empty() && *msgstr)
    {
        msg = wxString(__("报告的日志中包含了非UTF-8编码的字符，这通常是由于文件采用了非UTF8编码引发的。请使用编码转化工具将其转为UTF8编码文件。其原始内容为:\n\t\t"),gme_wx_utf8_conv);
        msg.append(msgstr);
    }
    if(level <= Doc::LOG_DEBUG)
    {
        if(boost::ifind_first(msgstr,"ERROR"))
        {
            level = Doc::LOG_WARNING;
        }else if(mask)
        {
            ///example GME_SHOW_MASK: LuxRays;SDL;SLG
            const char* show_mask = std::getenv("GME_SHOW_MASK");
            if(boost::ifind_first(show_mask,mask))
            {
                level = Doc::LOG_STATUS;
            }
        }
    }
    switch(level)
    {
    case Doc::LOG_TRACE:
        wxLogStatus(msg);
        if(mask)
        {
            wxLogTrace(mask,msg);
        }else
        {
            wxLogTrace("default",msg);
        }
        break;
    case Doc::LOG_DEBUG:
        wxLogDebug(msg);
        break;
    case Doc::LOG_VERBOSE:
        wxLogVerbose(msg);
        break;
    case Doc::LOG_MESSAGE:
        wxLogMessage(msg);
        break;
    case Doc::LOG_STATUS:
        wxLogStatus(msg);
        break;
    case Doc::LOG_WARNING:
        wxLogWarning(msg);
        break;
    case Doc::LOG_ERROR:
        wxLogError(msg);
        break;
    case Doc::LOG_SYSERROR:
        wxLogSysError(msg);
        break;
    case Doc::LOG_FATALERROR:
        wxLogFatalError(msg);
        break;
    }
}


BEGIN_EVENT_TABLE(MainFrame, inherited)
	EVT_MENU(wxID_OPEN, MainFrame::onMenuFileOpen)
	EVT_MENU(wxID_SAVE, MainFrame::onMenuFileSave)
	EVT_UPDATE_UI(wxID_SAVE,MainFrame::onUpdateMenuFileSave)
	EVT_MENU(cmd::GID_EXPORT, MainFrame::onMenuFileExport)
	EVT_UPDATE_UI(cmd::GID_EXPORT,MainFrame::onUpdateonMenuFileExport)
	EVT_MENU(cmd::GID_IMPORT, MainFrame::onMenuFileImport)
	EVT_MENU(cmd::GID_PREFERENCES, MainFrame::onMenuPreferences)
	EVT_UPDATE_UI(cmd::GID_IMPORT,MainFrame::onUpdateMenuFileImport)
    EVT_MENU(cmd::GID_SAVE_IMAGE, MainFrame::onMenuFileSaveImage)
	EVT_UPDATE_UI(cmd::GID_SAVE_IMAGE,MainFrame::onUpdateMenuFileSaveImage)

	EVT_MENU(wxID_EXIT, MainFrame::onMenuFileQuit)
	EVT_MENU(wxID_DELETE, MainFrame::onMenuEditDelete)
	EVT_UPDATE_UI(wxID_DELETE,MainFrame::onUpdateMenuEditDelete)
	EVT_MENU(wxID_ABOUT, MainFrame::onMenuHelpAbout)
	EVT_MENU(cmd::GID_RENDER_START,MainFrame::onRenderStart)
	EVT_UPDATE_UI(cmd::GID_RENDER_START,MainFrame::onUpdateRenderStart)
	EVT_MENU(cmd::GID_RENDER_STOP,MainFrame::onRenderStop)
	EVT_UPDATE_UI(cmd::GID_RENDER_STOP,MainFrame::onUpdateRenderStop)
	EVT_MENU(cmd::GID_RENDER_PAUSE,MainFrame::onRenderPause)
	EVT_UPDATE_UI(cmd::GID_RENDER_PAUSE,MainFrame::onUpdateRenderPause)

	EVT_MENU(cmd::GID_AUTO_TARGET,MainFrame::onAutoTarget)
	EVT_UPDATE_UI(cmd::GID_AUTO_TARGET,MainFrame::onUpdateAutoTarget)

    EVT_MENU_RANGE(cmd::GID_PANE_BEGIN, cmd::GID_PANE_END,MainFrame::onViewPane)
    EVT_UPDATE_UI_RANGE(cmd::GID_PANE_BEGIN,cmd::GID_PANE_END,MainFrame::onUpdateViewPane)
    EVT_MENU_RANGE(cmd::GID_VM_BEGIN,cmd::GID_VM_END,MainFrame::onViewmodeChanged)
    EVT_UPDATE_UI_RANGE(cmd::GID_VM_BEGIN,cmd::GID_VM_END,MainFrame::onUpdateViewmode)
    EVT_MENU_RANGE(cmd::GID_MD_START,cmd::GID_MD_END,MainFrame::onEditmodeChanged)
    EVT_UPDATE_UI_RANGE(cmd::GID_MD_START,cmd::GID_MD_END,MainFrame::onUpdateEditmode)
    EVT_MENU_RANGE(cmd::GID_LOG_BEGIN,cmd::GID_LOG_END,MainFrame::onLogLevelChanged)
    EVT_UPDATE_UI_RANGE(cmd::GID_LOG_BEGIN,cmd::GID_LOG_END,MainFrame::onUpdateLogLevel)
    EVT_MENU_RANGE(cmd::GID_SET_BEGIN,cmd::GID_SET_END,MainFrame::onSetting)
    EVT_UPDATE_UI_RANGE(cmd::GID_SET_BEGIN,cmd::GID_SET_END,MainFrame::onUpdateSetting)


	EVT_MENU(cmd::GID_VIEWSELECTION,MainFrame::onViewSelection)
	EVT_UPDATE_UI(cmd::GID_VIEWSELECTION,MainFrame::onUpdateViewSelection)
	EVT_MENU(cmd::GID_VIEWSKYLIGHTDIR,MainFrame::onViewSkylightDir)
	EVT_UPDATE_UI(cmd::GID_VIEWSKYLIGHTDIR,MainFrame::onUpdateViewSkylightDir)
	EVT_MENU(cmd::GID_CLEAR_LOG,MainFrame::onClearLog)
	EVT_UPDATE_UI(cmd::GID_CLEAR_LOG,MainFrame::onUpdateClearLog)
	EVT_MENU(cmd::GID_CAM_NEWFROMCURRENT,MainFrame::onNewCamFromCurrent)
	EVT_UPDATE_UI(cmd::GID_CAM_NEWFROMCURRENT,MainFrame::onUpdateNewCamFromCurrent)
	EVT_MENU(cmd::GID_IMM_REFRESH,MainFrame::onImmRefresh)
	EVT_UPDATE_UI(cmd::GID_IMM_REFRESH,MainFrame::onUpdateImmRefresh)

	EVT_CLOSE(MainFrame::onClose)
END_EVENT_TABLE()

MainFrame::MainFrame(wxWindow* parent) : wxFrame(parent, -1, _("GMEditor"),
                  wxDefaultPosition, wxSize(800,600),
                  wxDEFAULT_FRAME_STYLE)
{
    createMenubar();
	createToolbar();
    createStatusbar();

    DECLARE_WXCONVERT;

    // notify wxAUI which frame to use
    m_mgr.SetManagedWindow(this);

    // create several text controls
    //wxTextCtrl *text1 = new wxTextCtrl(this, -1, _("Pane 1 - sample text"),wxDefaultPosition, wxSize(200,150),wxNO_BORDER | wxTE_MULTILINE);

    m_logWindow = new wxTextCtrl(this, -1, "",
                  wxDefaultPosition, wxSize(800,200),
                  wxNO_BORDER | wxTE_MULTILINE | wxTE_READONLY);
	wxLog::SetActiveTarget(new wxLogTextCtrl(m_logWindow));
    m_objectView = new ObjectView(this,wxID_ANY,wxDefaultPosition,wxSize(200,450));
    m_camView = new CameraView(this,wxID_ANY,wxDefaultPosition,wxSize(200,450));
    // add the panes to the manager
	wxMBConvUTF8	conv;
    m_mgr.AddPane(m_camView, wxLEFT, gmeWXT("摄像机列表"));
    m_mgr.AddPane(m_objectView, wxLEFT, gmeWXT("模型一览"));
    m_mgr.AddPane(m_logWindow, wxBOTTOM, gmeWXT("系统日志"));


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

    Doc::SetSysLog(Log_Adapter);
}

MainFrame::~MainFrame()
{
	m_mgr.UnInit();
    ///@todo 设置sv_getImageFilepath到一个恒定false的函数。
}

boost::function<bool (std::string &)>   MainFrame::sv_getImageFilepath;


wxString&
MainFrame::appendShortCutString(int cmdid,wxString &shortCut)
{
    switch(cmdid)
    {
    case cmd::GID_MD_LOCK:
        shortCut.append("\tCtrl+L");
        break;
    case wxID_SAVE:
        shortCut.append("\tCtrl+S");
        break;
    case cmd::GID_MD_PANE:
        shortCut.append("\tCtrl+P");
        break;
    case cmd::GID_MD_ROTATE:
        shortCut.append("\tCtrl+R");
        break;
    case cmd::GID_MD_ROTATE_AROUND_FOCUS:
        shortCut.append("\tCtrl+F");
        break;
    case cmd::GID_MD_ZOOM:
        shortCut.append("\tCtrl+Z");
        break;
    case cmd::GID_MD_SELECT:
        shortCut.append("\tCtrl+X");
        break;
    case cmd::GID_AUTO_TARGET:
        shortCut.append("\tCtrl+A");
        break;
    case cmd::GID_VIEWSELECTION:
        shortCut.append("\tCtrl+V");
        break;
    case cmd::GID_VIEWSKYLIGHTDIR:
        shortCut.append("\tCtrl+I");
        break;
    case cmd::GID_MD_SETSUNLIGHT:
        shortCut.append("\tCtrl+W");
        break;
    case cmd::GID_IMM_REFRESH:
        shortCut.append("\tF5");
        break;
    default:
        break;
    }
    return shortCut;
}

void
MainFrame::createMenubar()
{
    DECLARE_WXCONVERT;
    wxString    name;
    wxMenuBar *pMenuBar = new wxMenuBar();

    {//File
        wxMenu *pFileMenu = new wxMenu();
        pFileMenu->Append(wxID_OPEN, gmeWXT("Open"), gmeWXT("打开已有场景"));
        pFileMenu->AppendSeparator();

        name = gmeWXT("保存(&S)");
        pFileMenu->Append(wxID_SAVE, appendShortCutString(wxID_SAVE,name), gmeWXT("保存现有场景"));
        pFileMenu->Append(cmd::GID_SAVE_IMAGE, gmeWXT("保存图片(&S)"), gmeWXT("保存当前渲染结果"));
        pFileMenu->Append(cmd::GID_EXPORT, gmeWXT("导出(&E)"), gmeWXT("导出现有场景"));
		pFileMenu->Append(cmd::GID_PREFERENCES, gmeWXT("参数设置(&P)"), gmeWXT("参数设置"));
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

		pEditMenu->AppendSeparator();
        wxMenu *pEditmodeMenu = new wxMenu();

        name = gmeWXT("锁定(&L)");
        pEditmodeMenu->AppendRadioItem(cmd::GID_MD_LOCK,appendShortCutString(cmd::GID_MD_LOCK,name), gmeWXT("锁定窗口。"));
        name = gmeWXT("平移(&P)");
		pEditmodeMenu->AppendRadioItem(cmd::GID_MD_PANE, appendShortCutString(cmd::GID_MD_PANE,name), gmeWXT("平移控制。"));
		name = gmeWXT("旋转(&R)");
		pEditmodeMenu->AppendRadioItem(cmd::GID_MD_ROTATE, appendShortCutString(cmd::GID_MD_ROTATE,name), gmeWXT("自身旋转。"));
		name = gmeWXT("焦点旋转(&C)");
		pEditmodeMenu->AppendRadioItem(cmd::GID_MD_ROTATE_AROUND_FOCUS, appendShortCutString(cmd::GID_MD_ROTATE_AROUND_FOCUS,name), gmeWXT("绕摄像机焦点旋转。"));
		name = gmeWXT("缩放(&Z)");
		pEditmodeMenu->AppendRadioItem(cmd::GID_MD_ZOOM, appendShortCutString(cmd::GID_MD_ZOOM,name), gmeWXT("缩放控制。"));
		name = gmeWXT("选择(&S)");
		pEditmodeMenu->AppendRadioItem(cmd::GID_MD_SELECT, appendShortCutString(cmd::GID_MD_SELECT,name), gmeWXT("点击选择模式。"));
		name = gmeWXT("阳光方向(&S)");
		pEditmodeMenu->AppendRadioItem(cmd::GID_MD_SETSUNLIGHT, appendShortCutString(cmd::GID_MD_SETSUNLIGHT,name), gmeWXT("进入阳光方向编辑模式。"));

        pEditMenu->AppendSubMenu(pEditmodeMenu,gmeWXT("编辑模式(&M)"),gmeWXT("控制主窗口的编辑模式。"));
		name = gmeWXT("自动对正(&A)");
		pEditMenu->AppendCheckItem(cmd::GID_AUTO_TARGET,appendShortCutString(cmd::GID_AUTO_TARGET,name),gmeWXT("选择物体时自动校正摄像机中心点."));

		pEditMenu->AppendSeparator();
        pEditMenu->Append(cmd::GID_CAM_NEWFROMCURRENT,gmeWXT("保存当前视角(&M)"),gmeWXT("为当前视角新建一个摄像机位。"));

        pMenuBar->Append(pEditMenu, gmeWXT("编辑(&E)"));
    }

	{// view menu
		wxMenu *pViewMenu = new wxMenu();
		pViewMenu->AppendCheckItem(cmd::GID_PANE_OBJECTVIEW, gmeWXT("对象一览(&O)"), gmeWXT("显示/隐藏对象一览面板"));
		pViewMenu->AppendCheckItem(cmd::GID_PANE_PROPVIEW, gmeWXT("属性设置(&P)"), gmeWXT("显示/隐藏属性面板"));
        pViewMenu->AppendCheckItem(cmd::GID_PANE_CAMVIEW, gmeWXT("摄像机列表(&C)"), gmeWXT("显示/隐藏摄像机列表面板"));
		pViewMenu->AppendCheckItem(cmd::GID_PANE_SYSLOG, gmeWXT("系统日志(&L)"), gmeWXT("显示/隐藏系统日志面板"));

		pViewMenu->AppendSeparator();
        wxMenu *pViewModeMenu = new wxMenu();
		pViewModeMenu->AppendRadioItem(cmd::GID_VM_ADJDOC, gmeWXT("校正文档(&A)"), gmeWXT("根据显示区域的大小自动设置渲染文档的尺寸。"));
		pViewModeMenu->AppendRadioItem(cmd::GID_VM_DOCSIZE, gmeWXT("实际尺寸(&P)"), gmeWXT("按照文档的实际尺寸显示。"));
		pViewModeMenu->AppendRadioItem(cmd::GID_VM_FULLWINDOW, gmeWXT("全屏缩放(&P)"), gmeWXT("自动缩放以充满全屏。"));
		pViewModeMenu->AppendRadioItem(cmd::GID_VM_SCALEWITHASPECT, gmeWXT("等比缩放(&P)"), gmeWXT("自动缩放到全屏并保持文档的横纵必不变。"));
        pViewMenu->AppendSubMenu(pViewModeMenu,gmeWXT("显示方式(&M)"),gmeWXT("控制主窗口如何匹配渲染图的尺寸。"));
        name = gmeWXT("标识选中对象(&Z)");
		pViewMenu->AppendCheckItem(cmd::GID_VIEWSELECTION, appendShortCutString(cmd::GID_VIEWSELECTION,name), gmeWXT("在编辑视图中标识选中对象。"));
        name = gmeWXT("显示阳光方向(&L)");
		pViewMenu->AppendCheckItem(cmd::GID_VIEWSKYLIGHTDIR, appendShortCutString(cmd::GID_VIEWSKYLIGHTDIR,name), gmeWXT("在编辑视图中居中显示阳光方向。"));

		pViewMenu->AppendSeparator();
        wxMenu *pLogLevel = new wxMenu();
		pLogLevel->AppendRadioItem(cmd::GID_LOG_TRACE, gmeWXT("Trace(&A)"), gmeWXT("设置日志级别到Trace"));
		pLogLevel->AppendRadioItem(cmd::GID_LOG_DEBUG, gmeWXT("Debug(&D)"), gmeWXT("设置日志级别到Debug"));
		pLogLevel->AppendRadioItem(cmd::GID_LOG_VERBOSE, gmeWXT("Verbose(&V)"), gmeWXT("设置日志级别到Verbose"));
		pLogLevel->AppendRadioItem(cmd::GID_LOG_MESSAGE, gmeWXT("Message(&M)"), gmeWXT("设置日志级别到Message"));
		pLogLevel->AppendRadioItem(cmd::GID_LOG_STATUS, gmeWXT("Status(&S)"), gmeWXT("设置日志级别到Status"));
		pLogLevel->AppendRadioItem(cmd::GID_LOG_WARNING, gmeWXT("Warning(&W)"), gmeWXT("设置日志级别到Warning"));
		pLogLevel->AppendRadioItem(cmd::GID_LOG_ERROR, gmeWXT("Error(&E)"), gmeWXT("设置日志级别到Error"));
		pLogLevel->AppendRadioItem(cmd::GID_LOG_SYSERROR, gmeWXT("SysError(&S)"), gmeWXT("设置日志级别到SysError"));
		pLogLevel->AppendRadioItem(cmd::GID_LOG_FATALERROR, gmeWXT("FAtalError(&F)"), gmeWXT("设置日志级别到FAtalError"));
		pViewMenu->AppendSubMenu(pLogLevel,gmeWXT("日志级别(&L)"),gmeWXT("设置系统显示的日志级别。"));
        pViewMenu->Append(cmd::GID_CLEAR_LOG,gmeWXT("清空日志(&C)"),gmeWXT("清空当前日志的内容。"));

		pMenuBar->Append(pViewMenu, gmeWXT("视图(&V)"));
	}

    {// setting
		wxMenu *pSettingMenu = new wxMenu();
		pSettingMenu->AppendCheckItem(cmd::GID_SET_FORCEREFRESH, gmeWXT("强制刷新"), gmeWXT("每次编辑时强制刷新GPU缓冲。"));
        name = gmeWXT("立即刷新(&R)");
		pSettingMenu->Append(cmd::GID_IMM_REFRESH, appendShortCutString(cmd::GID_IMM_REFRESH,name), gmeWXT("立即刷新GPU缓冲。"));
		pSettingMenu->AppendSeparator();

        wxMenu *pLoadingMenu = new wxMenu();
		pLoadingMenu->AppendCheckItem(cmd::GID_SET_ValidateDataStructure, gmeWXT("数据有效性校正(&A)"), gmeWXT("检查导入模型的索引、动画以及骨骼的关联有效性，如果无效，自动校正之。"));
		pLoadingMenu->AppendCheckItem(cmd::GID_SET_JoinIdenticalVertices, gmeWXT("合并相同顶点(&J)"), gmeWXT("自动合并具有相同坐标的顶点。"));
		pLoadingMenu->AppendCheckItem(cmd::GID_SET_RemoveRedundantMaterials, gmeWXT("移除冗余材质(&R)"), gmeWXT("将冗余材质移除。"));
		pLoadingMenu->AppendCheckItem(cmd::GID_SET_ImproveCacheLocality, gmeWXT("提升缓冲效率(&I)"), gmeWXT("校正模型以提升ACMR(average post-transform vertex cache miss ratio)。"));
		pLoadingMenu->AppendCheckItem(cmd::GID_SET_FixInfacingNormals, gmeWXT("校正反转法线(&F)"), gmeWXT("根据包围盒方向尝试校正错误方向的法线。"));
		pLoadingMenu->AppendCheckItem(cmd::GID_SET_FindDegenerates, gmeWXT("退化校正(&D)"), gmeWXT("由于点的移除，有些面会退化为点或者线，退化校正会把这些点和线移除。"));
		pLoadingMenu->AppendCheckItem(cmd::GID_SET_FindInvalidData, gmeWXT("无效数据校正(&V)"), gmeWXT("检查是否有原点法线以及溢出数据，并自动校正这些数据。"));
		pLoadingMenu->AppendCheckItem(cmd::GID_SET_FlipUVs, gmeWXT("UV反转(&U)"), gmeWXT("反转UV以适应导出器数据。"));
		pLoadingMenu->AppendCheckItem(cmd::GID_SET_OptimizeMeshes, gmeWXT("模型合并(&O)"), gmeWXT("尝试合并模型并保持绘制结果不变。"));
		pLoadingMenu->AppendCheckItem(cmd::GID_SET_Debone, gmeWXT("骨骼解绑(&B)"), gmeWXT("尝试按照初始位置解绑骨骼，这允许我们渲染封套后的模型。"));
		pLoadingMenu->AppendSeparator();
        pLoadingMenu->AppendRadioItem(cmd::GID_SET_GenSmoothNormals, gmeWXT("产生顶点法线(&G)"), gmeWXT("如果模型中未包含法线，自动计算基于顶点的法线，这将产生光滑感。"));
		pLoadingMenu->AppendRadioItem(cmd::GID_SET_IgnoreNormals, gmeWXT("忽略法线(&I)"), gmeWXT("忽略模型中包含的法线信息，在某些场合下，这将减少黑面出现的几率。"));
		pLoadingMenu->AppendRadioItem(cmd::GID_SET_CommonNormals, gmeWXT("模型法线(&M)"), gmeWXT("只有模型中包含了法线信息，才读取法线，这将是否使用法线的控制权交给导出器决定。"));


        pSettingMenu->AppendSubMenu(pLoadingMenu,gmeWXT("加载设定"),gmeWXT("设定导入外部模型时对模型的自动处理。"));

		pMenuBar->Append(pSettingMenu, gmeWXT("设置(&S)"));
	}

	{//Help
		wxMenu *pHelpMenu = new wxMenu();
		pHelpMenu->Append(wxID_ABOUT,gmeWXT("关于(&A)"),gmeWXT("关于飞鹿材质编辑器"));


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

	{//View Toolbar
		wxAuiToolBar *pViewTbr = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                                  wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW| wxAUI_TB_HORIZONTAL);

		pViewTbr->AddTool(cmd::GID_MD_LOCK,gmeWXT("锁定"),wxBitmap(xpm::help),gmeWXT("锁定说明"),wxITEM_RADIO);
		pViewTbr->AddTool(cmd::GID_MD_PANE,gmeWXT("平移"),wxBitmap(xpm::help),gmeWXT("平移说明"),wxITEM_RADIO);
		pViewTbr->AddTool(cmd::GID_MD_ROTATE,gmeWXT("旋转"),wxBitmap(xpm::help),gmeWXT("旋转说明"),wxITEM_RADIO);
		pViewTbr->AddTool(cmd::GID_MD_ROTATE_AROUND_FOCUS,gmeWXT("焦点旋转"),wxBitmap(xpm::translateCam),gmeWXT("焦点旋转说明"),wxITEM_RADIO);
		pViewTbr->AddTool(cmd::GID_MD_ZOOM,gmeWXT("缩放"),wxBitmap(xpm::help),gmeWXT("缩放说明"),wxITEM_RADIO);
		pViewTbr->AddTool(cmd::GID_MD_SELECT,gmeWXT("选择"),wxBitmap(xpm::help),gmeWXT("选择说明"),wxITEM_RADIO);
		pViewTbr->AddTool(cmd::GID_MD_SETSUNLIGHT,gmeWXT("阳光方向"),wxBitmap(xpm::help),gmeWXT("阳光方向说明"),wxITEM_RADIO);

		pViewTbr->Realize();

		m_mgr.AddPane(pViewTbr,wxAuiPaneInfo().Name(gmeWXT("编辑工具栏")).Caption(gmeWXT("编辑")).
			ToolbarPane().Top().Position(2));
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
    wxStatusBar *pStatusBar = new GMEStatusBar(this,wxSTB_DEFAULT_STYLE);
    this->SetStatusBar(pStatusBar);
    this->PositionStatusBar();
	SetStatusText(gmeWXT("就绪"), 0);
}


/** @brief 这里是真正的退出实现。所有清理工作在这里执行。
**/
void
MainFrame::onClose(wxCloseEvent& event)
{
    // deinitialize the frame manager
	/*
    m_mgr.UnInit();
	Destroy();
    wxTheApp->ExitMainLoop();
    exit(0);
	event.Skip(false);
	*/
	DocCtl dctl;
	dctl.stop();
	Destroy();
}

void
MainFrame::onEditmodeChanged(wxCommandEvent &event)
{
    this->m_renderView->setEditmodeFromCmd(event.GetId());
}

void
MainFrame::onUpdateEditmode(wxUpdateUIEvent &event)
{
    event.Check(this->m_renderView->getEditmodeCmd() == event.GetId());
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

int
MainFrame::getLoadingFlagFromCmd(int cmdid)
{
    switch(cmdid)
    {
    case cmd::GID_SET_ValidateDataStructure:
        return DocSetting::ValidateDataStructure;
        break;
    case cmd::GID_SET_GenSmoothNormals:
        return DocSetting::GenSmoothNormals;
        break;
    case cmd::GID_SET_JoinIdenticalVertices:
        return DocSetting::JoinIdenticalVertices;
        break;
    case cmd::GID_SET_RemoveRedundantMaterials:
        return DocSetting::RemoveRedundantMaterials;
        break;
    case cmd::GID_SET_ImproveCacheLocality:
        return DocSetting::ImproveCacheLocality;
        break;
    case cmd::GID_SET_FixInfacingNormals:
        return DocSetting::FixInfacingNormals;
        break;
    case cmd::GID_SET_FindDegenerates:
        return DocSetting::FindDegenerates;
        break;
    case cmd::GID_SET_FindInvalidData:
        return DocSetting::FindInvalidData;
        break;
    case cmd::GID_SET_FlipUVs:
        return DocSetting::FlipUVs;
        break;
    case cmd::GID_SET_OptimizeMeshes:
        return DocSetting::OptimizeMeshes;
        break;
    case cmd::GID_SET_Debone:
        return DocSetting::Debone;
        break;
    }
    BOOST_ASSERT_MSG(false,"invalid loading flag.");
    throw std::runtime_error("invalid loading flag");
}

void
MainFrame::onSetting(wxCommandEvent &event)
{
    switch(event.GetId())
    {
    case cmd::GID_SET_FORCEREFRESH:
        DocSetting::forceRefresh(!DocSetting::forceRefresh());
        break;
    case cmd::GID_SET_IgnoreNormals:
        DocSetting::ignoreNormals(true);
        DocSetting::clearLoadingFlag(DocSetting::GenSmoothNormals);
        break;
    case cmd::GID_SET_CommonNormals:
        DocSetting::ignoreNormals(false);
        DocSetting::clearLoadingFlag(DocSetting::GenSmoothNormals);
        break;
    default:
        {
            int loadingFlag = getLoadingFlagFromCmd(event.GetId());
            if(DocSetting::hasLoadingFlag(loadingFlag))
            {
                DocSetting::clearLoadingFlag(loadingFlag);
            }else
            {
                DocSetting::setLoadingFlag(loadingFlag);
            }
            ///如果设置了gen sommth normal,需要清理掉ignoreNormal属性。
            if(DocSetting::hasLoadingFlag(DocSetting::GenSmoothNormals))
                DocSetting::ignoreNormals(false);
        }
    }
}

void
MainFrame::onUpdateSetting(wxUpdateUIEvent &event)
{
    switch(event.GetId())
    {
    case cmd::GID_SET_FORCEREFRESH:
        event.Check(DocSetting::forceRefresh());
        break;
    case cmd::GID_SET_IgnoreNormals:
        if(DocSetting::hasLoadingFlag(DocSetting::GenSmoothNormals) || !DocSetting::ignoreNormals() )
        {
            event.Check(false);
        }else{
            event.Check(true);
        }
        break;
    case cmd::GID_SET_CommonNormals:
        if(DocSetting::hasLoadingFlag(DocSetting::GenSmoothNormals) || DocSetting::ignoreNormals() )
        {
            event.Check(false);
        }else{
            event.Check(true);
        }
        break;
    default:
        event.Check(DocSetting::hasLoadingFlag(getLoadingFlagFromCmd(event.GetId())));
    }
}


void
MainFrame::onLogLevelChanged(wxCommandEvent &event)
{
    Doc::SyslogLevel(event.GetId() - cmd::GID_LOG_TRACE);
}

void
MainFrame::onUpdateLogLevel(wxUpdateUIEvent &event)
{
    int level = event.GetId() - cmd::GID_LOG_TRACE;
    event.Check(level == Doc::SyslogLevel());
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
    case cmd::GID_PANE_SYSLOG:
        pWindow = m_logWindow;
        break;
    case cmd::GID_PANE_CAMVIEW:
        pWindow = m_camView;
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
MainFrame::onUpdateMenuFileSaveImage(wxUpdateUIEvent& event)
{
    DocCtl dc;
    event.Enable(dc.isRuning() || dc.isPause());
}

void
MainFrame::onMenuFileImport(wxCommandEvent &event)
{
    ImportDialog    dialog(this);
    if(dialog.ShowModal() == wxID_OK)
	{
        {
            wxBusyCursor wait;
		    gme::DocObj	obj;
		    gme::ObjectNode *pParent = NULL;
		    if(obj.getSelection().size())
		    {
                pParent = obj.getRootObject().findObject(obj.getSelection().back(),NULL);
		    }
            obj.importObject(dialog.GetPath(),pParent);
        }
        refreshMouseEvt();
	}
}

void
MainFrame::onMenuPreferences(wxCommandEvent &event)
{
	// show preferences dialog
	PreferencesDialog dialog(this);
}

void
MainFrame::onUpdateMenuFileImport(wxUpdateUIEvent& event)
{
    DocCtl  dc;
    event.Enable(dc.isRuning());
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
MainFrame::refreshMouseEvt(void)
{
    m_renderView->refreshMouseEvt();
}


void
MainFrame::onMenuFileOpen(wxCommandEvent &event)
{
	OpenSceneDialog dialog(this);
	if ( dialog.ShowModal() == wxID_OK )
	{
        {
            wxBusyCursor wait;
            gme::DocIO  dio;
		    dio.loadScene(dialog.GetPath());
        }
        refreshMouseEvt();
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
MainFrame::onUpdateonMenuFileExport(wxUpdateUIEvent& event)
{
    DocCtl  dc;
    event.Enable(dc.isRuning() || dc.isPause());
}



void
MainFrame::onMenuFileSave(wxCommandEvent &event)
{
	gme::DocIO  dio;
    std::string filepath = dio.getLastLoadedPath();
	if(filepath.empty() || !boost::iends_with(filepath,".sps"))
	{
        SaveSceneDialog dialog(this);
        filepath.clear();
        if ( dialog.ShowModal() == wxID_OK )
        {
            filepath = dialog.GetPath();
        }

	}
	if(!filepath.empty())
	{
       wxBusyCursor wait;
       std::string  error = __("失败。");
	   if(dio.exportScene(filepath,false))
       {
           dio.setLastLoadedPath(filepath);
           error = __("成功。");
       }
       std::string content = boost::str(boost::format(__("保存文件'%s'%s")) % filepath % error);
       Log_Adapter(Doc::LOG_STATUS,content.c_str(),NULL);
	}
}

void
MainFrame::onUpdateMenuFileSave(wxUpdateUIEvent &event)
{
	DocCtl dctl;
    event.Enable(dctl.isPause() || dctl.isRuning());
}

void
MainFrame::onMenuFileQuit(wxCommandEvent &event)
{
	Close(false);
}

void
MainFrame::onMenuEditDelete(wxCommandEvent &event)
{
    wxBusyCursor wait;
    DocObj   dobj;
    dobj.deleteAllSelection();
}


void
MainFrame::onMenuHelpAbout(wxCommandEvent &event)
{
	DECLARE_WXCONVERT;

    std::stringstream   ss;
    ss << __("GMEDITOR version : ") << GME_VERSION_MAJOR << '.' << GME_VERSION_MINOR << std::endl;
    ss << __("build number : ") << gme_build_number << std::endl;
    ss << __("build time : ") << gme_build_time << std::endl;
    wxString content(ss.str().c_str(),gme_wx_utf8_conv);

	(void)wxMessageBox(content,
                       gmeWXT("关于飞鹿材质编辑器"),
                       wxOK | wxICON_INFORMATION);
}

void
MainFrame::onViewSelection(wxCommandEvent &event)
{
    (void)event;
    m_renderView->setViewSelection(!m_renderView->isViewSelection());
}

void
MainFrame::onUpdateViewSelection(wxUpdateUIEvent &event)
{
    event.Check(m_renderView->isViewSelection());
}

void
MainFrame::onViewSkylightDir(wxCommandEvent &event)
{
    (void)event;
    m_renderView->setViewSkylightDir(!m_renderView->isViewSkylightDir());
}

void
MainFrame::onUpdateViewSkylightDir(wxUpdateUIEvent &event)
{
    event.Check(m_renderView->isViewSkylightDir());
}

void
MainFrame::onClearLog(wxCommandEvent &event)
{
    this->m_logWindow->Clear();
}

void
MainFrame::onUpdateClearLog(wxUpdateUIEvent &event)
{
    event.Enable(!m_logWindow->IsEmpty());
}

void
MainFrame::onNewCamFromCurrent(wxCommandEvent &event)
{
    this->m_camView->newCamFromCurrent();
}

void
MainFrame::onImmRefresh(wxCommandEvent &event)
{
    wxBusyCursor wait;
	DocCtl dctl;
    dctl.refresh();
}

void
MainFrame::onUpdateImmRefresh(wxUpdateUIEvent &event)
{
	DocCtl dctl;
    event.Enable(dctl.isRuning());
}


void
MainFrame::onUpdateNewCamFromCurrent(wxUpdateUIEvent &event)
{
	DocCtl dctl;
    event.Enable(dctl.isRuning());
}


void
MainFrame::onAutoTarget(wxCommandEvent &event)
{
    (void)event;
    DocCamera dc;
    dc.autoTarget(!dc.autoTarget());
}

void
MainFrame::onUpdateAutoTarget(wxUpdateUIEvent &event)
{
    DocCamera dc;
    event.Check(dc.autoTarget());
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
    wxBusyCursor wait;
	DocCtl dctl;
	dctl.start();
}

void
MainFrame::onRenderStop(wxCommandEvent &event)
{
    wxBusyCursor wait;
	DocCtl dctl;
	dctl.stop();
}

void
MainFrame::onRenderPause(wxCommandEvent &event)
{
    wxBusyCursor wait;
	DocCtl dctl;
	dctl.pause();
}

void
MainFrame::onUpdateRenderStart(wxUpdateUIEvent& event)
{
	DocCtl dctl;
    event.Enable(dctl.isPause());
}

void
MainFrame::onUpdateRenderStop(wxUpdateUIEvent& event)
{
	DocCtl dctl;
    event.Enable(dctl.isRuning() || dctl.isPause());
}

void
MainFrame::onUpdateRenderPause(wxUpdateUIEvent& event)
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

} //namespace gme

