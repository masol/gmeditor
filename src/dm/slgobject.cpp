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
#include "slgobject.h"
#include "docprivate.h"
#include "slg/slg.h"
#include "luxrays/luxrays.h"
#include "luxrays/core/exttrianglemesh.h"
#include "openctm/openctm.h"
#include "utils/MD5.h"
#include <boost/filesystem.hpp>


namespace gme{

template<class T>
bool    SaveCtmFile(bool useplynormals,T *pMesh,const std::string &filename,std::string &ctxHashValue)
{
    CTMcontext context = NULL;
    CTMuint    vertCount, triCount, * indices;
    CTMfloat   *vertices;
    CTMfloat   *aNormals = NULL;
    CTMfloat   *aUVCoords = NULL;
    
    vertCount = pMesh->GetTotalVertexCount ();
    triCount = pMesh->GetTotalTriangleCount ();
    vertices  = (CTMfloat*)(void*)pMesh->GetVertices();
    indices  = (CTMuint*)(void*)pMesh->GetTriangles();
    
    MD5     md5;
    
    md5.update((const unsigned char *)(void*)vertices,vertCount * 3 * sizeof(CTMfloat));
    md5.update((const unsigned char *)(void*)indices,triCount * 3 * sizeof(CTMuint));
    
    if(useplynormals) //pMesh->HasNormals())
    {
        std::cerr << "has normals ... " << std::endl;
        aNormals = new CTMfloat[vertCount * 3];
        for(unsigned int idx = 0 ; idx < vertCount; idx++)
        {
            luxrays::Normal n = pMesh->GetShadeNormal(idx);
            aNormals[idx * 3] = n.x;
            aNormals[idx * 3 + 1] = n.y;
            aNormals[idx * 3 + 2] = n.z;
        }
        md5.update((const unsigned char *)(void*)aNormals,vertCount * 3 * sizeof(CTMfloat));
    }
    
    context = ctmNewContext(CTM_EXPORT);
    ctmDefineMesh(context, vertices, vertCount, indices, triCount, aNormals);
    if(pMesh->HasUVs())
    {
        aUVCoords = new CTMfloat[vertCount * 2];
        for(unsigned int idx = 0 ; idx < vertCount; idx++)
        {
            luxrays::UV uv = pMesh->GetUV(idx);
            aUVCoords[idx * 2] = uv.u;
            aUVCoords[idx * 2 + 1] = uv.v;
        }
        ctmAddUVMap(context,aUVCoords,"def",NULL);
    }
    
    ctmSave(context, filename.c_str());
    
    ctxHashValue = md5.finalize().hexdigest();
    
    if(aNormals)
        delete[] aNormals;
    if(aUVCoords)
        delete[] aUVCoords;
    if(context)
        ctmFreeContext(context);
    
    
    return true;
}

static
bool    SaveCtmFile(bool useplynormals,luxrays::ExtMesh* extMesh,const std::string &filename,std::string &ctxHashValue)
{
    luxrays::ExtInstanceTriangleMesh*   pMesh = dynamic_cast<luxrays::ExtInstanceTriangleMesh*>(extMesh);
    if(pMesh)
    {
        return SaveCtmFile<luxrays::ExtInstanceTriangleMesh>(useplynormals,pMesh,filename,ctxHashValue);
    }
    luxrays::ExtTriangleMesh*   pMesh2 = dynamic_cast<luxrays::ExtTriangleMesh*>(extMesh);
    if(pMesh2)
    {
        return SaveCtmFile<luxrays::ExtTriangleMesh>(useplynormals,pMesh2,filename,ctxHashValue);
    }
    return false;
}


luxrays::ExtMesh*
ExtraObjectManager::getExtMesh(const boost::uuids::uuid &objid)
{
    std::string    name = this->getNameForSlg(objid);
    slg::RenderSession* session = Doc::instance().pDocData->getSession();
    if(session && session->renderConfig->scene)
    {
        return session->renderConfig->scene->meshDefs.GetExtMesh(name);
    }
    return NULL;
}


//@TODO 添加基于文件内容md5码的meshid属性。用于检查mesh一致性。
void
ExtraObjectManager::write(ObjectNode &pThis,ObjectWriteContext& ctx)
{
    std::ostream    &o = ctx.m_stream;
    o  << "<object id='" << ObjectNode::idto_string(pThis.id())
        << "' name='" << pThis.name() << "'";

    luxrays::ExtMesh*   extMesh = getExtMesh(pThis.id());
    if(extMesh){
        o << " material='" << ObjectNode::idto_string(pThis.matid())
            << "' useplynormals='" << (pThis.useplynormals() ? "true" : "false")
            << "'";

        std::string     write_file;
        std::string     ctxHashValue;
        //boost::filesystem::path target_model = ctx.m_dest_path / "mesh%%%%%%.ply";
        boost::filesystem::path target_model = ctx.m_dest_path / "mesh%%%%%%.ctm";
        if(pThis.filepath().length())
        {//获取映射的文件名。
            if(ctx.m_bSaveRes)
            {//保存资源。
                boost::filesystem::path target = boost::filesystem::unique_path(target_model);
                //extMesh->WritePly(target.string());
                SaveCtmFile(pThis.useplynormals(),extMesh,target.string(),ctxHashValue);
                write_file = target.filename().string();
            }else{//不保存资源，直接保存m_filepath.
                write_file = pThis.filepath();
            }
        }else{//没有定义文件名。此时直接保存资源。
            boost::filesystem::path target = boost::filesystem::unique_path(target_model);
            //extMesh->WritePly(target.string());
            SaveCtmFile(pThis.useplynormals(),extMesh,target.string(),ctxHashValue);
            write_file = target.filename().string();
        }
        if(ctxHashValue.length())
        {
            o << " ctxmd5='" << ctxHashValue << "'";
            ObjectWriteContext::type_file_ctxid2savename::iterator it = ctx.m_file_ctx2savename.find(ctxHashValue);
            if(it == ctx.m_file_ctx2savename.end())
            {
                ctx.m_file_ctx2savename[ctxHashValue] = write_file;
            }else{
                write_file = it->second;
                //由于内容重复，删除刚保存的模型文件。
                boost::filesystem::remove(target_model);
            }
        }
        o << " file='" << write_file << "'";
    }
	o << ">" << std::endl;
    ObjectNode::type_child_container::iterator  it = pThis.begin();
    while(it != pThis.end())
    {
        write((*it),ctx);
        it++;
    }
    o << "</object>" << std::endl;
}


}
