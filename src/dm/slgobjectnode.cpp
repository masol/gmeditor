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

#include "config.h"
#include "dm/doc.h"
#include "slgobjectnode.h"
#include "docprivate.h"
#include "slg/slg.h"


namespace gme{

luxrays::ExtMesh*
SlgObjectNode::getExtMesh(ObjectNode &pThis)
{
    std::string    name = Doc::instance().pDocData->getObjectNameInSlg(pThis.m_id);
    slg::RenderSession* session = Doc::instance().pDocData->getSession();
    if(session && session->renderConfig->scene)
    {
        return session->renderConfig->scene->meshDefs.GetExtMesh(name);
    }
    return NULL;
}


//@TODO 添加基于文件内容md5码的meshid属性。用于检查mesh一致性。
void
SlgObjectNode::write(ObjectNode &pThis,std::ofstream &o,ObjectWriteContext& ctx)
{
    o  << "<object id='" << ObjectNode::idto_string(pThis.m_id)
        << "' name='" << pThis.m_name << "'";

    luxrays::ExtMesh*   extMesh = getExtMesh(pThis);
    if(extMesh){
        o << " material='" << idto_string(pThis.m_matid)
            << "' useplynormals='" << (pThis.m_useplynormals ? "true" : "false")
            << "'";

        std::string     write_file;
        boost::filesystem::path target_model = ctx.m_dest_path / "mesh%%%%%%.ply";
        if(pThis.m_filepath.length())
        {//获取映射的文件名。
            if(ctx.m_bSaveRes)
            {//拷贝资源。
                ObjectWriteContext::type_filepath2savepath::iterator it = ctx.m_filepath2savepath.find(pThis.m_filepath);
                if(it == ctx.m_filepath2savepath.end())
                {//文件没有映射。
                    boost::filesystem::path target = boost::filesystem::unique_path(target_model);
                    extMesh->WritePly(target.string());
                    write_file = target.filename().string();
                    ctx.m_filepath2savepath[pThis.m_filepath] = write_file;
                }else{//不拷贝资源，文件已经映射，直接取出second。
                    write_file = it->second;
                }
            }else{//不拷贝资源，直接保存m_filepath.
                write_file = pThis.m_filepath;
            }
        }else{//没有定义文件名。此时直接导出文件。
            boost::filesystem::path target = boost::filesystem::unique_path(target_model);
            extMesh->WritePly(target.string());
            write_file = target.filename().string();
        }
        o << " file='" << write_file << "'";
    }
	o << ">" << std::endl;
    type_child_container::iterator  it = pThis.begin();
    while(it != pThis.end())
    {
        write((*it),o,ctx);
        it++;
    }
    o << "</object>" << std::endl;
}


}
