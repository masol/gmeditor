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
#include "utils/option.h"
#include "dm/docimg.h"
#include "dm/docio.h"
#include "dm/doccamera.h"
#include "glrenderframe.h"
#include "../cmdids.h"


namespace gme{

BEGIN_EVENT_TABLE(GlRenderFrame, inherited)

    EVT_MOTION(GlRenderFrame::mouseMoved)
    EVT_LEFT_DOWN(GlRenderFrame::mouseLeftDown)
    EVT_LEFT_UP(GlRenderFrame::mouseLeftReleased)
    EVT_MIDDLE_DOWN(GlRenderFrame::mouseMiddleDown)
    EVT_MIDDLE_UP(GlRenderFrame::mouseMiddleReleased)
    EVT_RIGHT_DOWN(GlRenderFrame::rightClick)
    EVT_LEAVE_WINDOW(GlRenderFrame::mouseLeftWindow)
    EVT_KEY_DOWN(GlRenderFrame::keyPressed)
    EVT_KEY_UP(GlRenderFrame::keyReleased)
    EVT_MOUSEWHEEL(GlRenderFrame::mouseWheelMoved)

    EVT_TIMER(wxID_ANY, GlRenderFrame::onRefreshTimer)

    // catch paint events
    EVT_PAINT(GlRenderFrame::paintEvent)
END_EVENT_TABLE()


GlRenderFrame::GlRenderFrame(wxWindow* parent,int *args,int vm) : inherited(parent,wxID_ANY,args,wxDefaultPosition, wxDefaultSize) , m_refreshTimer(this)
{
    m_context.reset(new wxGLContext(this));

    m_edit_mode = cmd::GID_MD_ROTATE;

    m_needClearColor = true;
    m_rorateAroundTarget = false;
    m_view_selection = true;

    m_v2dTranslate << 0.0f,0.0f;
    m_v2dScale << 1.0f,1.0f;

    m_viewMode = vm;
    m_docWidth = m_docHeight = 0;
    ///@todo: connection from option.
    SetMinSize( wxSize(20, 20) );
    opt_RotateStep = 4.f;
    m_micro_tick = boost::posix_time::microsec_clock::local_time();

    //0.1 second.
    opt_MinEditInterval = 200;
    opt_MinUpdateInterval = 200;
    m_refreshTimer.Stop();

    {
        DocIO   dio;
        dio.onSceneLoaded(boost::bind(&GlRenderFrame::onSceneLoaded,this));
        dio.onSceneClosed(boost::bind(&GlRenderFrame::onSceneClosed,this));
    }


    // To avoid flashing on MSW
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    // To avoid windows lose focus.
    this->SetFocus();
}

void
GlRenderFrame::initGL(void)
{
    if(!wxGLCanvas::SetCurrent(*m_context))
    {
        BOOST_ASSERT(false);
        return;
    }
    glShadeModel(GL_FLAT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);

    glDisable(GL_COLOR_MATERIAL);
    glClearColor(0, 0, 0, 0);
    glClearStencil(0);
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);

    GLfloat lightKa[] = {1.0f, 1.0f, 1.0f, 1.0f};  // ambient light
    GLfloat lightKd[] = {1.0f, 1.0f, 1.0f, 1.0f};  // diffuse light
    GLfloat lightKs[] = {1, 1, 1, 1};           // specular light
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

    // position the light
//    float lightPos[4] = {0, 0, 20, 1}; // positional light
//    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // enable light source after configuration.
    glEnable(GL_LIGHT0);

}


GlRenderFrame::~GlRenderFrame()
{
}

void
GlRenderFrame::onSceneLoaded(void)
{
    if(!this->m_refreshTimer.IsRunning())
        this->m_refreshTimer.Start(opt_MinUpdateInterval);
}

void
GlRenderFrame::refreshImmediate(void)
{
    //this->Update();
    //this->render();
    //if(this->m_refreshTimer.IsRunning())
    //{
    //    this->m_refreshTimer.Stop();
    //    this->m_refreshTimer.Start(opt_MinUpdateInterval);
    //}
}


void
GlRenderFrame::onSceneClosed(void)
{
    if(this->m_refreshTimer.IsRunning())
        this->m_refreshTimer.Stop();
}

void
GlRenderFrame::onRefreshTimer(wxTimerEvent& WXUNUSED(event))
{
    this->Refresh(false);
}

void GlRenderFrame::paintEvent(wxPaintEvent & evt)
{
    render();
}

