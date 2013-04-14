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
#include "dm/docctl.h"
#include "slg/slg.h"
#include "docprivate.h"
#include <boost/assert.hpp>

namespace gme{

bool
DocCtl::isRuning()
{
    return pDocData->m_started;
}

bool
DocCtl::start()
{
    if(pDocData->m_session)
    {
        pDocData->m_session->Start();
        pDocData->m_started = true;
        return true;
    }
    return false;
}

bool
DocCtl::stop()
{
    if(pDocData->m_session)
    {
        pDocData->m_session->Stop();
        pDocData->m_started = false;
        return true;
    }
    return false;
}

bool
DocCtl::pause()
{
    BOOST_ASSERT_MSG(false,"NOT IMPLEMNT");
    return false;
}

} //end namespace gme.
