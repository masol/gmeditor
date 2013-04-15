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
#include "dm/objectnode.h"
#include "docprivate.h"
#include "slg/slg.h"


namespace gme{

std::string
ObjectNode::idto_string(const boost::uuids::uuid &u)
{
    std::string result;
    result.reserve(32);

    for (boost::uuids::uuid::const_iterator it_data = u.begin(); it_data!=u.end(); ++it_data) {
        const size_t hi = ((*it_data) >> 4) & 0x0F;
        result += boost::uuids::detail::to_char(hi);

        const size_t lo = (*it_data) & 0x0F;
        result += boost::uuids::detail::to_char(lo);
    }
    return result;
}

luxrays::ExtMesh*
ObjectNode::getExtMesh(void)const
{
    std::string    name = Doc::instance().pDocData->getObjectName(this->m_id);
    slg::RenderSession* session = Doc::instance().pDocData->getSession();
    if(session && session->renderConfig->scene)
    {
        return session->renderConfig->scene->meshDefs.GetExtMesh(name);
    }
    return NULL;
}


void
ObjectNode::write(std::ofstream &o,ObjectWriteContext& ctx)
{
    o  << "<object id='" << idto_string(this->m_id)
        << "' name='" << this->m_name << "'";

    luxrays::ExtMesh*   extMesh = getExtMesh();
    if(extMesh){
        o << " material='" << idto_string(this->m_matid)
            << "' useplynormals='" << (m_useplynormals ? "true" : "false")
            << "'";

        std::string     write_file;
        boost::filesystem::path target_model = ctx.m_dest_path / "mesh%%%%%%.ply";
        if(this->m_filepath.length())
        {//获取映射的文件名。
            if(ctx.m_bSaveRes)
            {//拷贝资源。
                ObjectWriteContext::type_filepath2savepath::iterator it = ctx.m_filepath2savepath.find(this->m_filepath);
                if(it == ctx.m_filepath2savepath.end())
                {//文件没有映射。
                    boost::filesystem::path target = boost::filesystem::unique_path(target_model);
                    extMesh->WritePly(target.string());
                    write_file = target.filename().string();
                    ctx.m_filepath2savepath[this->m_filepath] = write_file;
                }else{//不拷贝资源，文件已经映射，直接取出second。
                    write_file = it->second;
                }
            }else{//不拷贝资源，直接保存m_filepath.
                write_file = this->m_filepath;
            }
        }else{//没有定义文件名。此时直接导出文件。
            boost::filesystem::path target = boost::filesystem::unique_path(target_model);
            extMesh->WritePly(target.string());
            write_file = target.filename().string();
        }
        o << " file='" << write_file << "'";
    }
	o << ">" << std::endl;
    type_child_container::iterator  it = begin();
    while(it != end())
    {
        it->write(o,ctx);
        it++;
    }
    o << "</object>" << std::endl;
}


}
