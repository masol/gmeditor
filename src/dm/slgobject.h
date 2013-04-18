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

#ifndef  GME_DM_SLGOBJECT_H
#define  GME_DM_SLGOBJECT_H

#include "dm/objectnode.h"
#include "utils/singleton.h"
#include <boost/unordered_map.hpp>

//forward declare.
namespace luxrays{
class ExtMesh;
}

namespace gme{

struct  ObjectWriteContext{
    ObjectWriteContext(bool bExportRes,const boost::filesystem::path& p,std::ostream &o)
        : m_dest_path(p),
          m_bSaveRes(bExportRes),
          m_stream(o)
    {}
    ~ObjectWriteContext(){}
protected:
    friend  class ExtraObjectManager;
    typedef   boost::unordered_map<std::string, std::string>    type_file_ctxid2savename;
    /** @brief 保存了文件内容md5字符串到本地文件名的映射。这可以判断是否是相同文件，以决定是否引用相同mesh.
    **/
    type_file_ctxid2savename  m_file_ctx2savename;
    const boost::filesystem::path m_dest_path;
    const bool      m_bSaveRes;
    std::ostream   &m_stream;
};


class ExtraObjectManager : public Singleton<ExtraObjectManager>
{
private:
    friend class Singleton<ExtraObjectManager>;
    typedef Singleton<ExtraObjectManager>   inherited;
    ExtraObjectManager(){}

    typedef   boost::unordered_map<boost::uuids::uuid, std::string>        type_id2name_map;
    /** @brief 保存了从object id到slg object name的map.
    **/
    type_id2name_map                        m_oid2slgname_map;

    /** @brief  保存了mesh组。根节点只作为容器节点存在，数据无效。
    **/
    ObjectNode                              m_objectGroup;
public:
    void    clear(){
        m_objectGroup.clear();
        m_oid2slgname_map.clear();
    }
    inline std::string     getNameForSlg(const boost::uuids::uuid &objid)
    {
        type_id2name_map::const_iterator it = m_oid2slgname_map.find(objid);
        if(it == m_oid2slgname_map.end())
        {
            return ObjectNode::idto_string(objid);
        }
        return it->second;
    }
    inline  ObjectNode&     getRoot(void){
        return m_objectGroup;
    }
    ///@brief 更新uuid到slgname的映射记录。
    inline  void    updateSlgMap(const boost::uuids::uuid &id,const std::string& slgname){
        m_oid2slgname_map[id] = slgname;
    }
    inline  void    write(ObjectWriteContext &ctx){
        ObjectNode::type_child_container::iterator  it = this->m_objectGroup.begin();
        while(it != this->m_objectGroup.end())
        {
            this->write(*it,ctx);
            it++;
        }
    }
    luxrays::ExtMesh*   getExtMesh(const boost::uuids::uuid &objid);
    void    write(ObjectNode &pThis,ObjectWriteContext& ctx);
};

}

#endif //GME_DM_SLGOBJECT_H
