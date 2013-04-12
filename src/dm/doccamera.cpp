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

namespace gme{

bool
DocCamera::rotate(int distX,int distY,float optRotateStep)
{
    if(m_session && m_session->film)
    {
	    m_session->BeginEdit();

	    m_session->renderConfig->scene->camera->RotateUp(0.04f * distY * optRotateStep);
	    m_session->renderConfig->scene->camera->RotateLeft(0.04f * distX * optRotateStep);


	    m_session->renderConfig->scene->camera->Update(m_session->film->GetWidth(), m_session->film->GetHeight());
	    m_session->editActions.AddAction(slg::CAMERA_EDIT);
	    m_session->EndEdit();
	    return true;
	}
	return false;
}


} //end namespace gme.
