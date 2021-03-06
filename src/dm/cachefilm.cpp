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
#include <boost/filesystem/fstream.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
//#include <boost/iostreams/filtering_streambuf.hpp>
//#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/scope_exit.hpp>
#include "cachefilm.h"
#include "docprivate.h"
#include "dm/docimg.h"
#include "archivefilm.h"


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

class FileFilm : public ContributeFilm
{
public:
    FileFilm(slg::Film *film,const std::string &localFile) : ContributeFilm(CT_FILE)
    {
        m_film = new slg::Film(film->GetWidth(),film->GetHeight());
        m_film->CopyDynamicSettings(*film);
        m_film->Init();
        boost::filesystem::ifstream in(localFile,std::ios::in | std::ios::binary);
        if(in)
        {
            BOOST_SCOPE_EXIT( (&in) )
            {
                in.close();
            }
            BOOST_SCOPE_EXIT_END

            CacheFilm::loadFromStream(in,m_film,m_renderInfo);
        }
    }
};


static const  boost::uint_least32_t   FilmStreamMagic = 0x42a4bdf7;
void   
CacheFilm::saveToStream(std::ostream &out,const slg::Film* pFilm,const RenderInfo &ri)
{
    //boost::iostreams::filtering_streambuf<boost::iostreams::output> filter_out;
    //filter_out.push(boost::iostreams::bzip2_compressor());
    //filter_out.push(out);
    //boost::archive::binary_oarchive oa(filter_out,boost::archive::no_header);

    boost::archive::binary_oarchive oa(out,boost::archive::no_header);
    boost::uint_least32_t	version = 1;
    oa << FilmStreamMagic;
    oa << version;
    boost::serialization::save(oa,ri,version);
    boost::serialization::save(oa,(*pFilm),version);
}

void
CacheFilm::loadFromStream(std::istream &in,slg::Film* pFilm,RenderInfo &ri)
{
    //boost::iostreams::filtering_streambuf<boost::iostreams::input> filter_in;
    //filter_in.push(boost::iostreams::bzip2_decompressor());
    //filter_in.push(in);
    //boost::archive::binary_iarchive ia(filter_in,boost::archive::no_header);

    boost::archive::binary_iarchive ia(in,boost::archive::no_header);
    boost::uint_least32_t	magid,version;
    ia >> magid;
    if(magid != FilmStreamMagic)
        throw std::runtime_error("invalid format");
    ia >> version;
    boost::serialization::load(ia,ri,version);
    boost::serialization::load(ia,(*pFilm),version);
}


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

void
CacheFilm::loadFilm(slg::Film *psysfilm,const std::string &filmfile)
{
    this->appendContribute(new FileFilm(psysfilm,filmfile));
}

bool
CacheFilm::saveFilm(const std::string &filmfile)
{
    //m_lastMerge_tick  = 0;
    updateNativeFilm();

    slg::Film   *pCurrentFilm = m_totalFilm;
    if(!pCurrentFilm)
    {
        pCurrentFilm = Doc::instance().pDocData->getSession()->film;
    }

    RenderInfo  ri;
    if(pCurrentFilm && Doc::instance().pDocData->getNativeRenderInfo(ri) )
    {
        boost::filesystem::ofstream out(filmfile,std::ios::out | std::ios::binary | std::ios::trunc);
        if(out)
        {
            BOOST_SCOPE_EXIT( (&out) )
            {
                out.close();
            }
            BOOST_SCOPE_EXIT_END

            getContributeRenderInfo(ri);

            CacheFilm::saveToStream(out,pCurrentFilm,ri);

            return true;
        }
    }
    return false;
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
            if(ct->getType() == ContributeFilm::CT_PAUSE || ct->getType() == ContributeFilm::CT_FILE)
            {
                ri.avgMerge(ct->getRenderInfo());
            }else{
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
