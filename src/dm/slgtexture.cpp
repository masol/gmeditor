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
#include "slg/slg.h"
#include "luxrays/luxrays.h"
#include "luxrays/core/exttrianglemesh.h"
#include <boost/filesystem.hpp>
#include "utils/MD5.h"

namespace gme{

void
ExtraTextureManager::writeTexture(TextureWriteContext &ctx,const std::string &tag,const slg::Texture *pTex,MD5 *ppmd5)
{
    std::ostream &o = ctx.m_stream;
    ctx.outIndent(o);
    o << "<" << tag;

    MD5 md5;
    std::string texTypeName = DocMat::texGetTypeNameFromType(pTex->GetType());
    o << " type='" << texTypeName << "'";

    md5.update(texTypeName.c_str(),texTypeName.length());
    switch(pTex->GetType())
    {
    case slg::CONST_FLOAT3:
        {
            slg::Spectrum color = dynamic_cast<const slg::ConstFloat3Texture*>(pTex)->GetColor();
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
            const slg::ImageMapTexture*   pImageMap = dynamic_cast<const slg::ImageMapTexture*>(pTex);
            BOOST_ASSERT_MSG(pImageMap,"Texture Type Panic!");
            float gain = pImageMap->GetGain();
            o << " gain='" << boost::lexical_cast<std::string>(gain) << "'";
            md5.update((unsigned char*)(void*)&gain,sizeof(gain));
            const slg::ImageMap   *pMap = pImageMap->GetImageMap();
            float gamma = pMap->GetGamma();
            o << " gamma='" << boost::lexical_cast<std::string>(gamma) << "'";
            md5.update((unsigned char*)(void*)&gamma,sizeof(gamma));
        }
        break;
    case slg::SCALE_TEX:
        {
        }
        break;
    case slg::FRESNEL_APPROX_N:
        {
        }
        break;
    case slg::FRESNEL_APPROX_K:
        {
        }
        break;
    case slg::MIX_TEX:
        {
        }
        break;
    case slg::ADD_TEX:
        {
        }
        break;
    case slg::CHECKERBOARD2D:
        {
        }
        break;
    case slg::CHECKERBOARD3D:
        {
        }
        break;
    case slg::FBM_TEX:
        {
        }
        break;
    case slg::MARBLE:
        {
        }
        break;
    case slg::DOTS:
        {
        }
        break;
    case slg::BRICK:
        {
        }
        break;
    case slg::WINDY:
        {
        }
        break;
    case slg::WRINKLED:
        {
        }
        break;
    case slg::UV_TEX:
        {
        }
        break;
    case slg::BAND_TEX:
        {
        }
        break;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code");
    }

    std::string     md5_value = md5.finalize().hexdigest();
    o << " ctxmd5='" << md5_value << "'>" << std::endl;
    ctx.outIndent(o);
    o << "</" << tag << ">" << std::endl;

    if(ppmd5)
    {
        ppmd5->update(md5_value.c_str(),md5_value.length());
    }
}


}
