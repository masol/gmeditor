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

#ifndef  GME_DM_SLGOBJECTNODE_H
#define  GME_DM_SLGOBJECTNODE_H

#include "dm/objectnode.h"

//forward declare.
namespace luxrays{
class ExtMesh;
}

namespace gme{

struct  ObjectWriteContext{
    ObjectWriteContext(bool bExportRes,const boost::filesystem::path& p)
        : m_dest_path(p),
          m_bSaveRes(bExportRes)
    {}
    ~ObjectWriteContext(){}
protected:
    friend  class SlgObjectNode;
    typedef   boost::unordered_map<std::string, std::string>    type_file_ctxid2savename;
    /** @brief 保存了文件内容md5字符串到本地文件名的映射。这可以判断是否是相同文件，以决定是否引用相同mesh.
    **/
    type_file_ctxid2savename  m_file_ctx2savename;
    const boost::filesystem::path m_dest_path;
    const bool   m_bSaveRes;
};


class SlgObjectNode : public ObjectNode
{
private:
    /// @brief 不允许创建。
    SlgObjectNode(){}
public:
    static  void    write(ObjectNode &pThis,std::ofstream &o,ObjectWriteContext& ctx);
    static  luxrays::ExtMesh*   getExtMesh(ObjectNode& pThis);
};

}

#endif //GME_DM_SLGOBJECTNODE_H
