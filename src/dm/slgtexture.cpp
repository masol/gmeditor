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
#include "dm/docmat.h"
#include "slgtexture.h"
#include "docprivate.h"
#include "slg/slg.h"
#include "luxrays/luxrays.h"
#include "luxrays/core/exttrianglemesh.h"
#include <boost/filesystem.hpp>
#include "utils/MD5.h"
#include "slgutils.h"

namespace gme{

SlgTexture2Name::SlgTexture2Name(void)
{
    slg::Scene  *scene = Doc::instance().pDocData->getSession()->renderConfig->scene;
    m_textureNameArray = scene->texDefs.GetTextureNames ();
    m_texIdx2NameIdx.resize(m_textureNameArray.size());
    u_int   nameIdx = 0;
    for(std::vector< std::string >::const_iterator it = m_textureNameArray.begin(); it < m_textureNameArray.end(); ++it,++nameIdx)
    {
        m_texIdx2NameIdx[scene->texDefs.GetTextureIndex(*it)] = nameIdx;
    }
}

const std::string&
SlgTexture2Name::getTextureName(const slg::Texture* ptex)
{
    slg::Scene  *scene = Doc::instance().pDocData->getSession()->renderConfig->scene;
    u_int matNameIdx = m_texIdx2NameIdx[scene->texDefs.GetTextureIndex(ptex)];
    return m_textureNameArray[matNameIdx];
}


void
ExtraTextureManager::loadExtraFromProps(luxrays::Properties &props)
{
    slg::Scene  *scene = Doc::instance().pDocData->getSession()->renderConfig->scene;
    SlgTexture2Name     tex2name;
    for(u_int idx = 0 ; idx < scene->texDefs.GetSize(); idx++)
    {
        slg::Texture* pTex = scene->texDefs.GetTexture(idx);
        if(pTex->GetType() == slg::IMAGEMAP)
        {
            const std::string &slgname = tex2name.getTextureName(pTex);
            std::string path = props.GetString("scene.textures." + slgname + ".file","");
            if(path.length())
            {
                boost::filesystem::path   filepath = boost::filesystem::canonical(path);
                this->addPath(slgname,filepath.string());
            }
        }
    }
}

void
ExtraTextureManager::writeTextureMapping2D(TextureWriteContext &ctx,const slg::TextureMapping2D *ptm2d,MD5 &md5)
{
    std::ostream &o = ctx.stream();
    ctx.outIndent();
    o << "<mapping";

    int type = ptm2d->GetType ();
    md5.update((const unsigned char*)(void*)&type,sizeof(type));
    switch( type )
    {
    case slg::UVMAPPING2D:
        {
            const slg::UVMapping2D    *pRealMapper = dynamic_cast<const slg::UVMapping2D*>(ptm2d);
            o << " type='uvmapping2d'";
            o << " uvscale='" << boost::lexical_cast<std::string>(pRealMapper->uScale) << ' ' << boost::lexical_cast<std::string>(pRealMapper->vScale) << "'";
            o << " uvdelta='" << boost::lexical_cast<std::string>(pRealMapper->uDelta) << ' ' << boost::lexical_cast<std::string>(pRealMapper->vDelta) << "'";
            md5.update((const unsigned char*)(void*)&pRealMapper->uScale,sizeof(pRealMapper->uScale));
            md5.update((const unsigned char*)(void*)&pRealMapper->vScale,sizeof(pRealMapper->vScale));
            md5.update((const unsigned char*)(void*)&pRealMapper->uDelta,sizeof(pRealMapper->uDelta));
            md5.update((const unsigned char*)(void*)&pRealMapper->vDelta,sizeof(pRealMapper->vDelta));
        }
        break;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code");
    }
    o << ">";
    ctx.outIndent();
    o << "</mapping>";
}


void
ExtraTextureManager::writeTextureMapping3D(TextureWriteContext &ctx,const slg::TextureMapping3D *ptm3d,MD5 &md5)
{
    std::ostream &o = ctx.stream();
    ctx.outIndent();
    o << "<mapping";

    int type = ptm3d->GetType ();
    md5.update((const unsigned char*)(void*)&type,sizeof(type));
    switch( type )
    {
    case slg::UVMAPPING3D:
        {
            const slg::UVMapping3D    *pRealMapper = dynamic_cast<const slg::UVMapping3D*>(ptm3d);
            o << " type='uvmapping3d'";
            o << " transformation='";
            SlgUtil::OutputSlgmat(o,pRealMapper->worldToLocal.m);
            o << "'";
        }
        break;
    case slg::GLOBALMAPPING3D:
        {
            const slg::GlobalMapping3D    *pRealMapper = dynamic_cast<const slg::GlobalMapping3D*>(ptm3d);
            o << " type='globalmapping3d'";
            o << " transformation='";
            SlgUtil::OutputSlgmat(o,pRealMapper->worldToLocal.m);
            o << "'";
        }
        break;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code");
    }
    o << ">";
    ctx.outIndent();
    o << "</mapping>";
}

std::string
ExtraTextureManager::getBondnameFromType(slg::MasonryBond type)
{
    switch(type)
    {
    case slg::FLEMISH:
        return "flemish";
    case slg::RUNNING:
        return "running";
    case slg::ENGLISH:
        return "english";
    case slg::HERRINGBONE:
        return "herringbone";
    case slg::BASKET:
        return "basket";
    case slg::KETTING:
        return "chain link";
    }
    BOOST_ASSERT_MSG(false,"unreachable code");
    return "";
}


void
ExtraTextureManager::writeTexture(TextureWriteContext &ctx,const std::string &tag,const slg::Texture *pTex,MD5 *ppmd5)
{
    if(!pTex)
        return;
    std::ostream &o = ctx.stream();
    ctx.outIndent();
    o << "<" << tag;

    MD5 md5;
    std::string texTypeName = DocMat::texGetTypeNameFromType(pTex->GetType());
    o << " type='" << texTypeName << "'";

    /// 在写入过程中，有些texture会有子节点，其子节点的内容输出到cache_stream.
    std::stringstream   cache_stream;
    md5.update(texTypeName.c_str(),texTypeName.length());
    switch(pTex->GetType())
    {
    case slg::CONST_FLOAT3:
        {
            luxrays::Spectrum color = dynamic_cast<const slg::ConstFloat3Texture*>(pTex)->GetColor();
            o << " r='" << boost::lexical_cast<std::string>( color.r ) << "'";
            o << " g='" << boost::lexical_cast<std::string>( color.g ) << "'";
            o << " b='" << boost::lexical_cast<std::string>( color.b ) << "'";
            md5.update((unsigned char*)(void*)&color,sizeof(color));
        }
        break;
    case slg::CONST_FLOAT:
        {
            float value = dynamic_cast<const slg::ConstFloatTexture*>(pTex)->GetValue();
            o << " value='" << boost::lexical_cast<std::string>( value ) << "'";
            md5.update((unsigned char*)(void*)&value,sizeof(value));
        }
        break;
    case slg::IMAGEMAP:
        {
            // 只有imageTexture才需要name.
            const std::string &TextureName = ctx.m_tex2name.getTextureName(pTex);
            o << " name='" << TextureName <<"'";

            const slg::ImageMapTexture*   pImageMap = dynamic_cast<const slg::ImageMapTexture*>(pTex);
            BOOST_ASSERT_MSG(pImageMap,"Texture Type Panic!");
            float gain = pImageMap->GetGain();
            o << " gain='" << boost::lexical_cast<std::string>(gain) << "'";
            md5.update((unsigned char*)(void*)&gain,sizeof(gain));
            const slg::ImageMap   *pMap = pImageMap->GetImageMap();
            float gamma = pMap->GetGamma();
            o << " gamma='" << boost::lexical_cast<std::string>(gamma) << "'";
            md5.update((unsigned char*)(void*)&gamma,sizeof(gamma));
            //开始保存图像。
            MD5             imageCtxMd5;
            const slg::ImageMap   *pImage = pImageMap->GetImageMap();
            imageCtxMd5.update((unsigned char*)(void*)pImage->GetPixels(),pImage->GetWidth() * pImage->GetHeight() * pImage->GetChannelCount() * sizeof(float));
            std::string imageCtxMd5_value =imageCtxMd5.finalize().hexdigest();
            md5.update(imageCtxMd5_value.c_str(),imageCtxMd5_value.length());

            o << " imagemd5='" << imageCtxMd5_value << "'";

            //保存了图像文件的路径。
            std::string     write_file;
            const std::string *pSameCtxPath = ctx.queryFilepath(imageCtxMd5_value);
            if(pSameCtxPath)
            {//文件内容可以被复用。
                write_file = *pSameCtxPath;
            }else{
                //获取原始文件地址。
                const std::string *porigPath = ExtraTextureManager::instance().queryPath(TextureName);
                if(porigPath && porigPath->length())
                {//原始文件被记录。
                    if(ctx.m_bSaveRes)
                    {//保存资源。
                        boost::filesystem::path     origpath(*porigPath);
                        boost::filesystem::path   target = ctx.m_dest_path;
                        target /= origpath.filename();
                        if(boost::filesystem::exists(target))
                        {//目标文件已经存在。添加后缀.
                            std::string extension = target.extension().string();
                            const std::string &fullpath = target.string();
                            std::string stempath = target.string().substr(0,fullpath.length() - extension.length());
                            int suffix = 1;
                            do{
                                target = boost::filesystem::path(stempath + boost::lexical_cast<std::string>(suffix++) + extension);
                            }while(boost::filesystem::exists(target));
                        }
                        //pImage->WriteImage(target.string());
                        //直接拷贝原始文件。
                        boost::filesystem::copy(*porigPath,target);
                        write_file = target.filename().string();
                    }else{//不保存资源，直接保存m_filepath.
                        write_file = *porigPath;
                    }
                }else{//没有定义文件名。此时直接保存资源。为了防止丢失信息，保存为hdr格式。
                    boost::filesystem::path target_model = ctx.m_dest_path / "image%%%%%%.exr";
                    boost::filesystem::path target = boost::filesystem::unique_path(target_model);
                    pImage->WriteImage(target.string());
                    write_file = target.filename().string();
                }
                ctx.m_ctx2filepath[imageCtxMd5_value] = write_file;
            }

            o << " file='" << write_file << "'";

            //输出TextureMapping2D.
            {
                TextureWriteContextSetter   textCtxAssign(ctx,cache_stream,ctx.m_indent+1);
                writeTextureMapping2D(ctx,pImageMap->GetTextureMapping(),md5);
            }
        }
        break;
    case slg::SCALE_TEX:
        {
            const slg::ScaleTexture*   pRealTex = dynamic_cast<const slg::ScaleTexture*>(pTex);
            TextureWriteContextSetter   textCtxAssign(ctx,cache_stream,ctx.m_indent+1);
            writeTexture(ctx,"texture1",pRealTex->GetTexture1(),&md5);
            writeTexture(ctx,"texture2",pRealTex->GetTexture2(),&md5);
        }
        break;
    case slg::FRESNEL_APPROX_N:
        {
            const slg::FresnelApproxNTexture*   pRealTex = dynamic_cast<const slg::FresnelApproxNTexture*>(pTex);
            TextureWriteContextSetter   textCtxAssign(ctx,cache_stream,ctx.m_indent+1);
            writeTexture(ctx,"texture",pRealTex->GetTexture(),&md5);
        }
        break;
    case slg::FRESNEL_APPROX_K:
        {
            const slg::FresnelApproxKTexture*   pRealTex = dynamic_cast<const slg::FresnelApproxKTexture*>(pTex);
            TextureWriteContextSetter   textCtxAssign(ctx,cache_stream,ctx.m_indent+1);
            writeTexture(ctx,"texture",pRealTex->GetTexture(),&md5);
        }
        break;
    case slg::MIX_TEX:
        {
            const slg::MixTexture*   pRealTex = dynamic_cast<const slg::MixTexture*>(pTex);
            TextureWriteContextSetter   textCtxAssign(ctx,cache_stream,ctx.m_indent+1);
            writeTexture(ctx,"texture1",pRealTex->GetTexture1(),&md5);
            writeTexture(ctx,"texture2",pRealTex->GetTexture2(),&md5);
            writeTexture(ctx,"amount",pRealTex->GetAmountTexture(),&md5);
        }
        break;
    case slg::ADD_TEX:
        {
            const slg::AddTexture*   pRealTex = dynamic_cast<const slg::AddTexture*>(pTex);
            TextureWriteContextSetter   textCtxAssign(ctx,cache_stream,ctx.m_indent+1);
            writeTexture(ctx,"texture1",pRealTex->GetTexture1(),&md5);
            writeTexture(ctx,"texture2",pRealTex->GetTexture2(),&md5);
        }
        break;
    case slg::CHECKERBOARD2D:
        {
            const slg::CheckerBoard2DTexture*   pRealTex = dynamic_cast<const slg::CheckerBoard2DTexture*>(pTex);
            TextureWriteContextSetter   textCtxAssign(ctx,cache_stream,ctx.m_indent+1);
            writeTexture(ctx,"texture1",pRealTex->GetTexture1(),&md5);
            writeTexture(ctx,"texture2",pRealTex->GetTexture2(),&md5);
            writeTextureMapping2D(ctx,pRealTex->GetTextureMapping(),md5);
        }
        break;
    case slg::CHECKERBOARD3D:
        {
            const slg::CheckerBoard3DTexture*   pRealTex = dynamic_cast<const slg::CheckerBoard3DTexture*>(pTex);
            TextureWriteContextSetter   textCtxAssign(ctx,cache_stream,ctx.m_indent+1);
            writeTexture(ctx,"texture1",pRealTex->GetTexture1(),&md5);
            writeTexture(ctx,"texture2",pRealTex->GetTexture2(),&md5);
            writeTextureMapping3D(ctx,pRealTex->GetTextureMapping(),md5);
        }
        break;
    case slg::FBM_TEX:
        {
            const slg::FBMTexture*   pRealTex = dynamic_cast<const slg::FBMTexture*>(pTex);
            o << " octaves='" << pRealTex->GetOctaves() << "'";
            o << " roughness='" << pRealTex->GetOmega() << "'";
            TextureWriteContextSetter   textCtxAssign(ctx,cache_stream,ctx.m_indent+1);
            writeTextureMapping3D(ctx,pRealTex->GetTextureMapping(),md5);
        }
        break;
    case slg::MARBLE:
        {
            const slg::MarbleTexture*   pRealTex = dynamic_cast<const slg::MarbleTexture*>(pTex);
            o << " octaves='" << pRealTex->GetOctaves() << "'";
            o << " roughness='" << pRealTex->GetOmega() << "'";
            o << " scale='" << pRealTex->GetScale() << "'";
            o << " variation='" << pRealTex->GetVariation() << "'";
            TextureWriteContextSetter   textCtxAssign(ctx,cache_stream,ctx.m_indent+1);
            writeTextureMapping3D(ctx,pRealTex->GetTextureMapping(),md5);
        }
        break;
    case slg::DOTS:
        {
            const slg::DotsTexture*   pRealTex = dynamic_cast<const slg::DotsTexture*>(pTex);
            TextureWriteContextSetter   textCtxAssign(ctx,cache_stream,ctx.m_indent+1);
            writeTexture(ctx,"inside",pRealTex->GetInsideTex(),&md5);
            writeTexture(ctx,"outside",pRealTex->GetOutsideTex(),&md5);
            writeTextureMapping2D(ctx,pRealTex->GetTextureMapping(),md5);
        }
        break;
    case slg::BRICK:
        {
            const slg::BrickTexture*   pRealTex = dynamic_cast<const slg::BrickTexture*>(pTex);
            o << " brickbond='" << getBondnameFromType(pRealTex->GetBond()) << "'";
            o << " brickwidth='" << pRealTex->GetBrickWidth() << "'";
            o << " brickheight='" << pRealTex->GetBrickHeight() << "'";
            o << " brickdepth='" << pRealTex->GetBrickDepth() << "'";
            o << " mortarsize='" << pRealTex->GetMortarSize() << "'";
            o << " brickrun='" << pRealTex->GetRun() << "'";
            o << " brickbevel='" << pRealTex->GetBevelWidth() / pRealTex->GetBrickWidth() << "'";

            TextureWriteContextSetter   textCtxAssign(ctx,cache_stream,ctx.m_indent+1);
            writeTexture(ctx,"bricktex",pRealTex->GetTexture1(),&md5);
            writeTexture(ctx,"mortartex",pRealTex->GetTexture2(),&md5);
            writeTexture(ctx,"brickmodtex",pRealTex->GetTexture3(),&md5);
            writeTextureMapping3D(ctx,pRealTex->GetTextureMapping(),md5);
        }
        break;
    case slg::WINDY:
        {
            const slg::WindyTexture*   pRealTex = dynamic_cast<const slg::WindyTexture*>(pTex);
            TextureWriteContextSetter   textCtxAssign(ctx,cache_stream,ctx.m_indent+1);
            writeTextureMapping3D(ctx,pRealTex->GetTextureMapping(),md5);
        }
        break;
    case slg::WRINKLED:
        {
            const slg::WrinkledTexture*   pRealTex = dynamic_cast<const slg::WrinkledTexture*>(pTex);
            o << " octaves='" << pRealTex->GetOctaves() << "'";
            o << " roughness='" << pRealTex->GetOmega() << "'";
            TextureWriteContextSetter   textCtxAssign(ctx,cache_stream,ctx.m_indent+1);
            writeTextureMapping3D(ctx,pRealTex->GetTextureMapping(),md5);
        }
        break;
    case slg::UV_TEX:
        {
            const slg::UVTexture*   pRealTex = dynamic_cast<const slg::UVTexture*>(pTex);
            TextureWriteContextSetter   textCtxAssign(ctx,cache_stream,ctx.m_indent+1);
            writeTextureMapping2D(ctx,pRealTex->GetTextureMapping(),md5);
        }
        break;
    case slg::BAND_TEX:
        {
            const slg::BandTexture*   pRealTex = dynamic_cast<const slg::BandTexture*>(pTex);
            const std::vector< float > &offsets =pRealTex->GetOffsets();
            const std::vector< luxrays::Spectrum > &values = pRealTex->GetValues();
            for(size_t idx = 0; idx < offsets.size(); idx++)
            {
                o << " offset" << idx << "='" << offsets[idx] << "'";
                o << " value" << idx << "='" << values[idx].r << ' ' << values[idx].g << ' ' << values[idx].b << "'";
            }
            TextureWriteContextSetter   textCtxAssign(ctx,cache_stream,ctx.m_indent+1);
            writeTexture(ctx,"amount",pRealTex->GetAmountTexture(),&md5);
        }
        break;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code");
    }

    std::string     md5_value = md5.finalize().hexdigest();
    o << " ctxmd5='" << md5_value << "'>" << std::endl;

    o << cache_stream.str();

    ctx.outIndent();
    o << "</" << tag << ">" << std::endl;

    //我们不在这里保存md5到texture的映射，这个工作交给加载来做。

    if(ppmd5)
    {
        ppmd5->update(md5_value.c_str(),md5_value.length());
    }
}


}
