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
#include "slgutils.h"



namespace gme{


SlgUtil::Editor::~Editor()
{
    if (m_session->editActions.Has(slg::MATERIALS_EDIT)) {
        m_session->renderConfig->scene->RemoveUnusedMaterials();
        m_session->renderConfig->scene->RemoveUnusedTextures();
    }
    if( m_session->editActions.Has(slg::IMAGEMAPS_EDIT) || m_session->editActions.Has(slg::MATERIAL_TYPES_EDIT) )//m_bNeedRefresh )
    {///@fixme: we must restart render when we have IMAGEMAPS_EDIT.
        m_session->editActions.Reset();
        m_session->EndEdit();
        m_session->Stop();
        m_session->Start();
        Doc::instance().pDocData->cachefilm().invalidate();
    }else{
        if(m_session->editActions.HasAnyAction())
        {
            Doc::instance().pDocData->cachefilm().invalidate();
        }
        m_session->EndEdit();
    }
}


DocPrivate::DocPrivate(void)
{
    m_state = DocPrivate::ST_STOPPED;
    m_bAutofocus = false;
    m_bAutoTarget = false;
}

void
DocPrivate::closeScene(void)
{
    if(this->isStop())
        return;
    clearSelection();
    if(m_session)
    {
        if(this->isRunning())
        {
            m_session->Stop();
        }
        m_session.reset();
    }
    this->m_state = ST_STOPPED;
    //m_bAutofocus不变，这个选项跨场景。
    //m_bAutofocus = false;
    objManager.clear();
    matManager.clear();
    texManager.clear();
    camManager.clear();
    m_cacheFilm.invalidate();
    fireStateChanged(STATE_CLOSE);
}

DocPrivate::~DocPrivate(void)
{
    clearAllListen();
    closeScene();
}

void
DocPrivate::start(void)
{
    BOOST_ASSERT(m_session.get() != NULL);
    bool   bPause = isPause();
    m_session->Start();
    m_state = ST_RUNNING;
    if(!bPause)
    {
        fireStateChanged(STATE_OPEN);
        fireSizeChanged();
    }
}

void
DocPrivate::pause(void)
{
    if(!this->isRunning())
        return;

    m_cacheFilm.saveNativeFilm();
    m_session->Stop();
    m_state = ST_PAUSED;
    fireStateChanged(STATE_PAUSE);
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
    	    cachefilm().invalidate();
        }
    }
}

bool
DocPrivate::getNativeRenderInfo(RenderInfo &ri)
{
    slg::RenderSession* session = getSession();
    if(session && session->film)
    {
        ri.convergence = session->renderEngine->GetConvergence();
        ri.elapsedTime = session->renderEngine->GetRenderingTime();
        ri.pass = session->renderEngine->GetPass();
        ri.totalRaysSec = session->renderEngine->GetTotalRaysSec();
        ri.totalSamplesSec = session->renderEngine->GetTotalSamplesSec();
        return true;
    }
    return false;
}

}


