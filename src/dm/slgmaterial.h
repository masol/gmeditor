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

#ifndef  GME_DM_SLGMATERIAL_H
#define  GME_DM_SLGMATERIAL_H

#include "utils/singleton.h"
#include "slg/rendersession.h"
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include "slgtexture.h"
#include "dm/objectnode.h"
#include "slgutils.h"
#include "dm/doc.h"
#include "importctx.h"

//forward declare.
class MD5;
struct aiMaterial;

namespace gme{

///@brief 用于从material出发反向查找name的变量。
/// 在ExtraMaterialManager中建立长期缓冲，本类不再需要。
class  SlgMaterial2Name
{
private:
    std::vector<u_int>      m_matIdx2NameIdx;
    std::vector< std::string >  m_materialNameArray;
public:
    SlgMaterial2Name(void);
    const std::string&      getMaterialName(const slg::Material* pmat)const;
};

class ExtraMaterialManager
{
private:
    friend class DocPrivate;
    ExtraMaterialManager(){}
    ~ExtraMaterialManager(){}

    ///@brief 保存了从材质id到材质名称的map.材质id就是slgname.
    typedef  boost::unordered_map<std::string, std::string>        type_id2name_map;
    /** @brief 保存了从material id 到material name.
    **/
    type_id2name_map                    m_id2name;

    typedef  boost::unordered_map<const slg::Material*,std::string>     type_mat2id;
    type_mat2id                         m_mat2id;
public:
    ///@brief 清空全部数据，用于场景重置(例如加载)。
    void    clear(){
        m_id2name.clear();
        m_mat2id.clear();
    }

    ///@brief 查询及获取方法。返回对应的materialName。
    inline std::string&  get(const std::string &id){
        return m_id2name[id];
    }

    void    appendMat2IdFromSlg(void);
    inline  const std::string&  getMaterialId(const slg::Material* pMat)
    {
        return m_mat2id[pMat];
    }

    inline  void    updateMaterialId(const slg::Material* pMat,const std::string &id)
    {
        m_mat2id[pMat] = id;
    }


    /** @brief recursion remove extrainfo from slg material.
    **/
    void    onMaterialRemoved(const slg::Material* pMat);
    ///@brief 递归更新material及所有子节点的信息。
    void    updateMaterialInfo(const slg::Material *pMat,SlgMaterial2Name &mat2name,SlgTexture2Name &tex2name);

    /** @brief get slg material object from it's id(slgname).
    **/
    static slg::Material* getSlgMaterial(const std::string &id);

    void createMatteMaterial(ImportContext &ctx,const std::string& id,const std::string& name,const std::string &kdpath,const char* emissionPath = NULL,const char* normalPath = NULL);
    void createGrayMaterial(ImportContext &ctx,const std::string& id);
    void createGrayMaterial(slg::Scene *scene,const std::string& id);
    ///@fixme : 使用rapidxml接口来导入导出材质。
    /** @brief 从xmlnode中定义一个材质。如果id未指定，尝试从xmlnode中读入，否则会自动创建一个随机id.
    **/
    void createMaterial(ImportContext &ctx,std::string& id,type_xml_node &xmlnode);
    ///@brief 改进slg缺陷，递归检查一个材质是否是光源。
    static  bool    materialIsLight(const slg::Material *pmat);

    ///@brief fix slg's ToProperties bug.
    const std::string&    dump(luxrays::Properties &prop,const slg::Material* pMat);

    ///@brief 设置材质属性。
    bool    updateMaterial(SlgUtil::UpdateContext &ctx,const slg::Material *pMat,size_t curIdx);

    type_xml_node*   dump(type_xml_node &parent,const slg::Material* pMat,dumpContext &ctx);

    ///@brief 获取指定property keypath的贴图。如果不是一个有效贴图，返回NULL.
    static const slg::Texture*   getTextureFromKeypath(const slg::Material *pMat,const std::vector<std::string> &keyPath,size_t curPost);
private:
    static  bool    isMaterialTypeCompiled(int matType);
    const std::string&    buildDefaultMaterial(SlgUtil::UpdateContext &ctx,const slg::Material *pMat,int type);
    //static  void    createMatteMaterial(ImportContext &ctx,const std::string& id);
};

}

#endif  //GME_DM_SLGMATERIAL_H
