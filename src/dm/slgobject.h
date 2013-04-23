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
#include "utils/pathext.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/unordered_map.hpp>
#include "luxrays/utils/properties.h"
#include "slg/slg.h"
#include "slg/editaction.h"
#include "slg/rendersession.h"
#include "slgutils.h"

//forward declare.
namespace luxrays{
class ExtMesh;
}

namespace gme{

struct  ObjectWriteContext{
    ObjectWriteContext(bool bExportRes,const boost::filesystem::path& p,std::ostream &o,int indent = 1)
        : m_dest_path(p),
          m_bSaveRes(bExportRes),
          m_stream(o),
          m_indent(indent)
    {}
    ~ObjectWriteContext(){}
    inline  const std::vector<std::string>&  refMaterials()const
    {
        return m_refMaterials;
    }
protected:
	inline	void outIndent(void)
	{
		for(int indent = 0; indent < this->m_indent; indent++)
			m_stream << "  ";
	}
    friend  class ExtraObjectManager;
    typedef   boost::unordered_map<std::string, std::string>    type_file_ctxid2savename;
    /** @brief 保存了文件内容md5字符串到本地文件名的映射。这可以判断是否是相同文件，以决定是否引用相同mesh.
    **/
    type_file_ctxid2savename  m_file_ctx2savename;
    const boost::filesystem::path &m_dest_path;
    const bool      m_bSaveRes;
    std::ostream   &m_stream;
    int             m_indent;
    /** 每个被引用的material被保存在这里。
    **/
    std::vector<std::string>     m_refMaterials;
};


class ExtraObjectManager
{
private:
    friend class DocPrivate;
    ExtraObjectManager(){}
    ~ExtraObjectManager(){}

    /** @brief  保存了mesh组。根节点只作为容器节点存在，数据无效。
    **/
    ObjectNode                              m_objectGroup;
public:
    inline  ObjectNode*     findObject(const std::string &id)
    {
        return m_objectGroup.findObject(id);
    }
    void    clear(){
        m_objectGroup.clear();
    }
    inline  ObjectNode&     getRoot(void){
        return m_objectGroup;
    }

/*
    inline  void    write(ObjectWriteContext &ctx){
        ObjectNode::type_child_container::iterator  it = this->m_objectGroup.begin();
        while(it != this->m_objectGroup.end())
        {
            this->write(*it,ctx);
            it++;
        }
    }
    void    write(ObjectNode &pThis,ObjectWriteContext& ctx);
*/
    static  luxrays::ExtMesh*   getExtMesh(const std::string &objid);
    void    loadExtraFromProps(luxrays::Properties &props)
    {//不加载根节点:
         ObjectNode::type_child_container::iterator it = m_objectGroup.begin();
         while(it != m_objectGroup.end())
         {
            loadExtraFromProps(*it,props);
            it++;
         }
    }
public:
    /** @brief 删除指定模型.
    **/
    bool    removeMesh(const std::string &id);
    /** @brief 从模型文件中加载对象组。
    **/
    bool    loadObjectsFromFile(const std::string &file,ObjectNode *pParent,SlgUtil::Editor &editor);
private:
    ///@brief 从props中加载原始文件信息。
    void    loadExtraFromProps(ObjectNode& node,luxrays::Properties &props);
    /**@brief 清理extMeshCache信息。
      *@details 本函数并不删除pMesh.
      *@return 返回pMesh的引用计数。(0通常说明pMesh自身就是一个mesh而不是object)
    **/
    static  int    deleteFromExtMeshCache(luxrays::ExtMeshCache &extMeshCache,luxrays::ExtMesh *pObject);
    ///@brief 检查有多少个对象引用了指定的造型对象。
    static  int     getReferenceCount(luxrays::ExtMeshCache &extMeshCache,luxrays::ExtTriangleMesh *pGeometry);
};

}

#endif //GME_DM_SLGOBJECT_H
