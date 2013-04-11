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
#include "mainframe.h"
#include "mainview.h"


namespace gme{

MainFrame::MainFrame(wxWindow* parent) : wxFrame(parent, -1, _("GMEditor"),
                  wxDefaultPosition, wxSize(800,600),
                  wxDEFAULT_FRAME_STYLE)                              
{
    // notify wxAUI which frame to use
    m_mgr.SetManagedWindow(this);

    // create several text controls
    wxTextCtrl* text1 = new wxTextCtrl(this, -1, _("Pane 1 - sample text"),
                  wxDefaultPosition, wxSize(200,150),
                  wxNO_BORDER | wxTE_MULTILINE);
                            
    MainView* text3 = new MainView(this);
     
    wxTextCtrl* text2 = new wxTextCtrl(this, -1, _("Pane 2 - sample text"),
                  wxDefaultPosition, wxSize(200,150),
                  wxNO_BORDER | wxTE_MULTILINE);
                                    
    // add the panes to the manager
    m_mgr.AddPane(text1, wxLEFT, wxT("Pane Number One"));
    m_mgr.AddPane(text2, wxBOTTOM, wxT("Pane Number Two"));
    m_mgr.AddPane(text3, wxCENTER);
                           
    // tell the manager to "commit" all the changes just made
    m_mgr.Update();
}

MainFrame::~MainFrame()
{
    // deinitialize the frame manager
    m_mgr.UnInit();
}
 
} //namespace gme

