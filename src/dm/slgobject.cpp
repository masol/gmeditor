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
#include "slgsetting.h"
#include "docprivate.h"
#include "slg/slg.h"
#include "luxrays/luxrays.h"
#include "luxrays/core/exttrianglemesh.h"
#include "openctm/openctm.h"
#include "utils/MD5.h"
#include "utils/strext.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/scope_exit.hpp>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "luxrays/core/exttrianglemesh.h"


namespace gme{

#if 0
//this member move to slg.
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
    return 0;
}
#endif

bool
ExtraObjectManager::removeMesh(slg::Scene *scene,const std::string &meshID,luxrays::ExtMesh *pMesh,SlgUtil::Editor &editor)
{
    if(!pMesh)
        return false;
    //获取mesh对应的本体信息。
    luxrays::ExtTriangleMesh *pGeometry = luxrays::ExtMeshDefinitions::GetReferMesh(pMesh);

    int ref_count = scene->meshDefs.GetReferenceCount(pGeometry);

    //清理extMeshCache.
    //int ref_count = deleteFromExtMeshCache(scene->extMeshCache,pMesh);

    //清理meshDefs.
    //mesh_idx 指示了本mesh在mesh array中的位置。灯光，材质等信息与位置关联。
    u_int mesh_idx = scene->meshDefs.GetExtMeshIndex(pMesh);
    //指示这个material是否被共享。
    bool materialIsShared = false;
    slg::Material   *pRefMaterial = NULL;

    scene->meshDefs.DeleteExtMesh(meshID);

    {//清理objectMaterials.
        GME_TRACE("erase mesh at mesh_idx = ",mesh_idx);
        std::vector< slg::Material * >::iterator matit = scene->objectMaterials.begin() + mesh_idx;
        pRefMaterial = *matit;
        scene->objectMaterials.erase(matit);
        BOOST_FOREACH(slg::Material *pMat,scene->objectMaterials)
        {
            if(pMat == pRefMaterial)
            {
                materialIsShared = true;
                break;
            }
        }
        GME_TRACE("erase mesh material.");
    }

    //清理meshTriLightDefsOffset
    u_int   meshTriLight_Offset = 0;
    {
        std::vector< u_int >::iterator meshTriLigh_it = scene->meshTriLightDefsOffset.begin() + mesh_idx;
        meshTriLight_Offset = *meshTriLigh_it;
        scene->meshTriLightDefsOffset.erase(meshTriLigh_it);
    }

    //检查材质是否是光源.如果是，继续清理triLightDefs.
    if(ExtraMaterialManager::materialIsLight(pRefMaterial))
    {
        GME_TRACE("pMesh->GetTotalTriangleCount()=",pMesh->GetTotalTriangleCount());
        unsigned int triCount = pMesh->GetTotalTriangleCount();
        for(u_int beginIdx = 0; beginIdx < triCount; beginIdx++ )
        {///@fixme: triangleLights.这里我们不能删除。只能设置为NULL,否则会引发其它灯光的逻辑关系错位。
            slg::TriangleLight *pTriLight = scene->triLightDefs[meshTriLight_Offset + beginIdx];
            scene->triLightDefs[meshTriLight_Offset + beginIdx] = NULL;
            BOOST_ASSERT_MSG(pTriLight->GetMesh() == pMesh,"wrong TriangleLight!");
            delete pTriLight;
        }
        editor.addAction(slg::AREALIGHTS_EDIT);
    }

    if(!materialIsShared)
    {
        //最后，清理extra信息。
        Doc::instance().pDocData->matManager.onMaterialRemoved(pRefMaterial);
    }

    if(ref_count <= 1)
    {//需要删除本体,如果有的话。
        scene->extMeshCache.DeleteExtMesh(pGeometry);
        delete pGeometry;
    }
    //last, delete pMesh. only when pMesh is a instance.
    if(pGeometry != pMesh)
    {
        delete pMesh;
    }

    editor.addAction(slg::GEOMETRY_EDIT);
    editor.addAction(slg::INSTANCE_TRANS_EDIT);
    editor.addAction(slg::MATERIALS_EDIT);
    //editor.needRefresh(true);
    return true;
}

void
ExtraObjectManager::removeMesh(ObjectNode &parent,ObjectNode &self,slg::Scene *scene,SlgUtil::Editor &editor)
{
    //首先遍历删除子节点。
    while(self.m_children.size())
    {
        ObjectNode &child = self.m_children[0];
        this->removeMesh(self,child,scene,editor);
    }
    //然后删除自身。
    if(!self.matid().empty())
    {
        ///@fixme this is cause crash at new version slg.change the first solution to set material to null.but null material has problem.then the solution is dump and reload scene.
        luxrays::ExtMesh *pMesh = this->getExtMesh(self.id());
        if(pMesh)
            removeMesh(scene,self.id(),pMesh,editor);
    }
    parent.removeChild(self.id());
}

