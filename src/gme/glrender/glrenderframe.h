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
#include <Eigen/Core>
#include "dm/docimg.h"

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

    wxTimer  m_refreshTimer;
    long     opt_MinUpdateInterval;
    float    opt_RotateStep;
    long     opt_MinEditInterval;
    long     m_lastx;
    long     m_lasty;
    int      m_edit_mode; //左钮编辑模式。
    bool     m_view_selection; //是否绘制选中对象。
    inline float  getFactor(wxMouseEvent& event)
    {
        float factor = 1.0f;
        if(event.ShiftDown())
            factor = 10.0f;
        else if(event.ControlDown())
            factor = 0.1f;
        return factor;
    }
    void    refreshImmediate(void);
    void    onSceneLoaded(void);
    void    onSceneClosed(void);
    void    rotateCam(wxMouseEvent& event);
    void    translateCam(wxMouseEvent& event);
    void    rotateCamAroundCenter(wxMouseEvent& event);
    void    zoomCam(wxMouseEvent& event);
    void    doMouseEvent(wxMouseEvent& event);
    ///@brief virtual coord to document coord translate. update after background draw called.
    ///@detail our coord transform no rotation.
    Eigen::Vector2f     m_v2dTranslate;
    ///@brief virtual coord to document coord scale. update after background draw called.
    Eigen::Vector2f     m_v2dScale;

    inline int  XDiffV2D(int diff)
    {
        return diff / m_v2dScale[0];
    }
    inline int  YDiffV2D(int diff)
    {
        return diff / m_v2dScale[1];
    }

    inline wxPoint     V2D(const wxPoint &pt)
    {
        return wxPoint((pt.x + m_v2dTranslate[0]) / m_v2dScale[0],(pt.y + m_v2dTranslate[1]) / m_v2dScale[1]);
    }
    inline wxPoint     D2V(const wxPoint &pt)
    {
        return wxPoint((pt.x * m_v2dScale[0] - m_v2dTranslate[0]),(pt.y * m_v2dScale[1] - m_v2dTranslate[1]));
    }

    int         m_viewMode;
    int         m_docWidth;
    int         m_docHeight;
    ///@brief 指示下次绘制时是否需要清背景。
    bool        m_needClearColor;
    ///@brief 指示旋转模式是否绕中心点旋转。如果是false则绕位置点旋转。缺省是false.
    bool        m_rorateAroundTarget;
protected:
    void    drawBackground(const wxSize &winsize,const float *pixels,gme::DocImg::ViewPort &vp);
    void    initGL(void);
public:
    GlRenderFrame(wxWindow* parent,int* args,int vm);
    virtual ~GlRenderFrame();
    inline bool    viewSelection(void)const
    {
        return m_view_selection;
    }
    inline void    viewSelection(bool bv)
    {
        m_view_selection = bv;
    }
    inline int     editMode(void)const
    {
        return m_edit_mode;
    }
    inline void    editMode(int mode)
    {
        m_edit_mode = mode;
    }
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
    void mouseLeftReleased(wxMouseEvent& event);
    void mouseMiddleDown(wxMouseEvent& event);
    void mouseMiddleReleased(wxMouseEvent& event);
    void mouseWheelMoved(wxMouseEvent& event);
    void rightClick(wxMouseEvent& event);
    void mouseLeftWindow(wxMouseEvent& event);
    void keyPressed(wxKeyEvent& event);
    void keyReleased(wxKeyEvent& event);
    void onRefreshTimer(wxTimerEvent& WXUNUSED(event));

    DECLARE_EVENT_TABLE()
};

} //end namespace gme

#endif //GME_GLRENDER_GLRENDERFRAME_H
