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
#include "dm/setting.h"
#include "slg/slg.h"
#include "slg/renderengine.h"


#define __CL_ENABLE_EXCEPTIONS 1
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif



namespace gme
{
const   int     Setting::PATHOCL = slg::PATHOCL;
const   int     Setting::LIGHTCPU = slg::LIGHTCPU;
const   int     Setting::PATHCPU = slg::PATHCPU;
const   int     Setting::BIDIRCPU = slg::BIDIRCPU;
const   int     Setting::BIDIRHYBRID = slg::BIDIRHYBRID;
const   int     Setting::CBIDIRHYBRID = slg::CBIDIRHYBRID;
const   int     Setting::BIDIRVMCPU = slg::BIDIRVMCPU;
const   int     Setting::FILESAVER = slg::FILESAVER;
const   int     Setting::RTPATHOCL = slg::RTPATHOCL;

const std::string	Setting::OPT_PLATFORMID = "opencl.platform.index";
const std::string	Setting::OPT_DEVICESTR = "opencl.devices.select";



clHardwareInfo::clHardwareInfo()
{
    try{
        cl_int err;
        std::vector< cl::Platform > platformList;
        cl::Platform::get(&platformList);
        for(std::vector< cl::Platform >::iterator it = platformList.begin(); it < platformList.end(); ++it)
        {
            std::string platformVendor;
            it->getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
            GME_TRACE("found platform:",platformVendor);

            m_platformNames.push_back(platformVendor);
            std::vector<std::string>    devSet;
            std::vector< cl::Device > devices;
            if(CL_SUCCESS == it->getDevices(CL_DEVICE_TYPE_ALL,&devices))
            {
                std::vector< cl::Device >::iterator dit = devices.begin();
                while(dit != devices.end())
                {
                    std::string name,vendor;
                    err = dit->getInfo((cl_platform_info)CL_DEVICE_NAME, &name);
                    err = dit->getInfo((cl_platform_info)CL_DEVICE_VENDOR,&vendor);
                    GME_TRACE("    name = ",name,";vendor=",vendor);
                    devSet.push_back(vendor + ':' + name);
                    ++dit;
                }
            }
            m_deviceNames.push_back(devSet);
        }
    }catch(cl::Error err)
    {
        std::cerr << err.what() << "(" << err.err() << ")" << std::endl;
        m_platformNames.clear();
        m_deviceNames.clear();
    }
}

std::string
clHardwareInfo::getFullSelectString(unsigned int platformIdx)
{
	if(platformIdx < m_deviceNames.size())
	{
		return std::string(m_deviceNames[platformIdx].size(),'1');
	}
	return "";
}

}

