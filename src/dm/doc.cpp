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
#include "slg/slg.h"
#include "luxrays/utils/properties.h"
#include "utils/pathext.h"
#include <boost/algorithm/string/predicate.hpp>

namespace gme{

Doc::Doc(void)
{
    m_pSession = NULL;
}

Doc::~Doc(void)
{
    if(m_pSession)
    {
        delete m_pSession;
        m_pSession = NULL;
    }
}

bool
Doc::loadScene(const std::string &path)
{
    std::string ext = boost::filesystem::gme_ext::get_extension(path);
    if(boost::iequals(ext,".cfg"))
    {
        luxrays::Properties cmdLineProp;
        slg::RenderConfig *config = new slg::RenderConfig(&path, &cmdLineProp);
        m_pSession = new slg::RenderSession(config);
        m_pSession->Start();
        return true;
    }
    return false;
}



}

