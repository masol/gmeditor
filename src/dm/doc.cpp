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
#include "dm/doc.h"
#include <iostream>
#include "docprivate.h"

#include "luxrays/luxrays.h"
#include "slg/slg.h"


static
void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
{
    gme::Doc::SysLog(gme::Doc::LOG_ERROR,message);
//	printf("\n*** ");
//	if(fif != FIF_UNKNOWN)
//		printf("%s Format\n", FreeImage_GetFormatFromFIF(fif));
//
//	printf("%s", message);
//	printf(" ***\n");
}

static
void LuxRaysDebugHandler(const char *msg)
{
    gme::Doc::SysLog(gme::Doc::LOG_TRACE,msg,"LuxRays");
}

static
void SDLDebugHandler(const char *msg)
{
    gme::Doc::SysLog(gme::Doc::LOG_TRACE,msg,"SDL");
}

static
void SLGDebugHandler(const char *msg)
{
    gme::Doc::SysLog(gme::Doc::LOG_TRACE,msg,"SLG");
}


namespace gme{

Doc::type_func_syslog        Doc::sv_syslog_func;
int                          Doc::sv_syslog_level = Doc::LOG_STATUS;


Doc::Doc(void)
{
	slg::LuxRays_DebugHandler = ::LuxRaysDebugHandler;
	slg::SLG_DebugHandler = ::SLGDebugHandler;
	slg::SLG_SDLDebugHandler = ::SDLDebugHandler;

    FreeImage_Initialise(0);
	FreeImage_SetOutputMessage(::FreeImageErrorHandler);

    pDocData = new DocPrivate();
}

Doc::~Doc(void)
{
    if(pDocData)
    {
        delete pDocData;
    }
	FreeImage_DeInitialise();
}

bool
Doc::isValid(void)
{
    return Doc::instance().pDocData->m_session.get() != NULL;
}

}

