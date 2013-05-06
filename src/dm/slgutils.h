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

#ifndef  GME_DM_SLGUTILS_H
#define  GME_DM_SLGUTILS_H

#include "slg/rendersession.h"
#include "dm/xmlutil.h"
#include <boost/foreach.hpp>

namespace gme{

class SlgUtil
{
public:
    struct   Editor{
    protected:
        slg::RenderSession      *m_session;
        bool                    m_bNeedRefresh;
    public:
        inline Editor(slg::RenderSession *session)
        {
            m_session = session;
            m_session->BeginEdit();
            m_bNeedRefresh = false;
        }
        inline void  needRefresh(bool v){
            m_bNeedRefresh = v;
        }
        inline void  resetAction(void){
            m_session->editActions.Reset();
        }
        inline ~Editor()
        {
            if (m_session->editActions.Has(slg::MATERIALS_EDIT)) {
                m_session->renderConfig->scene->RemoveUnusedMaterials();
                m_session->renderConfig->scene->RemoveUnusedTextures();
            }
            if(m_bNeedRefresh)// || m_session->editActions.Has(slg::MATERIAL_TYPES_EDIT))
            {
                m_session->editActions.Reset();
                m_session->EndEdit();
                m_session->Stop();
                m_session->Start();
            }else{
                m_session->EndEdit();
            }
        }
        inline void addAction(const slg::EditAction a)
        {
            m_session->editActions.AddAction(a);
        }
        inline slg::Scene* scene()
        {
            return m_session->renderConfig->scene;
        }
    };

    struct   UpdateContext{
        SlgUtil::Editor         &editor;
        const std::string       &value;
        const std::vector<std::string>      &keyPath;
        boost::function<bool (std::string &)>   getImageFilepath;
        luxrays::Properties     props;
        std::string             updatedId;
        bool                    idIsMat;
        bool                    bGenNode;   //指示是否为parent添加了子节点。
        bool                    bVeto;      //指示是否已经放弃了本次修改。
        UpdateContext(SlgUtil::Editor &e,type_xml_node &p,const std::string &v,const std::vector<std::string> &k,boost::function<bool (std::string &)> &f) :
                editor(e), value(v) , keyPath(k),getImageFilepath(f)
        {
            bGenNode  = false;
            idIsMat   = false;
            bVeto = false;
        }
    };

    static  inline  std::string propReplaceNewId(luxrays::Properties &oldProp,luxrays::Properties &newProp,const std::string &oldId,const std::string &prefix)
    {
        std::string oldPrefix = prefix + oldId;
        std::string newId = string::uuid_to_string(boost::uuids::random_generator()());
        std::string newPrefix = prefix + newId;
        const std::vector< std::string >  Keys = oldProp.GetAllKeys(oldPrefix);
        BOOST_FOREACH(const std::string &key,Keys)
        {
            std::string value = oldProp.GetString(key,"");
            std::string subkey = luxrays::Properties::ExtractField(key,3);
            oldProp.Delete(key);
            newProp.SetString(newPrefix + '.' + subkey,value);
        }
        return newId;
    }


    static  inline  void    OutputSlgmat(std::ostream &o,const luxrays::Matrix4x4 &luxmat)
    {
        for(int col = 0; col < 4; col++)
        {
            for(int row = 0; row < 4; row++)
            {
                if(!col && !row)
                    o << ' ';
                o << boost::lexical_cast<std::string>(luxmat.m[row][col]);
            }
            if(col != 3)
                o << ' ';
        }
    }
};

}

#endif //GME_DM_SLGUTILS_H

