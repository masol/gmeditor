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

#ifndef  GME_GLRENDER_GLRENDERFRAME_H
#define  GME_GLRENDER_GLRENDERFRAME_H

#include <wx/wx.h>
#include "wx/glcanvas.h"
#include <boost/date_time/posix_time/posix_time.hpp>

//include OpenGL
#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

//#define FREEGLUT_STATIC
//// Jens's patch for MacOS
//#if defined(__APPLE__)
//#include <GLUT/glut.h>
//#else
//#include <GL/glut.h>
//#endif


namespace gme{

class GlRenderFrame : public wxGLCanvas
{
public:
    enum{
        VM_DOCSIZE,    //按照doc的size来显示，如果显示区域小，自动卷滚。
        VM_FULLWINDOW,  //自动缩放至全屏.
        VM_SCALEWITHASPECT,  //缩放，但是保持aspect不变。
        VM_ADJDOC       //按照window size自动矫正DocSize.
    };
    typedef wxGLCanvas    inherited;
protected:
    boost::shared_ptr<wxGLContext>  m_context;
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

    int         m_viewMode;
    int         m_docWidth;
    int         m_docHeight;
    ///@brief 指示下次绘制时是否需要清背景。
    bool        m_needClearColor;
protected:
    void    drawBackground(const wxSize &winsize,const float *pixels);
public:
    GlRenderFrame(wxWindow* parent,int* args,int vm);
    virtual ~GlRenderFrame();
    inline int     viewMode(void)const
    {
        return m_viewMode;
    }
    inline void    viewMode(int m)
    {
        m_viewMode = m;
        m_needClearColor = true;
    }
    inline void   docWidth(int w)
    {
        m_docWidth = w;
        m_needClearColor = true;
    }
    inline int    docWidth()const
    {
        return m_docWidth;
    }
    inline void   docHeight(int w)
    {
        m_docHeight = w;
        m_needClearColor = true;
    }
    inline int    docHeight()const
    {
        return m_docHeight;
    }
protected:
    void render(void);
    void paintEvent(wxPaintEvent & evt);

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

#endif //GME_GLRENDER_GLRENDERFRAME_H
