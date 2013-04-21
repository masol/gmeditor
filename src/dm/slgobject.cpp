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
#include "slgmaterial.h"
#include "docprivate.h"
#include "slg/slg.h"
#include "luxrays/luxrays.h"
#include "luxrays/core/exttrianglemesh.h"
#include "openctm/openctm.h"
#include "utils/MD5.h"
#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags



#include "luxrays/core/exttrianglemesh.h"
namespace gme{

bool
ExtraObjectManager::removeMesh(const boost::uuids::uuid &id)
{
//    ObjectNode* pNode = this->m_objectGroup.findObject(id);
//    if(pNode)
//    {
//    }
    slg::RenderSession* session = Doc::instance().pDocData->getSession();
    slg::Scene       *scene = session->renderConfig->scene;
    luxrays::ExtMesh *pMesh = scene->meshDefs.GetExtMesh("luxshell");
    if(!pMesh)
    {
        std::cerr << "can not found luxshell" << std::endl;
    }
    SlgUtil::Editor      editor(session);

    std::cerr << "remove mesh id " << scene->extMeshCache.GetExtMeshIndex(pMesh) << std::endl;
    std::cerr << "extMeshCache size : " << scene->extMeshCache.GetMeshes().size() << std::endl;
    //std::cerr << "scene->extMeshCache.RemoveExtMesh = " << scene->extMeshCache.RemoveExtMesh(pMesh) <<std::endl;

  	std::map<std::string, luxrays::ExtTriangleMesh *>::iterator it = scene->extMeshCache.maps.begin();
  	while(it != scene->extMeshCache.maps.end())
  	{
        if(it->second == pMesh)
        {
            std::cerr << "found removed :" << it->first;
            scene->extMeshCache.maps.erase(it);
            break;
        }
        std::cerr << "pair : " << it->first << std::endl;
        it++;
  	}

    u_int idx = scene->meshDefs.GetExtMeshIndex(pMesh);
    std::cerr << "idx = " << idx << std::endl;
    {
        std::vector< luxrays::ExtMesh * >::iterator  it = std::find(scene->meshDefs.meshs.begin(),scene->meshDefs.meshs.end(),pMesh);
        scene->meshDefs.meshs.erase(it);
        std::vector< slg::Material * >::iterator matit = scene->objectMaterials.begin();
        for(u_int i = 0; i < idx; i++)
            matit++;
        scene->objectMaterials.erase(matit);
    }
    {
        std::map< std::string, luxrays::ExtMesh * >::iterator it = scene->meshDefs.meshsByName.begin();
        while(it != scene->meshDefs.meshsByName.end())
        {
            if(it->second == pMesh)
            {
                scene->meshDefs.meshsByName.erase(it);
            }
            it++;
        }
    }
    pMesh->Delete();
    delete pMesh;

    editor.addAction(slg::GEOMETRY_EDIT);
    //editor.addAction(slg::INSTANCE_TRANS_EDIT);
    editor.addAction(slg::MATERIALS_EDIT);
    editor.addAction(slg::MATERIAL_TYPES_EDIT);
    return false;
}

bool
ExtraObjectManager::loadObjectsFromFile(const std::string &path,ObjectNode *pParent,SlgUtil::Editor &editor)
{
    (void)editor;
    if(!pParent)
        pParent = &this->m_objectGroup;

    bool    bAdd = false;
    slg::Scene  *scene = Doc::instance().pDocData->m_session->renderConfig->scene;
    std::string ext = boost::filesystem::gme_ext::get_extension(path);
    if(boost::iequals(ext,".ctm"))
    {//加载openctm. assume ctm is internal data. generate from gmeditor,so no postprocessing with it!.
        CTMcontext context = ctmNewContext(CTM_IMPORT);
        BOOST_SCOPE_EXIT( (&context) )
        {
            if(context){
                ctmFreeContext(context);
            }
        }
        BOOST_SCOPE_EXIT_END

        ctmLoad(context, path.c_str());
        if(ctmGetError(context) == CTM_NONE)
        {
            CTMuint    vertCount, triCount;
            const CTMuint   *indices;
            const CTMfloat  *vertices, *uvarray = NULL;
            // Access the mesh data
            vertCount = ctmGetInteger(context, CTM_VERTEX_COUNT);
            vertices = ctmGetFloatArray(context, CTM_VERTICES);
            triCount = ctmGetInteger(context, CTM_TRIANGLE_COUNT);
            indices = ctmGetIntegerArray(context, CTM_INDICES);

            ObjectNode  obj;
            boost::uuids::random_generator  gen;
            obj.m_id = gen();

            obj.m_matid = ExtraMaterialManager::instance().createGrayMaterial();
            //we use ctm as comporessed geometry data. so no name information here.
            //obj.m_name = ctmGetString(context, CTM_NAME);
            obj.m_filepath = path;

            uvarray = ctmGetFloatArray(context,ctmGetNamedUVMap(context,"def"));
            ///@fixme: 我们只使用ctm作为geometry data.没有索引的贴图文件信息。

            luxrays::Point  *pPoint = new luxrays::Point[vertCount];
            memcpy(pPoint,vertices,sizeof(luxrays::Point) * vertCount);
            luxrays::Triangle   *pTri = new luxrays::Triangle[triCount];
            memcpy(pTri,indices,sizeof(luxrays::Triangle) * triCount);
            luxrays::UV *uv = NULL;
            if(uvarray)
            {
                uv = new luxrays::UV[vertCount];
                memcpy(uv,uvarray,sizeof(luxrays::UV) * vertCount);
            }

            std::string    objName = ObjectNode::idto_string(obj.id());
            //define object.
            scene->DefineObject("m" + objName, (const long)vertCount, (const long)triCount,pPoint, pTri, NULL, uv, NULL,NULL, false);

            // Add the object to the scene
            scene->AddObject(objName, "m" + objName,
                    "scene.objects." + objName + ".material = " + ObjectNode::idto_string(obj.matid()) + "\n"
                    "scene.objects." + objName + ".useplynormals = 0\n"
                );

            pParent->addChild(obj);
            bAdd = true;
        }
    }else{
    //使用assimp加载其它数据。
        Assimp::Importer importer;

        const aiScene* assimpScene = importer.ReadFile( path,
                aiProcess_ValidateDataStructure  |
                aiProcess_GenSmoothNormals       |
                aiProcess_CalcTangentSpace       |
                aiProcess_Triangulate            |
                aiProcess_JoinIdenticalVertices  |
                aiProcess_ImproveCacheLocality   |
                aiProcess_FixInfacingNormals     |
                aiProcess_FindDegenerates        |
                aiProcess_FindInvalidData        |
                aiProcess_GenUVCoords            |
                aiProcess_OptimizeMeshes         |
                aiProcess_Debone                 |
                aiProcess_SortByPType);
        if(assimpScene && assimpScene->HasMeshes())
        {//process data.
            for(unsigned int idx = 0; idx < assimpScene->mNumMeshes; idx++)
            {//process one mesh.
                aiMesh* pMesh = assimpScene->mMeshes[idx];
                if(pMesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE)
                {//只输出三角形面。
                    ObjectNode  obj;
                    boost::uuids::random_generator  gen;
                    obj.m_id = gen();
                    obj.m_matid = ExtraMaterialManager::instance().createGrayMaterial();
                    obj.m_name = pMesh->mName.C_Str();
                    //这里只有第一个被加载的
                    if(idx == 0)
                        obj.m_filepath = path;
//                    if(pMesh->mMaterialIndex >= 0)
//                    {
//                        //matid = ExtraMaterialManager::instance().createAssimpMaterial(aiMaterial *paiMat);
//                    }else{
//                        //matid = ExtraMaterialManager::instance().createStockMaterial();
//                    }
                    //这里的内存会被luxrays管理。因此不能被释放。
                    luxrays::Point  *pPoint = new luxrays::Point[pMesh->mNumVertices];
                    memcpy(pPoint,pMesh->mVertices,sizeof(luxrays::Point) * pMesh->mNumVertices);
                    luxrays::Triangle   *pTri = new luxrays::Triangle[pMesh->mNumFaces];
                    luxrays::UV *uv = NULL;
                    //@TODO: support normal.
                    luxrays::Normal *normal = NULL;

//                    BOOST_SCOPE_EXIT( (&pTri)(&uv) )
//                    {
//                        delete[] pTri;
//                        if(uv){
//                            delete[] uv;
//                        }
//                    }
//                    BOOST_SCOPE_EXIT_END

                    //prepare triangle data.
                    unsigned int   realFace = 0;
                    for(unsigned int i = 0; i < pMesh->mNumFaces; i++)
                    {
                        if(pMesh->mFaces[i].mNumIndices == 3)
                        {
                            //memcpy(pTri[i].v,pMesh->mFaces[i]->mIndices)
                            pTri[realFace].v[0] = pMesh->mFaces[i].mIndices[0];
                            pTri[realFace].v[1] = pMesh->mFaces[i].mIndices[1];
                            pTri[realFace].v[2] = pMesh->mFaces[i].mIndices[2];
                            realFace++;
                        }else{
                            BOOST_ASSERT_MSG(false,"can not support non-triangle now.");
                        }
                    }

                    for(unsigned int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; i++)
                    {
                        if(pMesh->mTextureCoords[i])
                        {//prepare uv data.only use first valid channel.
                            uv = new luxrays::UV[pMesh->mNumVertices];
                            aiVector3D *aiUV = pMesh->mTextureCoords[i];
                            for(i = 0; i < pMesh->mNumVertices; i++)
                            {
                                uv[i].u = aiUV[i].x;
                                uv[i].v = aiUV[i].y;
                            }
                            break;
                        }
                    }

                    std::string    objName = ObjectNode::idto_string(obj.id());
                    //define object.
                    scene->DefineObject("m" + objName, pMesh->mNumVertices, realFace,pPoint, pTri, normal, uv, NULL,NULL,false);

                    // Add the object to the scene
                    scene->AddObject(objName, "m" + objName,
                            "scene.objects." + objName + ".material = " + ObjectNode::idto_string(obj.matid()) + "\n"
                            "scene.objects." + objName + ".useplynormals = 0\n"
                        );

                    pParent->addChild(obj);
                    bAdd = true;
                }
            }
        }
    }

    if(bAdd)
    {
        editor.addAction(slg::GEOMETRY_EDIT);
        //editor.addAction(slg::INSTANCE_TRANS_EDIT);
        editor.addAction(slg::MATERIALS_EDIT);
        editor.addAction(slg::MATERIAL_TYPES_EDIT);
    }
    return bAdd;
}

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

    if(false)//useplynormals) //pMesh->HasNormals())
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
    ctx.outIndent();
    o  << "<object id='" << ObjectNode::idto_string(pThis.id())
        << "' name='" << pThis.name() << "'";

    luxrays::ExtMesh*   extMesh = getExtMesh(pThis.id());
    if(extMesh){
        ctx.m_refMaterials.push_back(pThis.matid());
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
    ctx.m_indent++;
    while(it != pThis.end())
    {
        write((*it),ctx);
        it++;
    }
    ctx.m_indent--;

    ctx.outIndent();
    o << "</object>" << std::endl;
}

void
ExtraObjectManager::loadExtraFromProps(ObjectNode& node,luxrays::Properties &props)
{
    std::string     slgname = getNameForSlg(node.id());
    std::string path = props.GetString("scene.objects." + slgname + ".ply","");
    if(path.length())
    {
        boost::filesystem::path   filepath = boost::filesystem::canonical(path);
        node.m_filepath = filepath.string();
    }

    ObjectNode::type_child_container::iterator it = node.begin();
    while(it != node.end())
    {
        loadExtraFromProps(*it,props);
        it++;
    }
}



}
