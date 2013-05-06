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

#ifndef  GME_DM_DOCSETTING_H
#define  GME_DM_DOCSETTING_H

#include "dm/doc.h"
#include "dm/xmlutil.h"
#include <boost/unordered_map.hpp>
#include "dm/docmat.h"
#include "luxrays/luxrays.h"
#include "luxrays/core/spectrum.h"

namespace slg
{
    class Scene;
}

namespace gme
{

/** @brief 动态配置接口。可以动态配置当前文档的各类信息。const information please see class Setting.
**/
class DocSetting : public DocScopeLocker
{
public:
    //film 接口:
	type_xml_node*   getToneMap(type_xml_node &parent);

	std::string   getToneMapTypeNameByName(int type);
	bool   getLinearScale(float &ls);
	bool   setLinearScale(float ls);
    //engine 接口:
    int    getEngineType(void);
    void   setEngineType(int type);
    int    getAccelType(void);
    void   setAccelType(int type);

    //light(evnlight and sunlight) interface:
    //void  getEnvLight(type_xml_node &parent);
    bool  changeHDRfile(const std::string &fullpath);
    bool  changeEnvGain(const luxrays::Spectrum &g);
    bool  changeEnvUDelta(float d);
    bool  changeEnvVDelta(float d);
    bool  changeSkyEnv(void);
    bool  changeSkyDir(const luxrays::Vector &dir);
    bool  changeSkyTurbidity(float t);
    bool  disableEnv(void);
    bool  disableSun(void);
    bool  enableSun(void);
    bool  changeSunDir(const luxrays::Vector &dir);
    bool  changeSunGain(const luxrays::Spectrum &g);
    bool  changeSunTurbidity(float t);
    bool  changeSunRelsize(float t);
    
    slg::Scene* getScene(void);
	const std::string&   getHDRLighterPath(void);
    //
    //platform 接口: @todo: 是否暴露？
//    int    getUsedCPU(void);
//    int    getOpenclPlatformId(void);
//    void   getUsedGPU(std::string &gpu);
private:

};

}

#endif //GME_DM_DOCSETTING_H