void
GlRenderFrame::drawBackground(const wxSize &winsize,const float *pixels,gme::DocImg::ViewPort &vp)
{
    switch(m_viewMode)
    {
    case VM_ADJDOC:
    case VM_DOCSIZE:
        {//居中显示
            int startx = (winsize.x - m_docWidth)/2;
            int starty = (winsize.y - m_docHeight)/2;

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            //glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
            glPixelZoom( 1.0f  , 1.0f );
            m_v2dScale << 1.0f,1.0f;
            m_v2dTranslate << -startx, -starty;

            vp.x = startx;
            vp.y = starty;
            vp.width = m_docWidth;
            vp.height = m_docHeight;

            glViewport(startx, starty, m_docWidth, m_docHeight);
            glOrtho(0.f, winsize.x - 1.f,
                    0.f, winsize.y - 1.f, -1.f, 1.f);

            glRasterPos2i(0, 0);
            glDrawPixels(m_docWidth, m_docHeight, GL_RGB, GL_FLOAT, pixels);
        }
        break;
    case VM_FULLWINDOW:
        {//缩放至全屏
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();

            m_v2dScale << (float)winsize.x / (float)m_docWidth , (float)winsize.y / (float)m_docHeight;
            m_v2dTranslate << 0.0f, 0.0f;

            glPixelZoom( m_v2dScale[0] , m_v2dScale[1] );
            glViewport(0, 0, winsize.x, winsize.y);
            glOrtho(0.f, winsize.x - 1.f,
                    0.f, winsize.y - 1.f, -1.f, 1.f);
            glRasterPos2i(0, 0);

            vp.x = 0;
            vp.y = 0;
            vp.width = winsize.x;
            vp.height = winsize.y;

            glDrawPixels(m_docWidth, m_docHeight, GL_RGB, GL_FLOAT, pixels);
        }
        break;
    case VM_SCALEWITHASPECT:
        {//缩放至全屏
            float   docAspect = (float)m_docHeight / (float)m_docWidth;
            float   winAspect = (float)winsize.y / (float)winsize.x;

            int realWidth,realHeight;
            if(docAspect < winAspect)
            {
                realWidth = winsize.x;
                realHeight = (int)(realWidth * docAspect);
            }else{
                realHeight = winsize.y;
                realWidth = (int)(realHeight / docAspect);
            }

            int startx = (winsize.x - realWidth)/2;
            int starty = (winsize.y - realHeight)/2;

            m_v2dScale << (float)realWidth / (float)m_docWidth  , (float)realHeight / (float)m_docHeight;
            m_v2dTranslate << -startx, -starty;

            glMatrixMode(GL_PROJECTION);

            vp.x = startx;
            vp.y = starty;
            vp.width = realWidth;
            vp.height = realHeight;

            //glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
            glViewport(startx, starty, realWidth,realHeight);
            glPixelZoom( m_v2dScale[0] , m_v2dScale[1] );
            glLoadIdentity();
            glOrtho(0.f, winsize.x - 1.f,
                    0.f, winsize.y - 1.f, -1.f, 1.f);

            glRasterPos2i(0, 0);
            glDrawPixels(m_docWidth, m_docHeight, GL_RGB, GL_FLOAT, pixels);
        }
        break;
    default:
        BOOST_ASSERT_MSG(false,"unrachable code");
        break;
    }
}


void GlRenderFrame::render(void)
{
    if(!IsShown()) return;
    if(!wxGLCanvas::SetCurrent(*m_context))
        return;
    wxPaintDC(this); // only to be used in paint events. use wxClientDC to paint outside the paint event

    DocImg  img;
    if(!img.getSize(m_docWidth,m_docHeight))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
        SwapBuffers();
        return;
    }

    wxSize  winsize = this->GetSize();

    ///@TODO :我们在这里检查尺寸.将其挪到消息响应里。
    if(m_viewMode == VM_ADJDOC)
    {
        if(m_docWidth != winsize.x || m_docHeight != winsize.y)
        {
            img.setSize(winsize.x,winsize.y);
            return;
        }
    }

    const float*    pixels = img.getPixels();
    BOOST_ASSERT_MSG(pixels != NULL, "why no pixels?");


    //glClear(GL_ALL_ATTRIB_BITS);
    if(m_needClearColor)
    {
        glClear( GL_COLOR_BUFFER_BIT );
        m_needClearColor = false;
    }

    gme::DocImg::ViewPort vp;

    drawBackground(winsize,pixels,vp);

    if(m_view_selection)
    {//draw selection.
        img.drawSelectedObject(vp);
    }

    //glFlush();
    SwapBuffers();
}

void
GlRenderFrame::translateCam(wxMouseEvent& event)
{
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration diff = now - m_micro_tick;
    if(diff.total_milliseconds() > opt_MinEditInterval)
    {
        long x,y;
        event.GetPosition(&x,&y);
		if(x != m_lastx || y != m_lasty)
		{
			gme::DocCamera doccam;
            doccam.translate( XDiffV2D(m_lastx - x), YDiffV2D(m_lasty - y),getFactor(event));
			m_lastx = x;
			m_lasty = y;
			m_micro_tick = now;
            this->refreshImmediate();
		}
    }
}

