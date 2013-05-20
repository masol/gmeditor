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
#include "cachefilm.h"
#include "docprivate.h"
#include "dm/docimg.h"


namespace gme{

class PauseFilm : public ContributeFilm
{
public:
    PauseFilm(slg::Film *film) : ContributeFilm(CT_PAUSE)
    {
        m_film = new slg::Film(film->GetWidth(),film->GetHeight());
        m_film->CopyDynamicSettings(*film);
        m_film->Init();
        m_film->AddFilm(*film);
    }
};


CacheFilm::CacheFilm(void)
{
    m_totalFilm = NULL;
    //最小合并contribute的周期是2s.
    opt_MinUpdateInterval = 2000;
    m_lastMerge_tick = boost::posix_time::microsec_clock::local_time();
}

CacheFilm::~CacheFilm(void)
{
    invalidate();
}

slg::Film*
CacheFilm::getTotalFilmFromNative(slg::RenderSession  *session)
{
    if(!m_totalFilm)
    {
        if(!m_totalFilm)
        {
            m_totalFilm = new slg::Film(session->film->GetWidth(),session->film->GetHeight());
            m_totalFilm->CopyDynamicSettings(*(session->film));
            m_totalFilm->Init();
        }
    }
    return m_totalFilm;
}


void
CacheFilm::updateNativeFilm(void)
{
    if(!Doc::instance().pDocData->isRunning())
        return;
    slg::RenderSession* session = Doc::instance().pDocData->getSession();

    if(session && session->film)
    {
        session->renderEngine->UpdateFilm();
	    session->film->UpdateScreenBuffer();
        if(m_contributeFilmSet.size())
        {
            boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
            boost::posix_time::time_duration diff = now - m_lastMerge_tick;
            if(diff.total_milliseconds() > opt_MinUpdateInterval)
            {
                slg::Film* totalfilm = getTotalFilmFromNative(session);
                if(totalfilm)
                {
                    totalfilm->Reset();
                    {
                        boost::unique_lock<boost::mutex> lock(session->filmMutex);
                        BOOST_FOREACH(ContributeFilm* ct,m_contributeFilmSet)
                        {
                            if(ct->getFilm())
                            {
                                totalfilm->AddFilm(*(ct->getFilm()));
                            }
                        }
                        totalfilm->AddFilm(*(session->film));
                    }
                    totalfilm->UpdateScreenBuffer();
                }
                //记录最新时间，而不是复用now.
                m_lastMerge_tick = boost::posix_time::microsec_clock::local_time();
            }
        }
    }
}

bool
CacheFilm::getContributeRenderInfo(RenderInfo &ri)
{
    if(m_contributeFilmSet.size())
    {
        BOOST_FOREACH(ContributeFilm* ct,m_contributeFilmSet)
        {
            //slg::Film   *film = ct->getFilm();
            //if(film)
            {
                ri.merge(ct->getRenderInfo());
            }
        }
        return true;
    }
    return false;
}


void
CacheFilm::invalidate(void)
{
    BOOST_FOREACH(ContributeFilm* ct,m_contributeFilmSet)
    {
        delete ct;
    }
    m_contributeFilmSet.clear();
    if(m_totalFilm)
    {
        delete m_totalFilm;
        m_totalFilm = NULL;
    }
}


const float*
CacheFilm::getPixels(void)
{
    if(m_totalFilm)
    {
        return m_totalFilm->GetScreenBuffer();
    }
    slg::RenderSession* session = Doc::instance().pDocData->getSession();
    if(session && session->film)
    {
        return session->film->GetScreenBuffer();
    }
    return NULL;
}



void
CacheFilm::saveNativeFilm(void)
{
    slg::RenderSession  *session = Doc::instance().pDocData->m_session.get();
    if(session)
    {
        session->renderEngine->UpdateFilm();
        session->film->UpdateScreenBuffer();

        PauseFilm   *pFilm = NULL;
        {
            boost::unique_lock<boost::mutex> lock(session->filmMutex);
            pFilm = new PauseFilm(session->film);
        }
        Doc::instance().pDocData->getNativeRenderInfo(pFilm->getRenderInfo());
        appendContribute(pFilm);
    }
}



}
