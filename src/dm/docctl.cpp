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

#define __CL_ENABLE_EXCEPTIONS 1
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

namespace gme{

bool
DocCtl::isRuning()
{
    return pDocData->isRunning();
}

bool
DocCtl::isPause()
{
    return pDocData->isPause();
}

bool
DocCtl::isStop()
{
    return pDocData->isStop();
}


bool
DocCtl::start()
{
    if(pDocData->m_session && !pDocData->isRunning() )
    {
        pDocData->start();
        return true;
    }
    return false;
}

bool
DocCtl::refresh()
{
    if(pDocData->isRunning())
    {
        try{
            pDocData->getSession()->Stop();
        }catch(cl::Error &err)
        {
            Doc::SysLog(Doc::LOG_ERROR,boost::str(boost::format(__("停止渲染线程时发生错误。失败原因%s:(%s)")) % err.what() % err.err() ) );
        }
        try{
            pDocData->getSession()->Start();
        }catch(cl::Error &err)
        {
            Doc::SysLog(Doc::LOG_ERROR,boost::str(boost::format(__("启动渲染线程时发生错误。失败原因%s:(%s)")) % err.what() % err.err() ) );
        }
        return true;
    }
    return false;
}


bool
DocCtl::stop()
{
    if( pDocData->m_session && pDocData->isRunning() )
    {
        pDocData->closeScene();
        return true;
    }
    return false;
}

bool
DocCtl::pause()
{
    if( pDocData->m_session && pDocData->isRunning() )
    {
        pDocData->pause();
        return true;
    }
    return false;
}

} //end namespace gme.
