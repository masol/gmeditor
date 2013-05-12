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
#include <boost/function.hpp>


namespace gme{

class DocPrivate;
class Doc : public Singleton<Doc>
{
public:
    typedef     boost::function<void (int,const char*,const char*)>     type_func_syslog;
protected:
    friend class DocScopeLocker;
    friend class ExtraObjectManager;
    friend class ExtraMaterialManager;
    friend class ExtraTextureManager;
    friend class ExtraSettingManager;
    friend class ExtraCameraManager;
    friend class SlgMaterial2Name;
    friend class SlgTexture2Name;
	friend class ObjectNode;
    friend class Singleton<Doc>;
    typedef Singleton<Doc>   inherited;
    Doc(void);
    boost::recursive_mutex      m_mutex;
    DocPrivate                  *pDocData;
private:
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
    bool    isValid(void);
    static  type_func_syslog        sv_syslog_func;
    static  int                     sv_syslog_level;
public:
    enum{
        LOG_TRACE,
        LOG_DEBUG,
        LOG_VERBOSE,
        LOG_MESSAGE,
        LOG_STATUS,
        LOG_WARNING,
        LOG_ERROR,
        LOG_SYSERROR,
        LOG_FATALERROR,
        LOG_MAX
    };
    inline  static  int    SyslogLevel(void)
    {
        return sv_syslog_level;
    }
    inline  static  void   SyslogLevel(int level)
    {
        sv_syslog_level = level;
    }
    inline  static  void   SetSysLog(type_func_syslog func)
    {
        sv_syslog_func = func;
    }
    ///@brief mask is only available for trace.
    inline static  void    SysLog(int level,const std::string &msg,const char *mask = NULL)
    {
        SysLog(level,msg.c_str(),mask);
    }
    inline static  void    SysLog(int level,const char *msg,const char* mask = NULL)
    {
        if( (level >= sv_syslog_level) && sv_syslog_func)
            sv_syslog_func(level,msg,mask);
    }
    ~Doc(void);
};

class DocScopeLocker
{
protected:
    DocPrivate   *pDocData;
public:
    DocScopeLocker() : pDocData(Doc::instance().pDocData)
    {
        Doc::instance().lock();
    }
    ~DocScopeLocker(){
        Doc::instance().unlock();
    }
    inline bool    isValid(void)const{
        return Doc::instance().isValid();
    }
};

}



#endif  //GME_DM_DOC_H
