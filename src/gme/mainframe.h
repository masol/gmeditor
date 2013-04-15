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

namespace gme{

class MainView;

class MainFrame : public wxFrame {
    typedef  wxFrame   inherited;
public:
    MainFrame(wxWindow* parent);
    virtual ~MainFrame();
	void onClose(wxCloseEvent& event);
public:
    void onSize(wxSizeEvent& event);

	/** Processes menu File|Open */
	void onMenuFileOpen(wxCommandEvent &event);

	/** Processes menu File|Save */
	void onMenuFileSave(wxCommandEvent &event);

	/** Processes menu File|Export */
	void onMenuFileExport(wxCommandEvent &event);

	/** Processes menu File|Quit */
	void onMenuFileQuit(wxCommandEvent &event);

	/** Processes menu About|Info */
	void onMenuHelpAbout(wxCommandEvent &event);
	enum{
		SFP_INFO = 0,
		SFP_STAT = 1,
		SFP_TIME_ELAPSE = 2,
		SFP_PROG = 3,
		SFP_TIME_TOTAL = 4,
		SFP_TOTOAL = 5
	};
private:
    inline void updateProgressbar(){
        wxRect rect;
        wxStatusBar *pSB = this->GetStatusBar();
        if(pSB && pSB->GetFieldRect(SFP_PROG,rect))
        {
            m_pGauge->SetSize(rect.GetWidth(),rect.GetHeight());
            m_pGauge->Move(rect.x,rect.y);
        }
    }
    wxGauge         *m_pGauge;
    void    createMenubar();
    void    createStatusbar();
    //@TODO meory leak detected by valgrind, caused by wxAuiManager loading paninfo using expat xmlparser.
    wxAuiManager    m_mgr;
    DECLARE_EVENT_TABLE()
};

}



#endif  //GME_MAINFRAME_H
