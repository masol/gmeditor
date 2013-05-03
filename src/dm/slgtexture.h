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

#ifndef  GME_DM_SLGTEXTURE_H
#define  GME_DM_SLGTEXTURE_H

#include "utils/singleton.h"
#include "slg/slg.h"
#include "slg/sdl/texture.h"
#include "slgutils.h"
#include <string>
#include <ostream>
#include <boost/filesystem.hpp>
#include <boost/unordered_map.hpp>
#include "dm/xmlutil.h"
#include "importctx.h"

class MD5;

namespace gme{

///** @brief 为了弥补slg中名称保存机制的辅助类。
///不再需要，被ExtraTextureManager内部维护。
//**/
class  SlgTexture2Name
{
private:
    ///@brief 用于从material出发反向查找name的变量。
    ///@todo: 是否有必要在ExtraMaterialManager中建立长期缓冲？
    std::vector<u_int>      m_texIdx2NameIdx;
    std::vector< std::string >  m_textureNameArray;
public:
    SlgTexture2Name(void);
    const std::string&      getTextureName(const slg::Texture* ptex);
};


class ExtraTextureManager
{
private:
    friend class DocPrivate;
    ExtraTextureManager(){}
    ~ExtraTextureManager(){}

    typedef   boost::unordered_map<std::string, std::string>        type_slgname2filepath;
    /** @brief 保存了imagetype类型的Texture从slgname到全路径的映射。
    **/
    type_slgname2filepath               m_slgname2filepath_map;

    /** @brief 为了便于处理，我们废弃使用slg的texture id pair set.而是在这里构建缓冲。
    **/
    typedef   boost::unordered_map<const slg::Texture*, std::string>      type_texture2id;
    type_texture2id           m_tex2id;

public:
    ///@brief 清空全部数据，用于场景重置(例如加载)。
    inline void    clear()
    {
        m_slgname2filepath_map.clear();
        m_tex2id.clear();
    }

    inline const std::string&   getTextureId(const slg::Texture *pTex)
    {
        return m_tex2id[pTex];
    }
    static  slg::Texture*   getSlgTexture(const std::string &id);


    /** @brief recursion remove extrainfo from slg material.
    **/
    void    onTextureRemoved(const slg::Texture *pTex);
    void    updateTextureInfo(const slg::Texture *pTex,SlgTexture2Name &tex2name);

    /** @brief 从当前场景构建tex到name的映射表。只有在加载cfg时需要构建一次，之后会被gmeditor处理。
    **/
    void    appendTex2IdFromSlg();

    /** @brief 查询路径映射是否存在。
    **/
    inline const std::string*  queryPath(const std::string &slgname)
    {
        type_slgname2filepath::iterator it = m_slgname2filepath_map.find(slgname);
        if(it != m_slgname2filepath_map.end())
        {
            return &(it->second);
        }
        return NULL;
    }

    void    loadExtraFromProps(luxrays::Properties &props);

    /** @brief fix bug for slg::Texture::ToProperties.
    **/
    static std::string dump(luxrays::Properties &prop,const slg::Texture* pTex);

    type_xml_node*  dump(type_xml_node &parent,const std::string &tag,const slg::Texture *pTex,dumpContext &ctx);
    inline type_xml_node*  dump(type_xml_node &parent,const std::string &tag,const slg::Texture *pTex,dumpContext &ctx,conditional_md5 &md5)
	{
        type_xml_node *pChild = dump(parent,tag,pTex,ctx);
        md5.updateChild(pChild);
		return pChild;
	}
	std::string    createTexture(ImportContext &ctx,type_xml_node &self);
public:
    static std::string getBondnameFromType(slg::MasonryBond type);
    bool defineImageMapTexture(ImportContext &ctx,const std::string &src,std::string &id);
    ///@brief 更新贴图属性。返回贴图id.
    std::string    updateTexture(SlgUtil::UpdateContext &ctx,const slg::Texture *pTex,size_t curIdx);
    static const slg::Texture*  getTextureFromKeypath(const slg::Texture *pTex,const std::vector<std::string> &keyPath,size_t curIdx);
private:
    std::string buildDefaultTexture(SlgUtil::UpdateContext &ctx,const slg::Texture *pTex,int type);
    static  inline bool ImageMapTexture_isGainDefault(float gain)
    {
        return (gain == 1.0f);
    }
    static  inline bool ImageMapTexture_isGammaDefault(float gamma)
    {
        return (gamma == 2.2f);
    }

    inline const std::string&  defineAndUpdate(const std::string &id,slg::Scene *scene,const std::string &sceneDef)
    {
        scene->DefineTextures(sceneDef);
        m_tex2id[scene->texDefs.GetTexture(id)] = id;
        return id;
    }
    static void  importTextureMapping2D(std::ostream &o,type_xml_node &self,const std::string &id);
    static void  importTextureMapping3D(std::ostream &o,type_xml_node &self,const std::string &id);
    static type_xml_node*   dumpTextureMapping2D(type_xml_node &parent,const slg::TextureMapping2D *ptm2d,dumpContext &ctx);
    static type_xml_node*   dumpTextureMapping3D(type_xml_node &parent,const slg::TextureMapping3D *ptm3d,dumpContext &ctx);

    /** @brief 添加一个路径映射。
    **/
    inline void    addPath(const std::string& slgname,const std::string &path)
    {
        m_slgname2filepath_map[slgname] = path;
    }
};

}

#endif  //GME_DM_SLGTEXTURE_H
