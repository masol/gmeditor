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

#ifndef GME_UTILS_SYSINFO_H
#define GME_UTILS_SYSINFO_H


/**
 * @file        sysinfo.h
 * @brief       获取信息信息的工具类。
 **/
namespace gme
{

struct	SysInfo{
	/** @brief 获取系统idle的时间(没有任何用户输入的时间).
	**/
	static	int		GetSystemIdleMicroSecond(void);
};

}

 
#endif //GME_UTILS_SYSINFO_H