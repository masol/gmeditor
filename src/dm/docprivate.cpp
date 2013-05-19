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
#include "docprivate.h"
#include "dm/docmat.h"
#include "slgobject.h"
#include "slgtexture.h"
#include "slgmaterial.h"



namespace gme{

DocPrivate::DocPrivate(void)
{
    m_started = false;
    m_bAutofocus = false;
    m_bAutoTarget = false;
}

void
DocPrivate::closeScene(void)
{
    clearSelection();
    if(m_session)
    {
        if(m_started)
        {
            m_session->Stop();
        }
        m_session.reset();
    }
    m_started = false;
    //m_bAutofocus不变，这个选项跨场景。
    //m_bAutofocus = false;
    objManager.clear();
    matManager.clear();
    texManager.clear();
    camManager.clear();
    fireStateChanged(STATE_CLOSE);
}

DocPrivate::~DocPrivate(void)
{
    clearAllListen();
    closeScene();
}

void
DocPrivate::onSelectedChanged(void)
{
    if(m_bAutoTarget && this->m_session.get() && this->m_session->renderConfig->scene)
    {
        luxrays::BBox bbox = objManager.getSelectionBBox();
        if(bbox.IsValid())
        {
    	    m_session->BeginEdit();
    
            slg::PerspectiveCamera *camera = this->m_session->renderConfig->scene->camera;

            camera->target = bbox.Center();
    	    camera->Update(m_session->film->GetWidth(), m_session->film->GetHeight());
	        m_session->editActions.AddAction(slg::CAMERA_EDIT);
    	    m_session->EndEdit();
        }
    }
}



}


