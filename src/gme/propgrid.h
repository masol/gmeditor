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

#ifndef GME_PROPGRID_H
#define GME_PROPGRID_H

#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <wx/propgrid/propgrid.h>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include "dm/docsetting.h"

namespace gme{

class PropFrame : public wxScrolledWindow
{
public:
    PropFrame(wxFrame *parent, wxWindowID id, const wxPoint& pos, const wxSize& size , const long& style);
    ~PropFrame();

    wxPropertyGridManager*  m_pPropGridManager;
    wxPropertyGrid*     m_propGrid;

    wxWindow*       m_panel;
    wxBoxSizer*     m_topSizer;

    wxMenuItem*     m_itemVetoDragging;

private:
    void CreateGrid( int style, int extraStyle );
    void FinalizeFramePosition();

    // These are used in CreateGrid(), and in tests to compose
    // grids for testing purposes.
    void InitPanel();
    void PopulateGrid();
    void FinalizePanel( bool wasCreated = true );

    void PopulateWithScene();
    void PopulateWithFilm();

    void OnPropertyGridChange( wxPropertyGridEvent& event );
    void OnPropertyGridChanging( wxPropertyGridEvent& event );
    void OnPropertyGridSelect( wxPropertyGridEvent& event );
    void OnPropertyGridHighlight( wxPropertyGridEvent& event );
    void OnPropertyGridItemRightClick( wxPropertyGridEvent& event );
    void OnPropertyGridItemDoubleClick( wxPropertyGridEvent& event );
    void OnPropertyGridPageChange( wxPropertyGridEvent& event );
    void OnPropertyGridTextUpdate( wxCommandEvent& event );
    void OnPropertyGridKeyEvent( wxKeyEvent& event );

    //void OnAbout( wxCommandEvent& event );

    void OnMove( wxMoveEvent& event );
    void OnResize( wxSizeEvent& event );
    void OnPaint( wxPaintEvent& event );
    void OnCloseEvent( wxCloseEvent& event );

    void OnIdle( wxIdleEvent& event );

public:
	// update properties value
	void updateProps();
	void showMatProps(const std::string &matId);

private:
	DocSettingHelper	m_docsettingHelper;
	std::string			m_filmPgName;
	std::string			m_matPgName;
    DECLARE_EVENT_TABLE()
};

} //end namespace gme

#endif // GME_PROPGRID_H
