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

#ifndef  GME_DM_SLGOBJECT_H
#define  GME_DM_SLGOBJECT_H

#include "dm/objectnode.h"
#include "utils/singleton.h"
#include "utils/pathext.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/unordered_map.hpp>
#include "luxrays/utils/properties.h"
#include "slg/slg.h"
#include "slg/editaction.h"
#include "slg/rendersession.h"
#include "slgutils.h"
#include "importctx.h"
#include "assimp/scene.h"

//forward declare.
namespace luxrays{
class ExtMesh;
}

namespace gme{

class ExtraObjectManager
{
private:
    friend class DocPrivate;
    ExtraObjectManager(){}
    ~ExtraObjectManager(){}

    /** @brief  保存了mesh组。根节点只作为容器节点存在，数据无效。
    **/
    ObjectNode                              m_objectGroup;
public:
    inline  ObjectNode*     findObject(const std::string &id)
    {
        return m_objectGroup.findObject(id,NULL);
    }
    void    clear(){
        m_objectGroup.clear();
    }
    inline  ObjectNode&     getRoot(void){
        return m_objectGroup;
    }

    /** @brief 将所有子节点dump到parent.没有直接调用m_objectGroup.dump的原因是忽略自身。
    **/
    inline  void  dump(type_xml_node &parent,dumpContext &ctx)
    {
		ObjectNode::type_child_container::iterator it = m_objectGroup.begin();
        while(it != m_objectGroup.end())
        {
            it->dump(parent,ctx);
            it++;
        }
    }

    static  luxrays::ExtMesh*   getExtMesh(const std::string &objid);
    void    loadExtraFromProps(luxrays::Properties &props)
    {//不加载根节点:
         ObjectNode::type_child_container::iterator it = m_objectGroup.begin();
         while(it != m_objectGroup.end())
         {
            loadExtraFromProps(*it,props);
            it++;
         }
    }
public:
    /** @brief Loading objects from the model file according to objNode information.
     * @details If not given matid / objid. Then create random id.
     * @param selfNode : will loading data to selfNode.
    **/
    static  bool   importObjects(const std::string& path,ObjectNode &selfNOde,ImportContext &ctx);
    /** @brief 删除指定模型.
    **/
    static  int   importSpScene(const std::string &path,ObjectNode &parentNode,ImportContext &ctx);
    bool    removeMesh(const std::string &id);
private:
    void    removeMesh(ObjectNode &parent,ObjectNode &self,slg::Scene *scene,SlgUtil::Editor &editor);
    bool    removeMesh(slg::Scene *scene,const std::string &meshID,luxrays::ExtMesh *pMesh,SlgUtil::Editor &editor);
    /** @brief 将指定xml node加载到objNode(node必须为object节点)。
    **/
    static  int   importObjects(type_xml_node &node,ObjectNode &objNode,ImportContext &ctx);
    /** @brief 搜索pNode下的所有子节点，并加入到parentNode中。
     * @return the object count of successfully imported.
    **/
    static  int   findAndImportObject(type_xml_node &pNode,ObjectNode &parentNode,ImportContext &ctx);
    static  bool   importAiNode(const aiScene *assimpScene,aiNode* pNode,ObjectNode &objNode,ImportContext &ctx);

    static  void   importAiMaterial(aiMaterial *pMat,const std::string &id,const std::string &name,ImportContext &ctx);
    /** @brief only load self.no child.
    **/
    static  bool   importAiMesh(const aiScene *assimpScene,aiMesh* pMesh,ObjectNode &objNode,ImportContext &ctx);
    /** @brief 根据内容来确保不加入重复对象。hacker to slg::Scene::DefineObject
     * @details any way, the callee obtain the owner of p,vi,n,uv,cols,alphas.(free use delete[]).
    **/
    static  std::string  DefineObject(slg::Scene *scene,long plyNbVerts, const long plyNbTris,luxrays::Point *p,
            luxrays::Triangle *vi, luxrays::Normal *n, luxrays::UV *uv,luxrays::Spectrum *cols, float *alphas,
            const bool usePlyNormals);
    /** @brief loading openctm data. assume ctm is internal data. generate from gmeditor,so no postprocessing with it!.
    **/
    static  bool   importCTMObj(const std::string& path,ObjectNode &objNode,ImportContext &ctx);

    ///@brief 从props中加载原始文件信息。
    void    loadExtraFromProps(ObjectNode& node,luxrays::Properties &props);
#if 0 //下面两个函数被dave加入到slg主线中。
    /**@brief 清理extMeshCache信息。
      *@details 本函数并不删除pMesh.
      *@return 返回pMesh的引用计数。(0通常说明pMesh自身就是一个mesh而不是object)
    **/
    static  int    deleteFromExtMeshCache(luxrays::ExtMeshCache &extMeshCache,luxrays::ExtMesh *pObject);
    ///@brief 检查有多少个对象引用了指定的造型对象。
    static  int     getReferenceCount(luxrays::ExtMeshCache &extMeshCache,luxrays::ExtTriangleMesh *pGeometry);
#endif
};

}

#endif //GME_DM_SLGOBJECT_H
