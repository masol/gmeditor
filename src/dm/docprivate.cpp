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
#include "docprivate.h"
#include "dm/docmat.h"
#include "slgobject.h"
#include "slgtexture.h"
#include "slgmaterial.h"



namespace gme{

DocPrivate::DocPrivate(void)
{
    m_started = false;
}

void
DocPrivate::closeScene(void)
{
    clearSelection();
    if(m_session)
    {
        if(m_started)
        {
            m_session->Stop();
        }
        m_session.reset();
    }
    m_started = false;
    objManager.clear();
    matManager.clear();
    texManager.clear();
    camManager.clear();
    fireStateChanged(STATE_CLOSE);
}



DocPrivate::~DocPrivate(void)
{
    closeScene();
}

}


