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
#include "utils/sysinfo.h"

#if WIN32
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#else
#endif

namespace gme
{

int
SysInfo::GetSystemIdleMicroSecond(void)
{
	int		ret = -1;
#if WIN32
	LASTINPUTINFO last_input;
    //BOOL screensaver_active;
	if( GetLastInputInfo(&last_input) /*&& SystemParametersInfo(SPI_GETSCREENSAVERACTIVE, 0, &screensaver_active, 0)*/ )
	{
		ret = last_input.dwTime;
	}
#else
#endif
	return ret;
}

}
