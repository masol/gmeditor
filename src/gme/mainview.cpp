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
#include "mainview.h"
#include "utils/option.h"

namespace gme{

BEGIN_EVENT_TABLE(MainView, inherited)

    EVT_MOTION(MainView::mouseMoved)
    EVT_LEFT_DOWN(MainView::mouseDown)
    EVT_LEFT_UP(MainView::mouseReleased)
    EVT_RIGHT_DOWN(MainView::rightClick)
    EVT_LEAVE_WINDOW(MainView::mouseLeftWindow)
    EVT_KEY_DOWN(MainView::keyPressed)
    EVT_KEY_UP(MainView::keyReleased)
    EVT_MOUSEWHEEL(MainView::mouseWheelMoved)

    // catch paint events
    EVT_PAINT(MainView::paintEvent)
END_EVENT_TABLE()



MainView::MainView(wxFrame* parent) : inherited(parent)
{
    //@TODO: connection from option.
    SetMinSize( wxSize(20, 20) );
}

MainView::~MainView()
{
}
 
/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */
void MainView::paintEvent(wxPaintEvent & evt)
{
    // depending on your system you may need to look at double-buffered dcs
    wxPaintDC dc(this);
    render(dc);
}
 
/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 */
void MainView::paintNow()
{
    // depending on your system you may need to look at double-buffered dcs
    wxClientDC dc(this);
    render(dc);
}
 
/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void MainView::render(wxDC&  dc)
{
    dc.SetBrush( *wxRED_BRUSH );

    int w,h;
    this->GetSize(&w,&h);
    dc.DrawRectangle( 0, 0, w, h );
}
 
void MainView::mouseDown(wxMouseEvent& event)
{
}

void MainView::mouseReleased(wxMouseEvent& event)
{
//    wxMessageBox( wxT("You pressed a custom button") );
}

void MainView::mouseLeftWindow(wxMouseEvent& event)
{
}
 
void MainView::mouseMoved(wxMouseEvent& event)
{
}

void MainView::mouseWheelMoved(wxMouseEvent& event)
{
}

void MainView::rightClick(wxMouseEvent& event)
{
}

void MainView::keyPressed(wxKeyEvent& event)
{
}

void MainView::keyReleased(wxKeyEvent& event)
{
}

}//end namespace gme

