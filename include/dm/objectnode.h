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

#ifndef  GME_DM_MESHNODE_H
#define  GME_DM_MESHNODE_H

#include <boost/unordered_map.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <Eigen/Core>
#include <fstream>
#include "dm/xmlutil.h"

namespace gme{

/** @brief 保存了对象的额外信息。
**/
class ObjectNode
{
public:
    typedef std::vector<ObjectNode*>        type_path;
private:
    friend class DocIO;
    friend class ExtraObjectManager;
protected:
    //以字符串形式保存了模型的id.这个id直接作为slgname使用。
    std::string             m_id;
    //扩展保存了name。这允许我们自定义重复的名称作为模型名称。
    std::string             m_name;
    /** @brief 模型文件。m_filepath保存了本mesh的原始文件。
      * @details 这个原始文件可能保存了若干模型.将会被展开为子模型节点。
    **/
    std::string             m_filepath;
    /** @brief 这里以string类型保存material id.matID就是slgname.
    **/
    std::string             m_matid;
    std::vector<ObjectNode> m_children;
    bool                    m_useplynormals;
    /** @brief m_transformation is difference from slg. this is a hierarchy.
    **/
    Eigen::Matrix4f         m_transformation;
    inline void    assignFrom(const ObjectNode& ref)
    {
        m_id = ref.m_id;
        m_name = ref.m_name;
        m_filepath = ref.m_filepath;
        m_matid = ref.m_matid;
        m_children = ref.m_children;
        m_useplynormals = ref.m_useplynormals;
        m_transformation = ref.m_transformation;
    }
public:
    /** @brief 将当前对象的内容dump到给定的xml node.
      * @details 如果使用相对路径或者拷贝资源或者导出ctm,都是相对于当前路径展开的计算，因此调用之前需要设置当前路径为资源目标路径。
      * @return self node.
    **/
    type_xml_node*   dump(type_xml_node &parent,dumpContext &ctx);

    /** @brief 搜索值为id的节点对象。
     *  @param parent 如果给出此参数，将把父对象写入这里。
    **/
    ObjectNode*     findObject(const std::string &id,type_path *pPath=NULL);
    inline void addChild(const ObjectNode&  child)
    {
        m_children.push_back(child);
    }
    inline  bool    removeChild(const std::string &id)
    {
        std::vector<ObjectNode>::iterator it = m_children.begin();
        while(it != m_children.end())
        {
            if(it->m_id == id)
            {
                m_children.erase(it);
                return true;
            }else if(it->removeChild(id))
            {
                return true;
            }
            it++;
        }
        return false;
    }
    inline void clear(){
        m_children.clear();
    }
    typedef std::vector<ObjectNode> type_child_container;
    inline type_child_container::iterator  begin(){
        return m_children.begin();
    }
    inline type_child_container::iterator  end(){
        return m_children.end();
    }
    inline type_child_container::const_iterator  begin()const{
        return m_children.begin();
    }
    inline type_child_container::const_iterator  end()const{
        return m_children.end();
    }
    inline  const std::string&     id(void)const{
        return m_id;
    }
    inline  const std::string&     matid(void)const{
        return m_matid;
    }
	inline	const std::string&		name(void)const{
		return m_name;
	}
	inline  void    name(const std::string &n){
	    m_name = n;
	}
	inline  const std::string&  filepath(void){
	    return m_filepath;
    }
	inline  bool    useplynormals()const{
	    return m_useplynormals;
    }
    ObjectNode()
    {
        m_useplynormals = false;
        //创建者需要自行维护nil.
//        m_id = boost::uuids::nil_generator()();
//        m_matid = boost::uuids::nil_generator()();
    }
    ObjectNode(const ObjectNode& ref){
        assignFrom(ref);
    }
    ~ObjectNode()
    {
        clear();
    }
    ObjectNode& operator=(const ObjectNode& ref){
        assignFrom(ref);
        return *this;
    }
};

}

#endif //GME_DM_MESHNODE_H
