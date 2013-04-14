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
void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
	printf("\n*** ");
	if(fif != FIF_UNKNOWN)
		printf("%s Format\n", FreeImage_GetFormatFromFIF(fif));

	printf("%s", message);
	printf(" ***\n");
}

static
void LuxRaysDebugHandler(const char *msg) {
	std::cerr << "[LuxRays] " << msg << std::endl;
}

static
void SDLDebugHandler(const char *msg) {
	std::cerr << "[SDL] " << msg << std::endl;
}

static
void SLGDebugHandler(const char *msg) {
	std::cerr << "[SLG] " << msg << std::endl;
}


namespace gme{

Doc::Doc(void)
{
	slg::LuxRays_DebugHandler = ::LuxRaysDebugHandler;
	slg::SLG_DebugHandler = ::SLGDebugHandler;
	slg::SLG_SDLDebugHandler = ::SDLDebugHandler;

    FreeImage_Initialise(TRUE);
	FreeImage_SetOutputMessage(::FreeImageErrorHandler);
	
    pDocData = new DocPrivate();
}

Doc::~Doc(void)
{
    if(pDocData)
    {
        delete pDocData;
    }
}

bool
Doc::isValid(void)
{
    return Doc::instance().pDocData->m_session.get() != NULL;
}

}