bool
ExtraObjectManager::removeMesh(const std::string &id)
{
    if(id.length() == 0)
        return false;
    ObjectNodePath   path;
    ObjectNode *pNode = this->m_objectGroup.findObject(id,&path);
    if(!pNode)
    {
        return false;
    }

    ObjectNode  *parent = path.getNodeFromTail(1);
    BOOST_ASSERT_MSG(parent,"found root in tree??");

    slg::RenderSession* session = Doc::instance().pDocData->getSession();
    slg::Scene       *scene = session->renderConfig->scene;
    SlgUtil::Editor      editor(session);
    ///
    //editor.needRefresh(true);

    this->removeMesh(*parent,*pNode,scene,editor);

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
            const char * ctm_diff_file = ctmGetUVMapString(context,uvid,CTM_FILE_NAME);
            if(ctm_diff_file)
                diffuse_file = ctm_diff_file;
        }

        if(obj.matid().length() == 0)
        {
            ///只有在没有给出材质信息时，我们才尝试加载贴图文件.否则我们使用ctm作为geometry data.没有索引的贴图文件信息。
            obj.m_matid = string::uuid_to_string(gen());
            if(diffuse_file.length())
                Doc::instance().pDocData->matManager.createMatteMaterial(ctx,obj.matid(),"",diffuse_file);
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
        std::string  meshIdentify = DefineObject(ctx.scene(),(const long)vertCount, (const long)triCount,pPoint, pTri, NULL, uv, NULL,NULL, false);

        // Add the object to the scene
        ctx.scene()->AddObject(obj.id(), meshIdentify,
                "scene.objects." + obj.id() + ".material = " + obj.matid() + "\n"
                "scene.objects." + obj.id() + ".useplynormals = 0\n"
            );

        ctx.addAction(slg::GEOMETRY_EDIT);
        bAdd = true;
    }
    return bAdd;
}

void
ExtraObjectManager::importAiMaterial(aiMaterial *pMat,const std::string &id,const std::string &name,ImportContext &ctx)
{
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

//    matCount = pMat->GetTextureCount(aiTextureType_REFLECTION);
//    if(matCount && (pMat->GetTexture(aiTextureType_EMISSIVE, 0, &emmisionPath, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) )
//    {//metal material.
//    }

//    type_xml_node   *pNode = doc->allocate_node(NS_RAPIDXML::node_element,)
    Doc::instance().pDocData->matManager.createMatteMaterial(ctx,id,name,diffusePath.C_Str(),emmisionPath.C_Str(),normalPath.C_Str());
}


bool
ExtraObjectManager::importAiMesh(const aiScene *assimpScene,aiMesh* pMesh,ObjectNode &obj,ImportContext &ctx)
{
    bool    bAdd = false;
    if(pMesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE)
    {//只输出三角形面。
        boost::uuids::random_generator  gen;
        std::string mat_name;
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

                //如果mesh名字为空，则采用material名字。
                aiString name;
                pMat->Get(AI_MATKEY_NAME,name);
                if(name.length)
                {
                    mat_name = name.C_Str();
                }

                importAiMaterial(pMat,obj.matid(),mat_name,ctx);
            }else{
                Doc::instance().pDocData->matManager.createGrayMaterial(ctx,obj.matid());
            }
        }

        if(obj.m_name.length() == 0)
        {
            if(pMesh->mName.length >  0)
                obj.m_name = pMesh->mName.C_Str();
            else if(mat_name.length() > 0)
                obj.m_name = mat_name;
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
                //ignore non-triangle.
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

        if(pMesh->HasNormals())
        {
            normal = new luxrays::Normal[pMesh->mNumVertices];
            memcpy(normal,pMesh->mNormals,sizeof(luxrays::Normal) * pMesh->mNumVertices);
        }


        std::string     meshIdentify = DefineObject(ctx.scene(),pMesh->mNumVertices, realFace,pPoint, pTri, normal, uv, NULL,NULL,false);

        ///@fixme : 这里加入matrix导入.
        // Add the object to the scene
        ctx.scene()->AddObject(obj.id(), meshIdentify,
                "scene.objects." + obj.id() + ".material = " + obj.matid() + "\n"
                "scene.objects." + obj.id() + ".useplynormals = 0\n"
            );
        bAdd = true;
    }
    return bAdd;
}

