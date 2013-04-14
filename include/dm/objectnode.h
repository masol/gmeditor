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
#include <string>
#include <vector>
#include "luxrays/core/geometry/matrix4x4.h"

namespace gme{

class ObjectNode
{
public:
    boost::uuids::uuid      m_id;
    std::string             m_name;
    std::string             m_mesh_name;
    /** @brief 模型文件。m_mesh_name从模型文件中索引mesh.
    **/
    std::string             m_filepath;
    /** @brief 这里以string类型保存material id.如果从cfg打开，则matID不能转化为uuid.在保存时自动更新。
    **/
    boost::uuids::uuid      m_matid;
    std::vector<ObjectNode> m_children;
    bool                    m_useplynormals;
    luxrays::Matrix4x4      m_transformation;
    inline void    assignFrom(const ObjectNode& ref)
    {
        m_id = ref.m_id;
        m_name = ref.m_name;
        m_mesh_name = ref.m_mesh_name;
        m_filepath = ref.m_filepath;
        m_matid = ref.m_matid;
        m_children = ref.m_children;
        m_useplynormals = ref.m_useplynormals;
        m_transformation = ref.m_transformation;
    }
public:
    static  std::string     idto_string(const boost::uuids::uuid &id);
    ObjectNode()
    {
        m_useplynormals = false;
    }
    ObjectNode(const ObjectNode& ref){
        assignFrom(ref);
    }
    ~ObjectNode(){}
    ObjectNode& operator=(const ObjectNode& ref){
        assignFrom(ref);
        return *this;
    }
};

}

#endif //GME_DM_MESHNODE_H