void
GlRenderFrame::zoomCam(wxMouseEvent& event)
{
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration diff = now - m_micro_tick;
    if(diff.total_milliseconds() > opt_MinEditInterval)
    {
        long x,y;
        event.GetPosition(&x,&y);
		if(x != m_lastx || y != m_lasty)
		{
			gme::DocCamera doccam;
			int doc_diffy = YDiffV2D(m_lasty - y);
            float step = (float)doc_diffy / 10.0f;

            doccam.straightTranslate( step * getFactor(event));

			m_lastx = x;
			m_lasty = y;
			m_micro_tick = now;
            this->refreshImmediate();
		}
    }
}


void
GlRenderFrame::rotateCamAroundCenter(wxMouseEvent& event)
{
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration diff = now - m_micro_tick;
    if(diff.total_milliseconds() > opt_MinEditInterval)
    {
        long x,y;
        event.GetPosition(&x,&y);
		if(x != m_lastx || y != m_lasty)
		{
			gme::DocCamera doccam;
			int doc_diffx = XDiffV2D(m_lastx - x);
			int doc_diffy = YDiffV2D(m_lasty - y);
            doccam.targetRotate(doc_diffx, doc_diffy,getFactor(event));
			m_lastx = x;
			m_lasty = y;
			m_micro_tick = now;
            this->refreshImmediate();
		}
    }
}


void
GlRenderFrame::rotateCam(wxMouseEvent& event)
{
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration diff = now - m_micro_tick;
    if(diff.total_milliseconds() > opt_MinEditInterval)
    {
        long x,y;
        event.GetPosition(&x,&y);
		if(x != m_lastx || y != m_lasty)
		{
			gme::DocCamera doccam;
			int doc_diffx = XDiffV2D(m_lastx - x);
			int doc_diffy = YDiffV2D(m_lasty - y);
            doccam.rotate(doc_diffx, doc_diffy,getFactor(event));
			m_lastx = x;
			m_lasty = y;
			m_micro_tick = now;
            this->refreshImmediate();
		}
    }
}

void GlRenderFrame::mouseLeftDown(wxMouseEvent& event)
{
    event.GetPosition(&m_lastx,&m_lasty);
    // To avoid windows lose focus.
    this->SetFocus();
}

void GlRenderFrame::doMouseEvent(wxMouseEvent& event)
{
    switch(m_edit_mode)
    {
    case cmd::GID_MD_PANE:
        translateCam(event);
        break;
    case cmd::GID_MD_ROTATE:
        rotateCam(event);
        break;
    case cmd::GID_MD_ROTATE_AROUND_CENTER:
        rotateCamAroundCenter(event);
        break;
    case cmd::GID_MD_ZOOM:
        zoomCam(event);
        break;
    }
}

void GlRenderFrame::mouseLeftReleased(wxMouseEvent& event)
{
    doMouseEvent(event);
}

void GlRenderFrame::mouseMiddleDown(wxMouseEvent& event)
{
    event.GetPosition(&m_lastx,&m_lasty);
    // To avoid windows lose focus.
    this->SetFocus();
}

void GlRenderFrame::mouseMiddleReleased(wxMouseEvent& event)
{
}

void GlRenderFrame::mouseLeftWindow(wxMouseEvent& event)
{
}

void GlRenderFrame::mouseMoved(wxMouseEvent& event)
{
    if(m_edit_mode == cmd::GID_MD_LOCK)
        return;
    bool   bLeftdown = event.ButtonIsDown(wxMOUSE_BTN_LEFT);
    bool   bMiddledown = event.ButtonIsDown(wxMOUSE_BTN_MIDDLE);
    if(bLeftdown && bMiddledown)
    {//left_middle down.
        zoomCam(event);
    }else if(bLeftdown)
    {//根据编辑模式
        doMouseEvent(event);
    }
    else if(bMiddledown)
    {
        translateCam(event);
    }
}

void GlRenderFrame::mouseWheelMoved(wxMouseEvent& event)
{
    if(m_edit_mode == cmd::GID_MD_LOCK)
        return;
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration diff = now - m_micro_tick;
    if(diff.total_milliseconds() > opt_MinEditInterval)
    {
//        std::cerr << "deltal=" << delta <<  ",GetWheelRotation = " <<  event.GetWheelRotation() << std::endl;
        float step = (float) event.GetWheelRotation()  /  (float)event.GetWheelDelta();

		gme::DocCamera doccam;
        doccam.straightTranslate( step * getFactor(event));
		m_micro_tick = now;
        this->refreshImmediate();
    }
}

void GlRenderFrame::rightClick(wxMouseEvent& event)
{
}

void GlRenderFrame::keyPressed(wxKeyEvent& event)
{
}

void GlRenderFrame::keyReleased(wxKeyEvent& event)
{
}

}//end namespace gme

