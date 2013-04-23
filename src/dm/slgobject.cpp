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
#include "utils/strext.h"
#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "luxrays/core/exttrianglemesh.h"


namespace gme{

int
ExtraObjectManager::deleteFromExtMeshCache(luxrays::ExtMeshCache &ec,luxrays::ExtMesh *pObject)
{
    //获取对应的geometry对象。
    luxrays::ExtTriangleMesh *pGeometry = NULL;
    {
        luxrays::ExtInstanceTriangleMesh    *pInstance = dynamic_cast<luxrays::ExtInstanceTriangleMesh*>(pObject);
        if(pInstance)
        {
            pGeometry = pInstance->GetExtTriangleMesh();
        }else{
            pGeometry = dynamic_cast<luxrays::ExtTriangleMesh*>(pObject);
        }
    }

    int reference_count = 0;
    {//如果pObject是一个mesh，则引用技术应该是0.
        std::vector< luxrays::ExtMesh * >::iterator  it = ec.meshes.begin();
        while(it != ec.meshes.end())
        {
            if(pGeometry == *it)
            {
                reference_count++;
            }else{
                luxrays::ExtInstanceTriangleMesh    *pInstance = dynamic_cast<luxrays::ExtInstanceTriangleMesh*>(*it);
                if(pInstance && pInstance->GetExtTriangleMesh() == pGeometry)
                {
                    reference_count++;
                }
            }
            //如果mesh是指定mesh,删除之。
            if(pObject == *it)
            {
                it = ec.meshes.erase(it);
                GME_TRACE("remove object...done");
            }else{
                it++;
            }
        }
    }
    //引用计数为0的情况说明没有使用instance信息，直接使用mesh作为object.可以安全删除。
    if(reference_count <= 1)
    {
        if(pGeometry)
        {
            std::map<std::string, luxrays::ExtTriangleMesh *>::iterator it = ec.maps.begin();
            while(it != ec.maps.end())
            {
                if(it->second == pGeometry)
                {
                    ec.maps.erase(it);
                    GME_TRACE("remove mesh...done");
                    break;
                }
                it++;
            }
        }
    }

    return reference_count;
}

bool
ExtraObjectManager::removeMesh(const std::string &id)
{
    ObjectNode  *parent = NULL;
    ObjectNode *pNode = this->m_objectGroup.findObject(id);
    if(!pNode)
    {
        return false;
    }

    if(!parent)
        parent = &this->m_objectGroup;

    slg::RenderSession* session = Doc::instance().pDocData->getSession();
    slg::Scene       *scene = session->renderConfig->scene;
    luxrays::ExtMesh *pMesh = this->getExtMesh(id);//scene->meshDefs.GetExtMesh("luxshell");
    if(!pMesh)
    {
        return false;
    }

    SlgUtil::Editor      editor(session);

    //清理extMeshCache.
    int ref_count = deleteFromExtMeshCache(scene->extMeshCache,pMesh);

    //清理meshDefs.
    //triangleLights_begin保存了对象对应光源三角形数组的开始位置。
    int triangleLights_begin = 0;
    slg::Material   *pRefMaterial = NULL;
    {
        u_int idx = 0;
        std::vector< luxrays::ExtMesh * >::iterator  it = scene->meshDefs.meshs.begin();
        while(it != scene->meshDefs.meshs.end())
        {
            if(pMesh != *it)
            {
                triangleLights_begin += pMesh->GetTotalTriangleCount();
            }else{
                scene->meshDefs.meshs.erase(it);
                break;
            }
            it++;
            idx++;
        }
        GME_TRACE("erase mesh at idx = ",idx);
        std::vector< slg::Material * >::iterator matit = scene->objectMaterials.begin() + idx;
        pRefMaterial = *matit;
        scene->objectMaterials.erase(matit);
        GME_TRACE("erase mesh material.");
    }
    {//删除mesh的name define.
        std::map< std::string, luxrays::ExtMesh * >::iterator it = scene->meshDefs.meshsByName.begin();
        while(it != scene->meshDefs.meshsByName.end())
        {
            if(it->second == pMesh)
            {
                scene->meshDefs.meshsByName.erase(it);
                GME_TRACE("erase mesh name define.");
                break;
            }
            it++;
        }
    }

#if 0
    //清理triangleLights.这里我们不能删除。只能设置为NULL,否则会引发其它灯光的逻辑关系错位。
    {
        GME_TRACE("triangleLights_begin = ",triangleLights_begin,";scene->triangleLights.size=",scene->triangleLights.size());
        GME_TRACE("pMesh->GetTotalTriangleCount()=",pMesh->GetTotalTriangleCount());
        std::vector<slg::TriangleLight *>::iterator it = scene->triangleLights.begin() + triangleLights_begin;
        BOOST_ASSERT_MSG( (  ( (*it) == NULL) || ( (*it)->GetMesh() == pMesh && (*it)->GetTriIndex() == 0 ) ),"panic light position!");
        scene->triangleLights.erase(it,it+pMesh->GetTotalTriangleCount()-1);
    }
#endif

    //检查材质是否是光源.如果是，继续清理triLightDefs
    if(ExtraMaterialManager::materialIsLight(pRefMaterial))
    {//开始寻找mesh对应的光源。
        std::vector< slg::TriangleLight * >::iterator    it = scene->triLightDefs.begin();
        while(it != scene->triLightDefs.end())
        {
            if( (*it)->GetMesh () == pMesh)
            {
                slg::TriangleLight *pTriLight = *it;
                GME_TRACE("erase light triangle : ",pTriLight->GetTriIndex());
                ///@fixme: 设置全局三角形数组为空。
                //scene->triangleLights[scene->triangleLights.begin() + pTriLight->] = NULL;

                it = scene->triLightDefs.erase(it);
                delete  pTriLight;
            }else{
                it++;
            }
        }
        editor.addAction(slg::AREALIGHTS_EDIT);
    }
//#endif

    if(ref_count <= 1)
    {//需要删除本体,如果有的话。
        luxrays::ExtInstanceTriangleMesh    *pInstance = dynamic_cast<luxrays::ExtInstanceTriangleMesh*>(pMesh);
        if(pInstance)
        {
            pInstance->GetExtTriangleMesh()->Delete();
            delete pInstance->GetExtTriangleMesh();
        }
    }

    ///@todo: 我们是否需要对象缓冲？现在删了下次就必须重新加载。
    //最后删除对象。
    pMesh->Delete();
    delete pMesh;

    editor.addAction(slg::GEOMETRY_EDIT);
    editor.addAction(slg::INSTANCE_TRANS_EDIT);
    editor.addAction(slg::MATERIALS_EDIT);
    editor.addAction(slg::MATERIAL_TYPES_EDIT);

    //最后，清理extra信息。
    Doc::instance().pDocData->matManager.eraseMaterialInfo(pNode->matid(),pRefMaterial);
    parent->removeChild(id);

    return true;
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
            obj.m_id = string::uuid_to_string(gen());
            obj.m_matid = string::uuid_to_string(gen());

            ExtraMaterialManager::createGrayMaterial(obj.m_matid);
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

            //define object.
            scene->DefineObject("m" + obj.id(), (const long)vertCount, (const long)triCount,pPoint, pTri, NULL, uv, NULL,NULL, false);

            // Add the object to the scene
            scene->AddObject(obj.id(), "m" + obj.id(),
                    "scene.objects." + obj.id() + ".material = " + obj.matid() + "\n"
                    "scene.objects." + obj.id() + ".useplynormals = 0\n"
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
                    obj.m_id = string::uuid_to_string(gen());
                    obj.m_matid = string::uuid_to_string(gen());
                    ExtraMaterialManager::createGrayMaterial(obj.m_matid);
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
                    //@fixme: support normal.
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

                    //define object.
                    scene->DefineObject("m" + obj.id(), pMesh->mNumVertices, realFace,pPoint, pTri, normal, uv, NULL,NULL,false);

                    // Add the object to the scene
                    scene->AddObject(obj.id(), "m" + obj.id(),
                            "scene.objects." + obj.id() + ".material = " + obj.matid() + "\n"
                            "scene.objects." + obj.id() + ".useplynormals = 0\n"
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


luxrays::ExtMesh*
ExtraObjectManager::getExtMesh(const std::string &objid)
{
    slg::RenderSession* session = Doc::instance().pDocData->getSession();
    if(session && session->renderConfig->scene)
    {
        return session->renderConfig->scene->meshDefs.GetExtMesh(objid);
    }
    return NULL;
}

/*
//基于文件内容md5码的ctxmd5属性。用于检查mesh一致性。
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
*/
void
ExtraObjectManager::loadExtraFromProps(ObjectNode& node,luxrays::Properties &props)
{
    std::string path = props.GetString("scene.objects." + node.id() + ".ply","");
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
