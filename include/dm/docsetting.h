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

namespace gme
{

/** @brief 动态配置接口。可以动态配置当前文档的各类信息。const information please see class Setting.
**/
class DocSetting : public DocScopeLocker
{
public:
    //film 接口:
    bool   getImageSize(unsigned long &width,unsigned long &height);
    bool   setImageSize(unsigned long w,unsigned long h);
	bool   getLinearScale(float &ls);
	bool   setLinearScale(float ls);
    //engine 接口:
    int    getEngineType(void);
    void   setEngineType(int type);
    int    getAccelType(void);
    void   setAccelType(int type);
    //platform 接口: @todo: 是否暴露？
//    int    getUsedCPU(void);
//    int    getOpenclPlatformId(void);
//    void   getUsedGPU(std::string &gpu);
};

}

#endif //GME_DM_DOCSETTING_H
