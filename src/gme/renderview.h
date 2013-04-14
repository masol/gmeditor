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

#ifndef  GME_RENDERVIEW_H
#define  GME_RENDERVIEW_H

#include <wx/wx.h>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace gme{

class RenderView : public wxScrolledWindow
{
typedef wxScrolledWindow    inherited;
protected:
    boost::posix_time::ptime    m_micro_tick;
    float    opt_RotateStep;
    long     opt_MinEditInterval;
    long     m_lastx;
    long     m_lasty;
    int      m_action;
    enum{
        ACTION_INVALID,
        ACTION_CAM_ROTATE
    };
    void    rotateCam(wxMouseEvent& event);
public:
    RenderView(wxFrame* parent);
    virtual ~RenderView();

    void paintEvent(wxPaintEvent & evt);
    void paintNow();
    void render(wxDC& dc);

    // some useful events
    void mouseMoved(wxMouseEvent& event);
    void mouseLeftDown(wxMouseEvent& event);
    void mouseWheelMoved(wxMouseEvent& event);
    void mouseLeftReleased(wxMouseEvent& event);
    void rightClick(wxMouseEvent& event);
    void mouseLeftWindow(wxMouseEvent& event);
    void keyPressed(wxKeyEvent& event);
    void keyReleased(wxKeyEvent& event);
    void onIdle(wxIdleEvent &event);

    DECLARE_EVENT_TABLE()
};

} //end namespace gme

#endif //GME_RENDERVIEW_H
