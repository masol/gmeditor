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



#ifndef GME_UTILS_SINGLETON_H
#define GME_UTILS_SINGLETON_H

#include <boost/utility.hpp>
#include <boost/thread/once.hpp>
#include <boost/scoped_ptr.hpp>

namespace gme
{
    template<class T>
    class Singleton : private boost::noncopyable
    {
    public:
        static T& instance()
        {
            boost::call_once(T::init, m_once_flag);
            return *m_instance_ptr;
        }
        static void init()
        {
            m_instance_ptr.reset(new T());
        }
    protected:
        ~Singleton() {}
        Singleton() {}
    private:
        static boost::scoped_ptr<T> m_instance_ptr;
        static boost::once_flag     m_once_flag;
    };
}

template<class T> boost::scoped_ptr<T> gme::Singleton<T>::m_instance_ptr(0);
template<class T> boost::once_flag gme::Singleton<T>::m_once_flag = BOOST_ONCE_INIT;

#endif //GME_UTILS_SINGLETON_H

