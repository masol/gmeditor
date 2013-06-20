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

#ifndef  GME_DM_IMPORTCTX_H
#define  GME_DM_IMPORTCTX_H


#include "slg/slg.h"
#include "slg/sdl/scene.h"
#include "slg/editaction.h"
#include "utils/i18n.h"
#include "dm/doc.h"
#include <boost/format.hpp>
#include <assimp/scene.h>           // Output data structure

namespace gme{

/** @brief Implements the import context.
 * @details ImportContext isolation new and import
**/
class ImportContext
{
    friend class SwitchAssimpData;
private:
    slg::Scene      *m_scene;
    int             m_editAction;
    std::string     m_docBasePath;
    const aiScene*  m_assimpScene;
    bool            m_loadFilm;
    //DocIO将读取下面的参数。
    friend class DocIO;
    //ExtraSettingManager将设置下面参数。
    friend class ExtraSettingManager;
    //下面的参数是系统设置参数。从配置文件中读取之后，等待后续使用。
    slg::ToneMapParams  *m_pTonemapParams;
    std::string          m_film_filter_type;
    std::string          m_width;
    std::string          m_height;
    std::string          m_film_gamma;
    std::string          m_renderengine_type;
    std::string          m_sampler_type;
    std::string          m_PathDepth;
    std::string          m_RouletteDepth;
    std::string          m_RouletteCap;
    std::string          m_largesteprate;
    std::string          m_imagemutationrate;
    std::string          m_maxconsecutivereject;
    std::string          m_pathFilter_type;
    std::string          m_pathFilter_widthx;
    std::string          m_pathFilter_widthy;
    std::string          m_pathFilter_alpha;
    std::string          m_pathFilter_b;
    std::string          m_pathFilter_c;
public:
    inline const aiScene*  getAiScene(void)const
    {
        return m_assimpScene;
    }
    ///@brief this method implement in docprivate.cpp
    std::string  findFile(const std::string &srcpath);
    inline const std::string&  docBasepath(void)const
    {
        return m_docBasePath;
    }
    inline bool loadFilm(void)const
    {
        return m_loadFilm;
    }
    inline void loadFilm(bool lf)
    {
        m_loadFilm = lf;
    }
    ImportContext(slg::Scene* s,const std::string &srcFile)
    {
        m_pTonemapParams = NULL;

        m_scene = s;
        m_editAction = 0;
        m_assimpScene = NULL;
        m_loadFilm = false;
        m_docBasePath = boost::filesystem::canonical(srcFile).parent_path().string();
    }
    ~ImportContext(void)
    {
        if(m_pTonemapParams)
            delete m_pTonemapParams;
    }
    inline void addAction(const slg::EditAction a)
    {
        m_editAction |= a;
    }
    inline slg::EditAction getAction()const
    {
        return (slg::EditAction)m_editAction;
    }
    inline slg::Scene*  scene()const{
        return m_scene;
    }
};

class SwitchAssimpData{
private:
    ImportContext   &m_ctx;
    const aiScene   *m_oldScene;
public:
    SwitchAssimpData(ImportContext &ctx) : m_ctx(ctx)
    {
        m_oldScene = m_ctx.m_assimpScene;
    }
    ~SwitchAssimpData(void)
    {
        m_ctx.m_assimpScene = m_oldScene;
    }
    inline void setAiScene(const aiScene* scene)
    {
        m_ctx.m_assimpScene = scene;
    }
};

}

#endif //GME_DM_IMPORTCTX_H

