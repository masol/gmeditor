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

namespace gme{

class DocPrivate;
class Doc : public Singleton<Doc>
{
protected:
    friend class DocScopeLocker;
    friend class ExtraObjectManager;
    friend class ExtraMaterialManager;
    friend class ExtraTextureManager;
    friend class SlgMaterial2Name;
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
public:
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
