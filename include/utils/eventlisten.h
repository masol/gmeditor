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

#ifndef GME_UTILS_EVENTLISTEN_H
#define GME_UTILS_EVENTLISTEN_H

#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/foreach.hpp>

/**
 * @file        eventlisten.h
 * @brief       添加事件回调支持。
 **/
namespace gme
{

template<class type_evt_id,class type_func>
class EventListen
{
protected:
    typedef std::vector<type_func>                                  type_callback_vector;
    typedef boost::unordered_map<type_evt_id,type_callback_vector>  type_callback;

    type_callback       m_callback_data;
public:
    template<class T1>
    void    fire(type_evt_id id,T1 v1)
    {
        typename type_callback::iterator it = m_callback_data.find(id);
        if(it != m_callback_data.end())
        {
            BOOST_FOREACH(type_func &f,it->second)
            {
                f(v1);
            }
        }
    }

    template<class T1,class T2>
    void    fire(type_evt_id id,T1 v1,T2 v2)
    {
        typename type_callback::iterator it = m_callback_data.find(id);
        if(it != m_callback_data.end())
        {
            BOOST_FOREACH(type_func &f,it->second)
            {
                f(v1,v2);
            }
        }
    }

    template<class T1,class T2,class T3>
    void    fire(type_evt_id id,T1 v1,T2 v2,T3 v3)
    {
        typename type_callback::iterator it = m_callback_data.find(id);
        if(it != m_callback_data.end())
        {
            BOOST_FOREACH(type_func &f,it->second)
            {
                f(v1,v2,v3);
            }
        }
    }

    template<class T1,class T2,class T3,class T4>
    void    fire(type_evt_id id,T1 v1,T2 v2,T3 v3,T4 v4)
    {
        typename type_callback::iterator it = m_callback_data.find(id);
        if(it != m_callback_data.end())
        {
            BOOST_FOREACH(type_func &f,it->second)
            {
                f(v1,v2);
            }
        }
    }

    void    addEventListen(type_evt_id id,type_func &func)
    {
        typename type_callback::iterator it = m_callback_data.find(id);
        if(it != m_callback_data.end())
        {
            it->second.push_back(func);
        }else{
            m_callback_data[id].push_back(func);
        }
    }

    void    removeEventListen(type_evt_id id,type_func &func)
    {
        typename type_callback::iterator it = m_callback_data.find(id);
        if(it != m_callback_data.end())
        {
            typename type_callback_vector::iterator vec_it = it->second.begin();
            while(vec_it != it->second.end())
            {
                if(*vec_it == func)
                {
                    it->second.erase(vec_it);
                    if(it->second.size() == 0)
                    {
                        m_callback_data.erase(it);
                    }
                }
                vec_it++;
            }
        }
    }
};
}


#endif //GME_UTILS_EVENTLISTEN_H