std::string
ExtraObjectManager::DefineObject(slg::Scene *scene,long plyNbVerts, const long plyNbTris,luxrays::Point *pPoint,
            luxrays::Triangle *pTri, luxrays::Normal *normal, luxrays::UV *uv,luxrays::Spectrum *cols, float *alphas,
            const bool usePlyNormals)
{
    MD5 md5;
    md5.update((const char*)(void*)pPoint,sizeof(luxrays::Point) * plyNbVerts);
    md5.update((const char*)(void*)pTri,sizeof(luxrays::Triangle) * plyNbTris);
    if(uv)
    {
        md5.update((const char*)(void*)uv,sizeof(luxrays::UV) * plyNbVerts);
    }
    char c = (usePlyNormals ?  1 : 0);
    md5.update(&c,1);
    std::string meshIdentify = md5.finalize().hexdigest();
    ///@todo we not caculate normal,cols and alphas.
    if(scene->extMeshCache.FindExtMesh(meshIdentify,usePlyNormals))
    {//对象已经定义，删除分配的空间。
        std::cerr << "found a mesh instance,use old " << meshIdentify << std::endl;
        delete[] pPoint;
        delete[] pTri;
        pPoint = NULL;
        pTri = NULL;
        if(uv)
        {
            delete[] uv;
            uv = NULL;
        }
        if(cols)
        {
            delete[] cols;
            cols = NULL;
        }
        if(alphas)
        {
            delete[] alphas;
            alphas = NULL;
        }
    }else{
        //define object.
        ///@fixme: 由于realFace可能小于pMesh->mNumFaces，这里有可能浪费一点内存，检测此情况并copy内存。
        scene->DefineObject(meshIdentify, plyNbVerts, plyNbTris,pPoint, pTri, normal, uv, cols,alphas,usePlyNormals);
    }
    return meshIdentify;
}


///@todo transform support.
bool
ExtraObjectManager::importAiNode(const aiScene *assimpScene,aiNode* pNode,ObjectNode &objNode,ImportContext &ctx)
{
    bool    bAdd = false;
    unsigned int tricount = 0;
    for(unsigned int i = 0; i < pNode->mNumMeshes; i++)
    {
        aiMesh  *pMesh = assimpScene->mMeshes[pNode->mMeshes[i]];
        if(pMesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE)
        {
            tricount++;
        }
    }

    if(pNode->mNumMeshes > 0)
    {
        for(unsigned int i = 0; i < pNode->mNumMeshes; i++)
        {
            aiMesh  *pMesh = assimpScene->mMeshes[pNode->mMeshes[i]];
            if(tricount == 1)
            {
                if(importAiMesh(assimpScene,pMesh,objNode,ctx))
                {
                    bAdd = true;
                    break;
                }
            }else{
                ObjectNode  obj;
                ///@todo error revovery
                if(importAiMesh(assimpScene,pMesh,obj,ctx))
                {
                    objNode.addChild(obj);
                    bAdd = true;
                }
            }
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
        type_xml_doc    *pDoc = node.document();
        BOOST_ASSERT_MSG(pDoc,"invalid document.");

		const char* transform = NULL;
		type_xml_attr*  pAttr = node.first_attribute();
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
                objNode.m_filepath = ctx.findFile(pAttr->value(),false);//boost::filesystem::canonical(pAttr->value(),basepath).string();
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
    boost::filesystem::ifstream file(path,std::ifstream::binary);
    if(file) {
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
        // ...buffer contains the entire file...
        buffer[length] = 0;
        type_xml_doc    doc;
        const int flag = NS_RAPIDXML::parse_no_element_values | NS_RAPIDXML::parse_trim_whitespace;
        try{
            doc.parse<flag>(buffer);
            //doc.append_attribute(allocate_attribute(&doc,constDef::file,ctx.docBasepath()));

            type_xml_node   *pScene = doc.first_node("scene");
            type_xml_node   *pObjects = NULL;
            if(pScene)
            {
                pObjects = pScene->first_node("objects");
                type_xml_node *pCamera = pScene->first_node("cameras");
                if(pCamera)
                {//load camera database.
                    Doc::instance().pDocData->camManager.findAndImportCamera(*pCamera);
                }
                type_xml_node *pLights = pScene->first_node(constDef::lights);
                if(pLights)
                {//load lights define.
                    ExtraSettingManager::createLights(ctx,*pLights);
                }
            }
            ///@brief load object library here.
            if(!pObjects)
                pObjects = &doc;
            count = findAndImportObject(*pObjects,parentNode,ctx);
        }catch(std::exception &e)
        {
            (void)e;
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

//        std::cerr << "importer.GetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE) = " << importer.GetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE) <<std::endl;
//        importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE,80.f);

        const aiScene* assimpScene = importer.ReadFile( path,
                aiProcess_ValidateDataStructure  |
                aiProcess_GenSmoothNormals       |
                aiProcess_Triangulate            |
                aiProcess_JoinIdenticalVertices  |
                aiProcess_ImproveCacheLocality   |
                aiProcess_FixInfacingNormals     |
                aiProcess_FindDegenerates        |
                aiProcess_FindInvalidData        |
                aiProcess_OptimizeMeshes         |
                aiProcess_OptimizeGraph          |
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
                //再次检查是否只有一个孩子，如果是，加入到当前节点。否则需要把obj的matid等信息清空。
                if(obj.size())
                {//有孩子，清空自身matid等信息。
                    obj.m_filepath.clear();
                    obj.m_matid.clear();
                }
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
    luxrays::ExtMesh* ret = NULL;
    if(session && session->renderConfig->scene)
    {
        try{
            ret = session->renderConfig->scene->meshDefs.GetExtMesh(objid);
        }catch(std::runtime_error &e)
        {
            (void)e;
            ret = NULL;
        }
    }
    return ret;
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
