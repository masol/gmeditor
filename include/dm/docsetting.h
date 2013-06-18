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
#include "slg/film/film.h"
#include "slg/film/tonemapping.h"

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
    ///@brief for include files reason, follow const define in slgobject.cpp.
protected:
    static   int     sv_loadingFlags;
    static   bool    sv_ignoreNormal;
    static   float   sv_aiSmoothing_Angle;
public:
    static   const  int ValidateDataStructure;
    static   const  int GenSmoothNormals;
    static   const  int JoinIdenticalVertices;
    static   const  int RemoveRedundantMaterials;
    static   const  int ImproveCacheLocality;
    static   const  int FixInfacingNormals;
    static   const  int FindDegenerates;
    static   const  int FindInvalidData;
    static   const  int FlipUVs;
    static   const  int OptimizeMeshes;
    static   const  int Debone;
public:
    //film 接口:
//	type_xml_node*   getToneMap(type_xml_node &parent);
//	std::string   getToneMapTypeNameByName(int type);
//	bool   getLinearScale(float &ls);
//	bool   setLinearScale(float ls);
    const slg::ToneMapParams*   getToneMapParams(void);
    bool  setToneMapParams(const slg::ToneMapParams &param);
    int getFilmFilter(void);
    bool setFilmFilter(int type);
    float getGamma(void);
    bool setGamma(float g);


    //engine 接口:
    int    getEngineType(void);
    void   setEngineType(int type);
    int    getAccelType(void);
    void   setAccelType(int type);

    //light(evnlight and sunlight) interface:
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
    ///@deprecated 新方案采用从主编辑视图上修改阳光方向。本函数作废。
    bool  changeSunDir(const luxrays::Vector &dir);
    bool  changeSunDir(float filmx,float filmy);
    bool  changeSunGain(const luxrays::Spectrum &g);
    bool  changeSunTurbidity(float t);
    bool  changeSunRelsize(float t);

    ///@brief 返回采样方式。
    int   getSamplerType(void);
    bool  setSamplerType(int type);

    //@brief 当前设置在编辑时是否强制刷新。
    static  bool    forceRefresh(void);
    static  void    forceRefresh(bool forceRefresh);
    static  bool    refreshWhenError(void);
    static  void    refreshWhenError(bool rwe);

    static  bool    exportNewImage(void);
    static  void    exportNewImage(bool eni);
    static  bool    exportNewMesh(void);
    static  void    exportNewMesh(bool enm);


    //@brief 当前加载设定。
    inline static  int loadingFlag(void)
    {
        return sv_loadingFlags;
    }
    inline static  void loadingFlag(int f)
    {
        sv_loadingFlags = f;
    }
    inline static  bool hasLoadingFlag(int f)
    {
        return ((sv_loadingFlags & f) != 0);
    }
    inline static  void setLoadingFlag(int f)
    {
        sv_loadingFlags |= f;
    }
    inline static  void clearLoadingFlag(int f)
    {
        sv_loadingFlags &= (~f);
    }
    inline static  bool ignoreNormals(void)
    {
        return sv_ignoreNormal;
    }
    inline static  void ignoreNormals(bool i)
    {
        sv_ignoreNormal = i;
    }
    inline static  float smoothAngle(void)
    {
        return sv_aiSmoothing_Angle;
    }
    inline static  void smoothAngle(float angle)
    {
        sv_aiSmoothing_Angle = angle;
    }

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
