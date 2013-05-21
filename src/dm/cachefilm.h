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

#ifndef  GME_DM_CACHEFILM_H
#define  GME_DM_CACHEFILM_H

#include "slg/slg.h"
#include "slg/film/tonemapping.h"
#include "slg/film/film.h"
#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include "dm/doc.h"

namespace gme{

struct  RenderInfo;

class NoneToneMapParams : public slg::ToneMapParams
{
public:
    NoneToneMapParams()
    {
    }
    slg::ToneMapType GetType() const { return slg::TONEMAP_NONE; }
    slg::ToneMapParams *Copy() const {
        return new NoneToneMapParams();
    }
};

class ContributeFilm
{
public:
    enum{
        CT_INVALID,
        CT_PAUSE,
        CT_FILE,
        CT_LAN,
        CT_INTERNET,
        CT_MAX
    };
protected:
    slg::Film           *m_film;
    const int           m_type;
    RenderInfo          m_renderInfo;
    ContributeFilm(int t) : m_film(NULL),m_type(t)
    {
    }
public:
    int    getType(void)const
    {
        return m_type;
    }
    inline RenderInfo&  getRenderInfo()
    {
        return m_renderInfo;
    }
    bool saveTotalFilm(std::ostream &out);
    virtual ~ContributeFilm(void)
    {
        if(m_film)
        {
            delete m_film;
            m_film = NULL;
        }
    }
    inline slg::Film*      getFilm(void)
    {
        return m_film;
    }
};

/** @brief cache for all film. this is useful for pause and networking rendering
**/
class CacheFilm
{
private:
    ///@brief   贡献的film.
    std::vector<ContributeFilm*>        m_contributeFilmSet;
    ///@brief   综合所有film的film.
    slg::Film                           *m_totalFilm;
    ///@brief   当有contribute时，最小合并时间。
    long                                opt_MinUpdateInterval;
    ///@brief   上一次合并contribute的时间。
    boost::posix_time::ptime            m_lastMerge_tick;
    ///@brief 以native为模版，构建totalFilm.
    slg::Film*  getTotalFilmFromNative(slg::RenderSession  *session);
public:
    inline  void    appendContribute(ContributeFilm* cf)
    {
        m_contributeFilmSet.push_back(cf);
    }
    bool    getContributeRenderInfo(RenderInfo &ri);
    /** @brief restore a film from file.
    **/
    static  slg::Film*      restoreFromFile(const std::string &filename);
    /** @brief save film to file.
    **/
    static  void            saveToFile(slg::Film *pFilm,const std::string &filename);
public:
    CacheFilm(void);
    ~CacheFilm(void);
    /**@brief save native film to cachefilm.
    **/
    void        saveNativeFilm(void);
    /** @brief merge film.
    **/
    void        mergeFilm(slg::Film *pfilm);

    ///@biref throw exception on error.
    void        loadFilm(slg::Film *psysfilm,const std::string &filmfile);
    bool        saveFilm(const std::string &filmfile);

    /** @brief update local film.
    **/
    void        updateNativeFilm(void);

    /** @brief get data (no update).
    **/
    const float*  getPixels(void);

    /** @brief save image to filename.
    **/
    bool        saveImage(const std::string &fullpath);
    /** @brief invalidate cache, this is caused by scene editting commonly.
    **/
    void   invalidate(void);
};

} //end namespace gme

#endif //CACHEFILM
