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

#ifndef  GME_DM_DOCPRIVATE_H
#define  GME_DM_DOCPRIVATE_H

#include "slg/rendersession.h"
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include "slgobjectnode.h"

namespace gme{

class DocPrivate
{
private:
    friend  class Doc;
    DocPrivate(void);
    ~DocPrivate(void);
public:
    //@FIXME: 这里不能直接暴露slg.需要使用DocImpl类来封装，以方便支持cycles,luxrender...
    //@TODO: 需要一个材质转化专家系统来支持材质转化。
	//
    boost::shared_ptr<slg::RenderSession>   m_session;
    /** @fixme: slg的started为保护成员。
    **/
    bool                                    m_started;
    /** @brief 保存了material的名称映射。如果键值不存在，则直接使用原始名称作为名称返回。
    **/
    typedef   boost::unordered_map<boost::uuids::uuid, std::string>        type_id2name_map;
    type_id2name_map                        m_material_map;
    type_id2name_map                        m_object_map;
    
    /** @brief  保存了mesh组。根节点只作为容器节点存在，数据无效。
    **/
    ObjectNode                              m_objectGroup;
public:
    inline std::string     getObjectNameInSlg(const boost::uuids::uuid &objid)
    {
        return getNameInSlgFromID(objid,m_object_map);
    }
    inline std::string     getMaterialNameInSlg(const boost::uuids::uuid &matid)
    {
        return getNameInSlgFromID(matid,m_material_map);
    }

    inline slg::RenderSession*  getSession(void){
        return m_session.get();
    }
protected:
    inline std::string     getNameInSlgFromID(const boost::uuids::uuid &id,const type_id2name_map &mapper)
    {
        type_id2name_map::const_iterator it = mapper.find(id);
        if(it == mapper.end())
        {
            return ObjectNode::idto_string(id);
        }
        return it->second;
    }
};

}



#endif  //GME_DM_DOC_H
