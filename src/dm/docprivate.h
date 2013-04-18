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

/** @brief 本类保存了额外的材质信息。可以看作对slg material基类的信息添加。
**/
struct  ExtraMaterialInfo{
public:
    std::string     m_slgname;
    std::string     m_name;
    ExtraMaterialInfo(){
    }
    ExtraMaterialInfo(const ExtraMaterialInfo& ref){
        AssignFrom(ref);
    }
    ~ExtraMaterialInfo(){
    }
    ExtraMaterialInfo&  operator=(const ExtraMaterialInfo& ref){
        AssignFrom(ref);
        return *this;
    }
private:
    void    AssignFrom(const ExtraMaterialInfo& ref)
    {
        this->m_slgname = ref.m_slgname;
        this->m_name = ref.m_name;
    }
};

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
    /** @brief 保存了从object id到slg object name的map.
    **/
    type_id2name_map                        m_oid2slgname_map;

    /** @brief  保存了mesh组。根节点只作为容器节点存在，数据无效。
    **/
    ObjectNode                              m_objectGroup;

    typedef  boost::unordered_map<boost::uuids::uuid, ExtraMaterialInfo>        type_id2material_map;
    /** @brief 保存了从material id 到material_map.
    **/
    type_id2material_map                    m_id2material_map;
    
public:
    void    WriteMaterial(std::ofstream &o);
    inline std::string     getObjectNameInSlg(const boost::uuids::uuid &objid)
    {
        type_id2name_map::const_iterator it = m_oid2slgname_map.find(objid);
        if(it == m_oid2slgname_map.end())
        {
            return ObjectNode::idto_string(objid);
        }
        return it->second;
    }
    
    
    /** @brief 本函数如果没有找到对应记录，会自动添加一个materialInfo记录。
    **/
    inline ExtraMaterialInfo&   getMaterialInfo(const boost::uuids::uuid &matid)
    {
        return m_id2material_map[matid];
    }
    
    /** @brief 本函数返回matid对应的materialInfo记录，如果没有，则返回空。
    **/
    inline ExtraMaterialInfo*   queryMaterialInfo(const boost::uuids::uuid &matid)
    {
        type_id2material_map::iterator it = m_id2material_map.find(matid);
        if(it != m_id2material_map.end())
        {
            return &(it->second);
        }
        return NULL;
    }

    inline slg::RenderSession*  getSession(void){
        return m_session.get();
    }
protected:
    void    WriteMaterialImpl(std::ofstream &o,const std::string &slgname,type_id2material_map::iterator &it);

};

}



#endif  //GME_DM_DOC_H
