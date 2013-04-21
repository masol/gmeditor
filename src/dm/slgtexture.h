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
#include <string>
#include <ostream>
#include <boost/filesystem.hpp>
#include <boost/unordered_map.hpp>

class MD5;

namespace gme{

/** @brief 为了弥补slg中名称保存机制的辅助类。
**/
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


struct  TextureWriteContext{
    TextureWriteContext(bool bExportRes,
            const boost::filesystem::path& p,
            std::ostream &o,
            int indent)
        : m_dest_path(p),
          m_bSaveRes(bExportRes),
          m_pstream(&o),
          m_indent(indent)
    {}
    ~TextureWriteContext(){}
public:
    inline std::ostream& stream()
    {
        return *m_pstream;
    }
protected:
	inline	void outIndent()
	{
		for(int indent = 0; indent < this->m_indent; indent++)
			stream() << "  ";
	}
    friend  class ExtraTextureManager;
    const boost::filesystem::path m_dest_path;
    const bool      m_bSaveRes;
    std::ostream   *m_pstream;
    int             m_indent;
    SlgTexture2Name m_tex2name;

    /** @brief 保存了图像加载后内容的md5string到文件名的映射。
    **/
    typedef   boost::unordered_map<std::string, std::string>        type_ctx2filepath;
    type_ctx2filepath       m_ctx2filepath;

    const std::string*  queryFilepath(const std::string &ctxmd5)
    {
        type_ctx2filepath::iterator it = m_ctx2filepath.find(ctxmd5);
        if(it != m_ctx2filepath.end())
        {
            return &(it->second);
        }
        return NULL;
    }
private:
    friend class TextureWriteContextSetter;
};

///@brief 通过本setter来设置stream,可以在脱离作用域时恢复原始stream.
class  TextureWriteContextSetter
{
private:
    TextureWriteContext     &m_ctx;
    std::ostream            *m_oldStream;
    int                     m_oldIndent;
public:
    TextureWriteContextSetter(TextureWriteContext &ctx,std::ostream &o,int indent) : m_ctx(ctx)
    {
        m_oldStream = m_ctx.m_pstream;
        m_oldIndent = m_ctx.m_indent;
        m_ctx.m_pstream = &o;
        m_ctx.m_indent = indent;
    }
    ~TextureWriteContextSetter()
    {
        m_ctx.m_pstream = m_oldStream;
        m_ctx.m_indent = m_oldIndent;
    }
};


/** @brief 本类保存了额外的贴图信息，可以看作对slg texture基类的信息添加。
**/
struct  ExtraTexture
{
protected:
    std::string     m_slgname;
    /// @brief 如果贴图是image类型，m_filename保存了原始文件名。
    std::string     m_filename;
    ExtraTexture(){
    }
public:
    ExtraTexture(const ExtraTexture& ref){
        AssignFrom(ref);
    }
    ~ExtraTexture(){
    }
    ExtraTexture&   operator=(const ExtraTexture& ref)
    {
        AssignFrom(ref);
        return *this;
    }
private:
    void    AssignFrom(const ExtraTexture& ref){
        this->m_slgname = ref.m_slgname;
        this->m_filename = ref.m_filename;
    }
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
public:
    ///@brief 清空全部数据，用于场景重置(例如加载)。
    void    clear(){
    }
    /** @brief 添加一个路径映射。
    **/
    inline void    addPath(const std::string& slgname,const std::string &path)
    {
        m_slgname2filepath_map[slgname] = path;
    }
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
public:
    static std::string getBondnameFromType(slg::MasonryBond type);

    static void    writeTexture(TextureWriteContext &ctx,const std::string &tag,const slg::Texture *pTex,MD5 *ppmd5);
private:
    static void    writeTextureMapping2D(TextureWriteContext &ctx,const slg::TextureMapping2D *ptm2d,MD5 &md5);
    static void    writeTextureMapping3D(TextureWriteContext &ctx,const slg::TextureMapping3D *ptm3d,MD5 &md5);
};

}

#endif  //GME_DM_SLGTEXTURE_H
