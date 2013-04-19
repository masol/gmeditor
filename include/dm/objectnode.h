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

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/unordered_map.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <Eigen/Core>
#include <fstream>

namespace gme{

/** @brief 保存了对象的额外信息。
**/
class ObjectNode
{
private:
    friend class DocIO;
    friend class ExtraObjectManager;
protected:
    boost::uuids::uuid      m_id;
    std::string             m_name;
    /** @brief 模型文件。m_filepath保存了本mesh的原始文件。
      * @details 这个原始文件可能保存了若干模型.将会被展开为子模型节点。
    **/
    std::string             m_filepath;
    /** @brief 这里以string类型保存material id.如果从cfg打开，则matID不能转化为uuid.在保存时自动更新。
    **/
    boost::uuids::uuid      m_matid;
    std::vector<ObjectNode> m_children;
    bool                    m_useplynormals;
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
    inline void addChild(const ObjectNode&  child)
    {
        m_children.push_back(child);
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
    inline  const boost::uuids::uuid&     id(void)const{
        return m_id;
    }
    inline  const boost::uuids::uuid&     matid(void)const{
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
    static  std::string     idto_string(const boost::uuids::uuid &id);
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
