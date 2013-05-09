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

#ifndef  GME_DM_SETTING_H
#define  GME_DM_SETTING_H

#include <vector>
#include <string>
#include "utils/singleton.h"

namespace gme{

/** @brief 描述了配置常量。这些信息会在本地配置以及文档级配置中用到。
  * @brief 本地配置使用option对象，文档配置使用docsetting对象。
**/
class Setting
{
public:
//accelType.
    static  const   int     ACCEL_DEFAULT = -1;
    static  const   int     ACCEL_BVH = 0;
    static  const   int     ACCEL_QBVH = 2;
    static  const   int     ACCEL_MQBVH = 3;
//renderEngineType

	static  const   int     PATHOCL;
	static  const   int     LIGHTCPU;
	static  const   int     PATHCPU;
	static  const   int     BIDIRCPU;
	static  const   int     BIDIRHYBRID;
	static  const   int     CBIDIRHYBRID;
	static  const   int     BIDIRVMCPU;
	static  const   int     FILESAVER;
	static  const   int     RTPATHOCL;
///@brief 配置名称常量.
	static	const std::string	OPT_PLATFORMID;
	static	const std::string	OPT_DEVICESTR;
};

/** @brief 提供了当前系统中opencl硬件信息。
**/
class   clHardwareInfo : public Singleton<clHardwareInfo>
{
private:
    typedef Singleton<clHardwareInfo>   inherit;
    friend class Singleton<clHardwareInfo>;
    std::vector<std::string>                    m_platformNames;
    std::vector<std::vector<std::string> >      m_deviceNames;
public:
    inline const std::vector<std::string>&   getPlatforms(void)const
    {
        return m_platformNames;
    }
    inline const std::vector<std::string>&   getDeviceNames(unsigned int idx)const
    {
        if(idx < m_deviceNames.size())
            return m_deviceNames[idx];
		throw std::exception();
        //return std::vector<std::string>();
    }
    clHardwareInfo();
    ~clHardwareInfo(){}
    /** @brief 返回选择指定平台下全部硬件的标识字符串。(参考slg的配置文件格式)
    **/
    std::string     getFullSelectString(unsigned int platformIdx = 0);
};

}

#endif //GME_DM_SETTING_H
