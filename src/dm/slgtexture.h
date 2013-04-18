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

class MD5;

namespace gme{

struct  TextureWriteContext{
    TextureWriteContext(bool bExportRes,
            const boost::filesystem::path& p,
            std::ostream &o,
            int indent)
        : m_bSaveRes(bExportRes),
          m_dest_path(p),
          m_stream(o),
          m_indent(indent)
    {}
    ~TextureWriteContext(){}
protected:
	inline	void outIndent(std::ostream &o)
	{
		for(int indent = 0; indent < this->m_indent; indent++)
			o << "  ";
	}
    friend  class ExtraTextureManager;
    const boost::filesystem::path m_dest_path;
    const bool      m_bSaveRes;
    std::ostream   &m_stream;
    int             m_indent;
};


/** @brief 本类保存了额外的贴图信息，可以看作对slg texture基类的信息添加。
**/
struct  ExtraTexture
{
protected:
    std::string     m_slgname;
    //如果从spolo scene加载，m_filename保存了原始文件名。
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

class ExtraTextureManager : public Singleton<ExtraTextureManager>
{
private:
    friend class Singleton<ExtraTextureManager>;
    typedef Singleton<ExtraTextureManager>   inherited;
    ExtraTextureManager(){}

public:
    ~ExtraTextureManager(){}
    ///@brief 清空全部数据，用于场景重置(例如加载)。
    void    clear(){
    }
public:
    static void    writeTexture(TextureWriteContext &ctx,const std::string &tag,const slg::Texture *pTex,MD5 *ppmd5);
};

}

#endif  //GME_DM_SLGTEXTURE_H
