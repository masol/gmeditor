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

#ifndef  GME_DM_RENDERSETTING_H
#define  GME_DM_RENDERSETTING_H

#include <slg/slg.h>

//@FIXME: 与slg解耦。

namespace gme{

class RenderSetting
{
public:
    static  const   int     ACCEL_DEFAULT = -1;
    static  const   int     ACCEL_BVH = 0;
    static  const   int     ACCEL_QBVH = 2;
    static  const   int     ACCEL_MQBVH = 3;
private:
    friend class DocIO;
    
protected:
    int                     m_accelType;
    slg::RenderEngineType   m_renderEngineType;
    int                     m_screenRefreshInterval;
    //@ in pathocl.cpp. import opencl,path and sampler setting.
    inline void    assignFrom(const RenderSetting& ref)
    {
        m_accelType = ref.m_accelType;
        m_renderEngineType = ref.m_renderEngineType;
        m_screenRefreshInterval = ref.m_screenRefreshInterval;
    }
public:
    RenderSetting()
    {
        m_accelType = ACCEL_DEFAULT;
        m_renderEngineType =  slg::RenderEngineType::PATHOCL;
        m_screenRefreshInterval = 100;
    }
    RenderSetting(const RenderSetting& ref){
        assignFrom(ref);
    }
    ~RenderSetting(){}
    RenderSetting& operator=(const RenderSetting& ref){
        assignFrom(ref);
        return *this;
    }
};

}

#endif //GME_DM_RENDERSETTING_H
