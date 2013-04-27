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
#include "dm/docmat.h"
#include "slg/slg.h"
#include "slg/sdl/material.h"
#include "docprivate.h"
#include "slgmaterial.h"
#include <boost/assert.hpp>

namespace gme{
//texture
const   int  CONST_FLOAT = slg::CONST_FLOAT;
const   int  CONST_FLOAT3 = slg::CONST_FLOAT3;
const   int  IMAGEMAP = slg::IMAGEMAP;
const   int  SCALE_TEX = slg::SCALE_TEX;
const   int  FRESNEL_APPROX_N = slg::FRESNEL_APPROX_N;
const   int  FRESNEL_APPROX_K = slg::FRESNEL_APPROX_K;
const   int  MIX_TEX = slg::MIX_TEX;
const   int  ADD_TEX = slg::ADD_TEX;
const   int  CHECKERBOARD2D = slg::CHECKERBOARD2D;
const   int  CHECKERBOARD3D = slg::CHECKERBOARD3D;
const   int  FBM_TEX = slg::FBM_TEX;
const   int  MARBLE = slg::MARBLE;
const   int  DOTS = slg::DOTS;
const   int  BRICK = slg::BRICK;
const   int  WINDY = slg::WINDY;
const   int  WRINKLED = slg::WRINKLED;
const   int  UV_TEX = slg::UV_TEX;
const   int  BAND_TEX = slg::BAND_TEX;

std::string
DocMat::texGetTypeNameFromType(int type)
{
    switch(type)
    {
    case slg::CONST_FLOAT:
        return "constfloat1";
    case slg::CONST_FLOAT3:
        return "constfloat3";
    case slg::IMAGEMAP:
        return "imagemap";
    case slg::SCALE_TEX:
        return "scale";
    case slg::FRESNEL_APPROX_N:
        return "fresnelapproxn";
    case slg::FRESNEL_APPROX_K:
        return "fresnelapproxk";
    case slg::MIX_TEX:
        return "mix";
    case slg::ADD_TEX:
        return "add";
    case slg::CHECKERBOARD2D:
        return "checkerboard2d";
    case slg::CHECKERBOARD3D:
        return "checkerboard3d";
    case slg::FBM_TEX:
        return "fbm";
    case slg::MARBLE:
        return "marble";
    case slg::DOTS:
        return "dots";
    case slg::BRICK:
        return "brick";
    case slg::WINDY:
        return "windy";
    case slg::WRINKLED:
        return "wrinkled";
    case slg::UV_TEX:
        return "uv";
    case slg::BAND_TEX:
        return "band";
    }
    BOOST_ASSERT_MSG(false,"unreachable code!");
    return "";
}

int
DocMat::texGetTypeFromTypeName(const std::string &name)
{
    if(name == "constfloat1")
    {
        return slg::CONST_FLOAT;
    }else if(name == "constfloat3")
    {
        return slg::CONST_FLOAT3;
    }else if(name == "imagemap")
    {
        return slg::IMAGEMAP;
    }else if(name == "scale")
    {
        return slg::SCALE_TEX;
    }else if(name == "fresnelapproxn")
    {
        return slg::FRESNEL_APPROX_N;
    }else if(name == "fresnelapproxk")
    {
        return slg::FRESNEL_APPROX_K;
    }else if(name == "mix")
    {
        return slg::MIX_TEX;
    }else if(name == "add")
    {
        return slg::ADD_TEX;
    }else if(name == "checkerboard2d")
    {
        return slg::CHECKERBOARD2D;
    }else if(name == "checkerboard3d")
    {
        return slg::CHECKERBOARD3D;
    }else if(name == "fbm")
    {
        return slg::FBM_TEX;
    }else if(name == "marble")
    {
        return slg::MARBLE;
    }else if(name == "dots")
    {
        return slg::DOTS;
    }else if(name == "brick")
    {
        return slg::BRICK;
    }else if(name == "windy")
    {
        return slg::WINDY;
    }else if(name == "wrinkled")
    {
        return slg::WRINKLED;
    }else if(name == "uv")
    {
        return slg::UV_TEX;
    }else if(name == "band")
    {
        return slg::BAND_TEX;
    }
    //BOOST_ASSERT_MSG(false,"unreachable code!");
    return INVALID_TEXTURE;
}



//material
const   int  DocMat::MATTE = slg::MATTE;
const   int  DocMat::MIRROR = slg::MIRROR;
const   int  DocMat::GLASS = slg::GLASS;
const   int  DocMat::METAL = slg::METAL;
const   int  DocMat::ARCHGLASS = slg::ARCHGLASS;
const   int  DocMat::MIX = slg::MIX;
const   int  DocMat::NULLMAT = slg::NULLMAT;
const   int  DocMat::MATTETRANSLUCENT = slg::MATTETRANSLUCENT;
const   int  DocMat::GLOSSY2 = slg::GLOSSY2;
const   int  DocMat::METAL2 = slg::METAL2;
//const   int  DocMat::GLOSSY2_ANISOTROPIC = slg::GLOSSY2_ANISOTROPIC;
//const   int  DocMat::GLOSSY2_ABSORPTION = slg::GLOSSY2_ABSORPTION;
//const   int  DocMat::GLOSSY2_INDEX = slg::GLOSSY2_INDEX;
//const   int  DocMat::GLOSSY2_MULTIBOUNCE = slg::GLOSSY2_MULTIBOUNCE;
//const   int  DocMat::METAL2_ANISOTROPIC = slg::METAL2_ANISOTROPIC;


std::string
DocMat::getTypeNameFromType(int type)
{
    switch(type)
    {
    case MATTE:
        return "matte";
    case MIRROR:
        return "mirror";
    case GLASS:
        return "glass";
    case METAL:
        return "metal";
    case ARCHGLASS:
        return "archglass";
    case MIX:
        return "mix";
    case NULLMAT:
        return "null";
    case MATTETRANSLUCENT:
        return "mattetranslucent";
    case GLOSSY2:
        return "glossy2";
    case METAL2:
        return "metal2";
    }
    BOOST_ASSERT_MSG(false,"unreachable code!");
    return "";
}

int
DocMat::getTypeFromTypeName(const std::string &name)
{
    if(name == "matte")
    {
        return MATTE;
    }else if(name == "mirror")
    {
        return MIRROR;
    }else if(name == "glass")
    {
        return GLASS;
    }else if(name == "metal")
    {
        return METAL;
    }else if(name == "archglass")
    {
        return ARCHGLASS;
    }else if(name == "mix")
    {
        return MIX;
    }else if(name == "null")
    {
        return NULLMAT;
    }else if(name == "mattetranslucent")
    {
        return MATTETRANSLUCENT;
    }else if(name == "glossy2")
    {
        return GLOSSY2;
    }else if(name == "metal2")
    {
        return METAL2;
    }
    //BOOST_ASSERT_MSG(false,"unreachable code!");
    return INVALID_MATERIAL;
}


std::string&
DocMat::getMatName(const std::string& id)
{
    return pDocData->matManager.get(id);
}

int
DocMat::getMatType(const std::string& id)
{
    return 0;
}

bool
DocMat::getMatProperty(const std::string& id,const std::string &prop,std::string &value)
{
    return false;
}

bool
DocMat::setMatProperty(const std::string& id,const std::string &prop,const std::string &value)
{
    return false;
}

bool
DocMat::setMaterial(const std::string& id,const type_xml_node &matdef)
{
	return false;
}

type_xml_node*
DocMat::getMaterial(const std::string &id,type_xml_node &parent)
{
    slg::Material* pMat = ExtraMaterialManager::getSlgMaterial(id);
    if(pMat)
    {
        dumpContext   ctx(dumpContext::DUMP_NORMAL,boost::filesystem::current_path());
        return pDocData->matManager.dump(parent,pMat,ctx);
    }
    return NULL;
}


} //end namespace gme.
