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

struct  RenderInfo{
    unsigned int    pass;
    float           convergence;
    double          elapsedTime;
    double          totalRaysSec;
    double          totalSamplesSec;
private:
    inline void    deepCopy(const RenderInfo& ri)
    {
        this->pass = ri.pass;
        this->convergence = ri.convergence;
        this->elapsedTime = ri.elapsedTime;
        this->totalRaysSec = ri.totalRaysSec;
        this->totalSamplesSec = ri.totalSamplesSec;
    }
public:
    RenderInfo(void)
    {
        pass = 0;
        this->convergence = 0;
        this->elapsedTime = 0;
        this->totalRaysSec = 0;
        this->totalSamplesSec = 0;
    }
    RenderInfo(const RenderInfo& ref)
    {
        deepCopy(ref);
    }
    inline RenderInfo& operator=(const RenderInfo& ri)
    {
        deepCopy(ri);
        return *this;
    }
    ///@brief add renderinfo from another.
    inline  void    merge(const RenderInfo& ri)
    {
        this->pass += ri.pass;
        ///@fixme : how to do about convergence?
        this->convergence = (this->convergence + ri.convergence) / 2;
        this->elapsedTime += ri.elapsedTime;
        this->totalRaysSec += ri.totalRaysSec;
        this->totalSamplesSec += ri.totalSamplesSec;
    }
};

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
    friend class SlgMesh2Name;
	friend class ObjectNode;
	friend class CacheFilm;
    friend class Singleton<Doc>;
    typedef Singleton<Doc>   inherited;
    Doc(void);
    boost::recursive_mutex      m_mutex;
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
    DocPrivate                  *pDocData;
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
