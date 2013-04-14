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
#include "dm/doccamera.h"
#include "slg/slg.h"
#include "docprivate.h"

namespace gme{

bool
DocCamera::rotate(int distX,int distY,float optRotateStep)
{
    slg::RenderSession* session = pDocData->getSession();
    if(session && session->film)
    {
	    session->BeginEdit();

	    session->renderConfig->scene->camera->RotateUp(0.04f * distY * optRotateStep);
	    session->renderConfig->scene->camera->RotateLeft(0.04f * distX * optRotateStep);


	    session->renderConfig->scene->camera->Update(session->film->GetWidth(), session->film->GetHeight());
	    session->editActions.AddAction(slg::CAMERA_EDIT);
	    session->EndEdit();
	    return true;
	}
	return false;
}


} //end namespace gme.
