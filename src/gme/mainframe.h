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

#ifndef  GME_MAINFRAME_H
#define  GME_MAINFRAME_H

#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <wx/filehistory.h>
#include <wx/config.h>
#include <boost/function.hpp>

namespace gme{

class MainView;
class ObjectView;
class PropFrame;
class GlRenderView;
class CameraView;

class MainFrame : public wxFrame {
    typedef  wxFrame   inherited;
public:
    MainFrame(wxWindow* parent);
    virtual ~MainFrame();

    inline static  boost::function<bool (std::string &)>&  getImageFilepathFunc(void)
    {
        return sv_getImageFilepath;
    }
    void refreshMouseEvt(void);
    void setDocLocked(bool bLock);
    void openFile(const std::string &filepath);
    void openFile(const wxString& filepath);
    bool importMaterial(const std::string &objID,const std::string &filepath);
    bool importGlueMaterial(const std::string &objID);
    bool saveMaterial(const std::string &objID,const std::string &filepath,bool bExport);
protected:
	void onClose(wxCloseEvent& event);

	/** Processes menu File|Open */
	void onMenuFileOpen(wxCommandEvent &event);

	/** Processes menu File|Import */
	void onMenuFileImport(wxCommandEvent &event);
    void onUpdateMenuFileImport(wxUpdateUIEvent& event);

    /** Processes menu File|Save */
    void onMenuFileSaveAs(wxCommandEvent &event);
	void onMenuFileSave(wxCommandEvent &event);
    ///@brief 如果filepath不是一个可识别后缀，自动加入后缀保存。
    void saveFile(const std::string &filepath,bool bExport);
  	void onUpdateMenuFileSave(wxUpdateUIEvent& event);

	void onMenuFileSaveImage(wxCommandEvent &event);
	void onMenuPreferences(wxCommandEvent &event);
  	void onUpdateMenuFileSaveImage(wxUpdateUIEvent& event);

	/** Processes menu File|Export */
	void onMenuFileExport(wxCommandEvent &event);
  	void onUpdateonMenuFileExport(wxUpdateUIEvent& event);

	/** Processes menu File|Quit */
	void onMenuFileQuit(wxCommandEvent &event);

	/** Processes menu Edit|Delete */
	void onMenuEditDelete(wxCommandEvent &event);

	/** Processes menu About|Info */
	void onMenuHelpAbout(wxCommandEvent &event);

	void onViewSelection(wxCommandEvent &event);
    void onUpdateViewSelection(wxUpdateUIEvent &event);

	void onViewSkylightDir(wxCommandEvent &event);
    void onUpdateViewSkylightDir(wxUpdateUIEvent &event);

	void onClearLog(wxCommandEvent &event);
    void onUpdateClearLog(wxUpdateUIEvent &event);

	void onNewCamFromCurrent(wxCommandEvent &event);
    void onUpdateNewCamFromCurrent(wxUpdateUIEvent &event);

	void onImmRefresh(wxCommandEvent &event);
    void onUpdateImmRefresh(wxUpdateUIEvent &event);


	void onAutoTarget(wxCommandEvent &event);
    void onUpdateAutoTarget(wxUpdateUIEvent &event);
	void onAutoFocus(wxCommandEvent &event);
    void onUpdateAutoFocus(wxUpdateUIEvent &event);
    

	void onViewPane(wxCommandEvent &event);
    void onUpdateViewPane(wxUpdateUIEvent &event);

	/** Processes Render Start**/
	void onRenderStart(wxCommandEvent &event);

	/** Processes Render Stop**/
	void onRenderStop(wxCommandEvent &event);

	/** Processes Render Pause**/
	void onRenderPause(wxCommandEvent &event);

	/** Updata Render Menu Edit Delete UI**/
	void onUpdateMenuEditDelete(wxUpdateUIEvent& event);

	/** Updata Render UI**/
	void onUpdateRenderStart(wxUpdateUIEvent& event);
	void onUpdateRenderStop(wxUpdateUIEvent& event);
	void onUpdateRenderPause(wxUpdateUIEvent& event);


    void onEditmodeChanged(wxCommandEvent &event);
    void onUpdateEditmode(wxUpdateUIEvent &event);

	void onViewmodeChanged(wxCommandEvent &event);
    void onUpdateViewmode(wxUpdateUIEvent &event);

    void onLogLevelChanged(wxCommandEvent &event);
    void onUpdateLogLevel(wxUpdateUIEvent &event);

    void onMRUFile(wxCommandEvent& event);

	void onSaveMaterial(wxCommandEvent &event);
	void onExportMaterial(wxCommandEvent &event);
	void onImportMaterial(wxCommandEvent &event);
	void onImportGlueMaterial(wxCommandEvent &event);
    bool saveSelectMaterial(const std::string &filepath,bool bExport);
    void onUpdateMaterialOperator(wxUpdateUIEvent &event);

    void onSetting(wxCommandEvent &event);
    void onUpdateSetting(wxUpdateUIEvent &event);
    int  getLoadingFlagFromCmd(int cmdid);
private:
    wxFileHistory*  m_FileHistory;
	wxConfig*       m_Config;
    //可以通过Option,环境变量来配置glueserver.缺省是www.render001.com
    std::string     m_glueserver;
private:
    static  boost::function<bool (std::string &)>   sv_getImageFilepath;
	bool getImageFilepath(std::string &result);
    wxAuiPaneInfo&  getPaneFromCmdID(int cmdid);
	void createMenuImageItem(wxMenu* parentMenu,int id,const wxString& text, const wxString& helpString,wxBitmap bmp);
	//not need free members.
    ObjectView      *m_objectView;
    GlRenderView    *m_renderView;
	PropFrame       *m_propFrame;
	CameraView      *m_camView;
	///@todo customize log window.
	wxTextCtrl      *m_logWindow;
    void    createMenubar();
    ///@brief append shortcut and return it.
    wxString&   appendShortCutString(int cmdid,wxString &shortCut);
	void    createToolbar();
    void    createStatusbar();
    ///@todo meory leak detected by valgrind, caused by wxAuiManager loading paninfo using expat xmlparser.
    wxAuiManager    m_mgr;
    DECLARE_EVENT_TABLE()
};

}



#endif  //GME_MAINFRAME_H
