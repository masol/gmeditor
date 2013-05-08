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
#include "dm/doccamera.h"
#include "glrenderview.h"
#include "glrender/glrenderframe.h"
#include "boost/bind.hpp"
#include "cmdids.h"


namespace gme{

const char* GlRenderView::vm_opt_path = "window.editor.viewmode";

BEGIN_EVENT_TABLE(GlRenderView, GlRenderView::inherited)
    EVT_MOUSEWHEEL(GlRenderView::mouseWheelMoved)
END_EVENT_TABLE()


GlRenderView::GlRenderView(wxFrame* parent) : inherited(parent,wxID_ANY)
{
    //初始化显示模式。
    int viewMode = Option::instance().get<int>(vm_opt_path,GlRenderFrame::VM_DOCSIZE);

    int args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};
    m_glframe = new GlRenderFrame(this,args,viewMode);

    m_sizer = new wxBoxSizer(wxVERTICAL);
    m_sizer->Add(m_glframe,wxSizerFlags(1).Expand());


    SetSizer(m_sizer);

    DocImg  img;
    int w,h;
    if(img.getSize(w,h))
    {
        m_glframe->docWidth(w);
        m_glframe->docHeight(h);
    }

    img.onImagesizeChanged(boost::bind(&GlRenderView::onDocumentSizeChanged,this,_1,_2));

    Option::instance().connect(vm_opt_path,boost::bind(&GlRenderView::onEditModeChanged,this,_2));
    updateViewmode(viewMode);
    this->SetScrollRate(5, 5);
}

GlRenderView::~GlRenderView()
{
}

void
GlRenderView::setViewmode(int mode)
{
    if(mode != this->m_glframe->viewMode())
    {
        this->m_glframe->viewMode(mode);
        updateViewmode(this->m_glframe->viewMode());
    }
}


void
GlRenderView::updateViewmode(int viewMode)
{
//    std::cerr << "enter GlRenderView::updateViewmode" << std::endl;
    switch(viewMode)
    {
    case GlRenderFrame::VM_DOCSIZE:
        if(m_glframe->docWidth() && m_glframe->docHeight())
        {
//            std::cerr << "SetItemMinSize to " << m_glframe->docWidth() << ',' << m_glframe->docHeight() << std::endl;
            m_sizer->SetItemMinSize(m_glframe,m_glframe->docWidth(),m_glframe->docHeight());
        }else{
            m_sizer->SetItemMinSize(m_glframe,0,0);
        }
        break;
    case GlRenderFrame::VM_FULLWINDOW:
        m_sizer->SetItemMinSize(m_glframe,0,0);
        break;
    case GlRenderFrame::VM_SCALEWITHASPECT:
        m_sizer->SetItemMinSize(m_glframe,0,0);
        break;
    case GlRenderFrame::VM_ADJDOC:
        m_sizer->SetItemMinSize(m_glframe,0,0);
        break;
    default:
        BOOST_ASSERT_MSG(false,"unrachable code");
    }
    this->FitInside();
}


void
GlRenderView::onDocumentSizeChanged(int w,int h)
{
    ///@todo thread switch needed here!
    m_glframe->docWidth(w);
    m_glframe->docHeight(h);
    updateViewmode(this->m_glframe->viewMode());
}

bool
GlRenderView::onEditModeChanged(const boost::any &nvar)
{
    ///@todo thread switch needed here!
    int mode = boost::any_cast<int>( nvar );
    if(mode != this->m_glframe->viewMode())
    {
        setViewmode(mode);
    }
    return true;
}

int
GlRenderView::getViewmodeFromCmd(int cmdid)
{
    switch(cmdid)
    {
    case cmd::GID_VM_ADJDOC:
        return GlRenderFrame::VM_ADJDOC;
    case cmd::GID_VM_DOCSIZE:
        return GlRenderFrame::VM_DOCSIZE;
    case cmd::GID_VM_FULLWINDOW:
        return GlRenderFrame::VM_FULLWINDOW;
    case cmd::GID_VM_SCALEWITHASPECT:
        return GlRenderFrame::VM_SCALEWITHASPECT;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code!");
        break;
    }
    throw std::runtime_error("invalid position");
}

bool
GlRenderView::isCurrentViewmodeFromCmd(int cmds)
{
    return (this->m_glframe->viewMode() == getViewmodeFromCmd(cmds));
}

void
GlRenderView::setViewmodeFromCmd(int cmds)
{
    this->setViewmode(getViewmodeFromCmd(cmds));
}

void
GlRenderView::setEditmodeFromCmd(int mode)
{
    this->m_glframe->editMode(mode);
}

int
GlRenderView::getEditmodeCmd(void)
{
    return  this->m_glframe->editMode();
}




}//end namespace gme

