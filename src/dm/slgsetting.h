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

#ifndef  GME_DM_SLGSETTING_H
#define  GME_DM_SLGSETTING_H

#include "slg/slg.h"
#include "slg/rendersession.h"
#include "slgutils.h"
#include "importctx.h"


namespace gme{

class ExtraSettingManager{
public:
    static void dumpLights(type_xml_node &parent,dumpContext &ctx);
    static void dumpSettings(type_xml_node &parent,dumpContext &ctx);
    static void loadSettings(ImportContext &ctx,type_xml_node &parents);
    static void createLights(ImportContext &ctx,type_xml_node &parents);
    static const std::string&  getImageMapPath(const slg::ImageMapCache &imcache,const slg::ImageMap *im);
    ///@brief 指示场景中是否有超过两个(含2个)灯光。
    static bool hasTwoOrMoreLighter(slg::Scene *scene);
    ///@brief 返回场景中的光源总数。
    static int getLighterNumber(slg::Scene *scene);
private:
    static std::string  nameFromFiltertype(int filter);
    static std::string  nameFromRenderengine(int enginetype);
    static bool isDefault_turbidity(float tur)
    {
        return (tur == 2.2f);
    }
    static bool isDefault_relSize(float rel)
    {
        return (rel == 1.0f);
    }
};

}

#endif //GME_DM_SLGSETTING_H
