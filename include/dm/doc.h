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

#ifndef  GME_DM_DOC_H
#define  GME_DM_DOC_H

#include "utils/singleton.h"
#include <boost/thread/recursive_mutex.hpp>
#include "slg/rendersession.h"
#include <boost/shared_ptr.hpp>

namespace gme{

class Doc : public Singleton<Doc>
{
protected:
    friend class DocScopeLocker;
    friend class Singleton<Doc>;
    typedef Singleton<Doc>   inherited;
    Doc(void);
    boost::recursive_mutex      m_mutex;
    ///@fixme: slg的started为保护成员。
    bool            m_started;
    void            start();
    void            stop();
private:
    //@FIXME: 这里不能直接暴露slg.需要使用DocImpl类来封装，以方便支持cycles,luxrender...
    //@TODO: 需要一个材质转化专家系统来支持材质转化。
	//
    boost::shared_ptr<slg::RenderSession>     m_session;

    /** @brief 锁定文档。
    **/
    inline  void    lock(){
        m_mutex.lock();
    }
    /** @brief 解锁文档。
    **/
    inline  void    unlock(){
        m_mutex.unlock();
    }
public:
    ~Doc(void);
};

class DocScopeLocker
{
protected:
    boost::shared_ptr<slg::RenderSession>   &m_session;
public:
    DocScopeLocker() : m_session(Doc::instance().m_session)
    {
        Doc::instance().lock();
    }
    ~DocScopeLocker(){
        Doc::instance().unlock();
    }
    bool    isValid(void)const{
        return Doc::instance().m_session.get() != NULL;
    }
};

}



#endif  //GME_DM_DOC_H
