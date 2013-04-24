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

    if(ref_count <= 1)
    {//需要删除本体,如果有的话。
        luxrays::ExtInstanceTriangleMesh    *pInstance = dynamic_cast<luxrays::ExtInstanceTriangleMesh*>(pMesh);
        if(pInstance)
        {
            pInstance->GetExtTriangleMesh()->Delete();
            delete pInstance->GetExtTriangleMesh();
        }
    }

    //最后，清理extra信息。
    Doc::instance().pDocData->matManager.onMaterialRemoved(pRefMaterial);

    ///@todo: 我们是否需要对象缓冲？现在删了下次就必须重新加载。
    //最后删除对象。
    pMesh->Delete();
    delete pMesh;

    editor.addAction(slg::GEOMETRY_EDIT);
    editor.addAction(slg::INSTANCE_TRANS_EDIT);
    editor.addAction(slg::MATERIALS_EDIT);
    editor.addAction(slg::MATERIAL_TYPES_EDIT);

    parent->removeChild(id);

    return true;
}

bool
ExtraObjectManager::importCTMObj(const std::string& path,ObjectNode &obj,ImportContext &ctx)
{
    bool   bAdd = false;
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

        boost::uuids::random_generator  gen;
        if(obj.id().length() == 0)
        {
            obj.m_id = string::uuid_to_string(gen());
        }

        std::string     diffuse_file;
        if(ctmGetInteger(context, CTM_UV_MAP_COUNT) > 0)
        {//获取uv信息
            CTMenum uvid = ctmGetNamedUVMap(context,"def");
            if(uvid < CTM_UV_MAP_1 || uvid > CTM_UV_MAP_8)
                uvid = CTM_UV_MAP_1;
            uvarray = ctmGetFloatArray(context,uvid);
            diffuse_file = ctmGetUVMapString(context,uvid,CTM_FILE_NAME);
        }

        if(obj.matid().length() == 0)
        {
            ///只有在没有给出材质信息时，我们才尝试加载贴图文件.否则我们使用ctm作为geometry data.没有索引的贴图文件信息。
            obj.m_matid = string::uuid_to_string(gen());
            if(diffuse_file.length())
                Doc::instance().pDocData->matManager.createMatteMaterial(ctx,obj.matid(),diffuse_file);
            else
                Doc::instance().pDocData->matManager.createGrayMaterial(ctx,obj.matid());
        }

        //we use ctm as comporessed geometry data. so no name information here.
        if(obj.name().length() == 0)
        {//use file stem as the init name.
            obj.m_name = boost::filesystem::path(path).stem().string();
        }

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
        ctx.scene()->DefineObject("m" + obj.id(), (const long)vertCount, (const long)triCount,pPoint, pTri, NULL, uv, NULL,NULL, false);

        // Add the object to the scene
        ctx.scene()->AddObject(obj.id(), "m" + obj.id(),
                "scene.objects." + obj.id() + ".material = " + obj.matid() + "\n"
                "scene.objects." + obj.id() + ".useplynormals = 0\n"
            );

        ctx.addAction(slg::GEOMETRY_EDIT);
        bAdd = true;
    }
    return bAdd;
}

