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

#ifndef  GME_DM_DOCMAT_H
#define  GME_DM_DOCMAT_H


#include "dm/doc.h"
#include "dm/xmlutil.h"
#include <string>

namespace gme{

/** @brief 材质操作接口。
 * @notice 为了简化数据流，我们不使用boost::property_tree.而是使用luxrays::Properties作为数据交换。
 * @todo how to resolve dependence if we use luxrays::Properties?
**/
class DocMat : public DocScopeLocker
{
public: //texture.
    static  const   int  CONST_FLOAT;
    static  const   int  CONST_FLOAT3;
    static  const   int  IMAGEMAP;
    static  const   int  SCALE_TEX;
    static  const   int  FRESNEL_APPROX_N;
    static  const   int  FRESNEL_APPROX_K;
    static  const   int  MIX_TEX;
    static  const   int  ADD_TEX;
    static  const   int  CHECKERBOARD2D;
    static  const   int  CHECKERBOARD3D;
    static  const   int  FBM_TEX;
    static  const   int  MARBLE;
    static  const   int  DOTS;
    static  const   int  BRICK;
    static  const   int  WINDY;
    static  const   int  WRINKLED;
    static  const   int  UV_TEX;
    static  const   int  BAND_TEX;
    static  std::string  texGetTypeNameFromType(int type);
    static  int   texGetTypeFromTypeName(const std::string &name);
public: //material.
    static  const   int  MATTE;
    static  const   int  MIRROR;
    static  const   int  GLASS;
    static  const   int  METAL;
    static  const   int  ARCHGLASS;
    static  const   int  MIX;
    static  const   int  NULLMAT;
    static  const   int  MATTETRANSLUCENT;
    static  const   int  GLOSSY2;
    static  const   int  METAL2;
//    static  const   int  GLOSSY2_ANISOTROPIC;
//    static  const   int  GLOSSY2_ABSORPTION;
//    static  const   int  GLOSSY2_INDEX;
//    static  const   int  GLOSSY2_MULTIBOUNCE;
//    static  const   int  METAL2_ANISOTROPIC;
    static  std::string  getTypeNameFromType(int type);
    static  int   getTypeFromTypeName(const std::string &name);
public:
    std::string& getMatName(const std::string& id);
    int         getMatType(const std::string& id);
    bool        getMatProperty(const std::string& id,const std::string &prop,std::string &value);
    bool        setMatProperty(const std::string& id,const std::string &prop,const std::string &value);
    bool        setMaterial(const std::string &id,const type_xml_node &matdef);
    //no used.
//    std::string addMaterial(const type_xml_node &matdef);
    /**@brief 获取模型描述信息。将自动添加到parent下。parent必须有doc对象。
      * @return self.
    **/
    type_xml_node*   getMaterial(const std::string &id,type_xml_node &parent);
};

}

#endif //GME_DM_DOCMAT_H

