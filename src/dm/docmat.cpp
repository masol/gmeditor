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
    BOOST_ASSERT_MSG(false,"unreachable code!");
    return -1;
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
    BOOST_ASSERT_MSG(false,"unreachable code!");
    return -1;
}


std::string
DocMat::getMatName(const boost::uuids::uuid& id)
{
    ExtraMaterial*  pmatInfo = pDocData->matManager.query(id);
    if(pmatInfo)
    {
        return pmatInfo->slgname();
    }
    return ObjectNode::idto_string(id);
}

int
DocMat::getMatType(const boost::uuids::uuid& id)
{
    return 0;
}

boost::any
DocMat::getMatProperty(const boost::uuids::uuid& id,const std::string &prop)
{
    return boost::any();
}

bool
DocMat::setMatProperty(const boost::uuids::uuid& id,const std::string &prop,boost::any &value)
{
    return false;
}

void
DocMat::setMaterial(const boost::uuids::uuid& id,type_material_def &matdef)
{
}

boost::uuids::uuid
DocMat::addMaterial(type_material_def &matdef)
{
    return boost::uuids::uuid();
}



} //end namespace gme.