bool
ExtraObjectManager::importAiMesh(const aiScene *assimpScene,aiMesh* pMesh,ObjectNode &obj,ImportContext &ctx)
{
    bool    bAdd = false;
    if(pMesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE)
    {//只输出三角形面。
        boost::uuids::random_generator  gen;
        if(obj.id().length() == 0)
        {
            obj.m_id = string::uuid_to_string(gen());
        }
        if(obj.matid().length() == 0)
        {
            obj.m_matid = string::uuid_to_string(gen());
            if(pMesh->mMaterialIndex >= 0)
            {
                ///@todo add material name if exist.
                aiMaterial *pMat = assimpScene->mMaterials[pMesh->mMaterialIndex];

                aiString    diffusePath;
                aiString    normalPath;
                aiString    emmisionPath;
                unsigned int matCount = pMat->GetTextureCount(aiTextureType_DIFFUSE);
                if(!matCount || (pMat->GetTexture(aiTextureType_DIFFUSE, 0, &diffusePath, NULL, NULL, NULL, NULL, NULL) != AI_SUCCESS) )
                {
                    diffusePath.Clear();
                }

                matCount = pMat->GetTextureCount(aiTextureType_NORMALS);
                if(!matCount || (pMat->GetTexture(aiTextureType_NORMALS, 0, &normalPath, NULL, NULL, NULL, NULL, NULL) != AI_SUCCESS) )
                {
                    normalPath.Clear();
                }

                matCount = pMat->GetTextureCount(aiTextureType_EMISSIVE);
                if(!matCount || (pMat->GetTexture(aiTextureType_EMISSIVE, 0, &emmisionPath, NULL, NULL, NULL, NULL, NULL) != AI_SUCCESS) )
                {
                    emmisionPath.Clear();
                }

                Doc::instance().pDocData->matManager.createMatteMaterial(ctx,obj.matid(),diffusePath.C_Str(),emmisionPath.C_Str(),normalPath.C_Str());
            }else{
                Doc::instance().pDocData->matManager.createGrayMaterial(ctx,obj.matid());
            }
        }

        if(obj.name().length() == 0)
        {
            obj.m_name = pMesh->mName.C_Str();
        }
        //这里的内存会被luxrays管理。因此不能被释放。
        luxrays::Point  *pPoint = new luxrays::Point[pMesh->mNumVertices];
        memcpy(pPoint,pMesh->mVertices,sizeof(luxrays::Point) * pMesh->mNumVertices);
        luxrays::Triangle   *pTri = new luxrays::Triangle[pMesh->mNumFaces];
        luxrays::UV *uv = NULL;
        //@fixme: support normal.
        luxrays::Normal *normal = NULL;

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

        ///@fixme : 这里加入matrix导入.
        //define object.
        ctx.scene()->DefineObject("m" + obj.id(), pMesh->mNumVertices, realFace,pPoint, pTri, normal, uv, NULL,NULL,false);

        // Add the object to the scene
        ctx.scene()->AddObject(obj.id(), "m" + obj.id(),
                "scene.objects." + obj.id() + ".material = " + obj.matid() + "\n"
                "scene.objects." + obj.id() + ".useplynormals = 0\n"
            );
        bAdd = true;
    }
    return bAdd;
}


///@todo transform support.
bool
ExtraObjectManager::importAiNode(const aiScene *assimpScene,aiNode* pNode,ObjectNode &objNode,ImportContext &ctx)
{
    bool    bAdd = false;
    if(pNode->mNumMeshes > 1)
    {//超过多于1个mesh.需要在当前mesh下构建子节点。
        for(unsigned int i = 0; i < pNode->mNumMeshes; i++)
        {
            aiMesh  *pMesh = assimpScene->mMeshes[pNode->mMeshes[i]];
            ObjectNode  obj;
            ///@todo error revovery
            if(importAiMesh(assimpScene,pMesh,obj,ctx))
            {
                objNode.addChild(obj);
                bAdd = true;
            }
        }
    }else if(pNode->mNumMeshes)
    {
        aiMesh  *pMesh = assimpScene->mMeshes[pNode->mMeshes[0]];
        if(importAiMesh(assimpScene,pMesh,objNode,ctx))
        {
            bAdd = true;
        }
    }
    //开始处理孩子。
    for(unsigned int i = 0; i < pNode->mNumChildren; i++)
    {
        aiNode *child = pNode->mChildren[i];
        ObjectNode  childNode;
        if(importAiNode(assimpScene,child,childNode,ctx))
        {
            objNode.addChild(childNode);
            bAdd = true;
        }
    }
    if(bAdd)
    {//如果objNode无id,这里添加。
        if(objNode.id().length() == 0)
        {
            objNode.m_id = string::uuid_to_string(boost::uuids::random_generator()());
        }
    }
    return bAdd;
}

