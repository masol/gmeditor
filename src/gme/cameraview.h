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

#ifndef  GME_CAMERAVIEW_H
#define  GME_CAMERAVIEW_H

#include <wx/wx.h>
#include <wx/listctrl.h>

namespace gme
{

class CameraView : public wxListCtrl
{
private:
    typedef wxListCtrl inherited;
    long    m_menuCmdTarget;
protected:

    void OnSelectionChanged(wxListEvent& event);
    void OnEndLabelEdit(wxListEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);
    void onMenuNewFromCurrent(wxCommandEvent &event);
    void onMenuDelete(wxCommandEvent& event);
    void refresh(void);
private:
	void  onDocumentOpend(void);
	void  onDocumentClosed(void);
public:
    CameraView(wxWindow* parent, wxWindowID id,const wxPoint& pos, const wxSize& size);
    ~CameraView();
    void    newCamFromCurrent(void);
private:
    wxDECLARE_EVENT_TABLE();
};

}


#endif //GME_CAMERAVIEW_H