int
ExtraObjectManager::importObjects(type_xml_node &node,ObjectNode &objNode,ImportContext &ctx)
{
    int ret = 0;
    if(boost::iequals(constDef::object,node.name()))
    {
        std::string     basepath;
        type_xml_doc    *pDoc = node.document();
        BOOST_ASSERT_MSG(pDoc,"invalid document.");
        type_xml_attr*  pAttr = pDoc->first_attribute(constDef::file);
        if(pAttr)
        {
            basepath = pAttr->value();
        }
        if(basepath.length() == 0)
            basepath = boost::filesystem::current_path().string();

		const char* transform = NULL;
		pAttr = node.first_attribute();
		while(pAttr)
		{
            if(boost::iequals(constDef::id,pAttr->name()))
            {
                objNode.m_id = pAttr->value();
            }else if(boost::iequals(constDef::name,pAttr->name()))
            {
                objNode.m_name = pAttr->value();
            }else if(boost::iequals(constDef::file,pAttr->name()))
            {
                objNode.m_filepath = boost::filesystem::canonical(pAttr->value(),basepath).string();
            }else if(boost::iequals(constDef::transformation,pAttr->name()))
            {
                transform = pAttr->value();
            }
            else if(boost::iequals("matid",pAttr->name()))
            {
                objNode.m_matid = pAttr->value();
            }
			pAttr = pAttr->next_attribute();
		}
		//开始读入material.
		type_xml_node *pMatNode = node.first_node(constDef::material);

		bool  loadOk = false;
		if(pMatNode)
		{
            Doc::instance().pDocData->matManager.createMaterial(ctx,objNode.m_matid,*pMatNode);

            //开始加载内容。
            if(objNode.m_filepath.length())
            {
                //开始设置transform
                if(transform)
                {
                }

                ///@fixme : how to set transform?
                loadOk = importObjects(objNode.filepath(),objNode,ctx);
            }
		}

		if(loadOk)
		{
            ret++;
        }else{
            objNode.m_filepath.clear();
        }

		//process sub object.
        type_xml_node   *pChild = node.first_node(constDef::object);
        while(pChild)
        {
            ObjectNode  childNode;
            int tmpRet = importObjects(*pChild,childNode,ctx);
            if(tmpRet)
            {
                ret += tmpRet;
                objNode.addChild(childNode);
            }
            pChild = pChild->next_sibling(constDef::object);
        }
    }
    return ret;
}

int
ExtraObjectManager::findAndImportObject(type_xml_node &node,ObjectNode &parentNode,ImportContext &ctx)
{
    int ret = 0;
    if(boost::iequals(constDef::object,node.name()))
    {
        ObjectNode obj;
        int tmpRet = importObjects(node,obj,ctx);
        if(tmpRet)
        {
            ret += tmpRet;
            parentNode.addChild(obj);
        }
    }else{
        type_xml_node *pChild = node.first_node();
        while(pChild)
        {
            ret += findAndImportObject(*pChild,parentNode,ctx);
            pChild = pChild->next_sibling();
        }
    }
    return ret;
}

int
ExtraObjectManager::importSpScene(const std::string &path,ObjectNode &parentNode,ImportContext &ctx)
{
    int    count = 0;
    std::ifstream file(path.c_str(),std::ifstream::binary);
    if (file) {
        BOOST_SCOPE_EXIT( (&file))
        {
            file.close();
        }BOOST_SCOPE_EXIT_END
        // get length of file:
        file.seekg (0, file.end);
        int length = file.tellg();
        file.seekg (0, file.beg);

        char * buffer = new char [length + 1];
        BOOST_SCOPE_EXIT( (buffer))
        {
            delete[] buffer;
        }BOOST_SCOPE_EXIT_END
        // read data as a block:
        file.read (buffer,length);
        if(file)
        {
            // ...buffer contains the entire file...
            buffer[length] = 0;
            type_xml_doc    doc;
            const int flag = NS_RAPIDXML::parse_no_element_values | NS_RAPIDXML::parse_trim_whitespace;
            try{
                doc.parse<flag>(buffer);
                boost::filesystem::path tmp = boost::filesystem::canonical(path);
                doc.append_attribute(doc.allocate_attribute(constDef::file,tmp.parent_path().string().c_str()));
                count = findAndImportObject(doc,parentNode,ctx);
            }catch(std::exception &e)
            {
                (void)e;
            }
        }
    }
    return count;
}


bool
ExtraObjectManager::importObjects(const std::string& path,ObjectNode &obj,ImportContext &ctx)
{
    std::string ext = boost::filesystem::gme_ext::get_extension(path);

    bool    bAdd = false;
    if(boost::iequals(ext,".ctm"))
    {
        if(importCTMObj(path,obj,ctx))
        {
            obj.m_filepath = path;
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
            if(assimpScene->mNumMeshes == 1)
            {//只有模型为1时我们才加入原始路径。
                obj.m_filepath = path;
            }
            //从root开始加载数据。
            if(importAiNode(assimpScene,assimpScene->mRootNode,obj,ctx))
            {
                bAdd = true;
            }
        }
    }

    if(bAdd)
    {
        ctx.addAction(slg::GEOMETRY_EDIT);
        ctx.addAction(slg::INSTANCE_TRANS_EDIT);
        //ctx.addAction(slg::MATERIALS_EDIT);
        //ctx.addAction(slg::MATERIAL_TYPES_EDIT);
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
