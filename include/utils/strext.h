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

#ifndef GME_UTILS_STREXT_H
#define GME_UTILS_STREXT_H

/**
 * @file        strext.h
 * @brief       str的扩展函数。
 **/
 
#include <boost/locale.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/unordered_map.hpp>


namespace gme{

namespace string{

//@FIXME: Boost.Locale provides to_utf, from_utf and utf_to_utf functions in the boost::locale::conv namespace, so this file is useless.

inline std::wstring from_utf8( const std::string& str )
{
    return boost::locale::conv::utf_to_utf<wchar_t>(str);
}

inline std::string to_utf8( const std::wstring& str )
{
    return boost::locale::conv::utf_to_utf<char>(str);
}

inline std::string uuid_to_string(const boost::uuids::uuid &u)
{
    std::string result;
    result.reserve(32);

    for (boost::uuids::uuid::const_iterator it_data = u.begin(); it_data!=u.end(); ++it_data) {
        const size_t hi = ((*it_data) >> 4) & 0x0F;
        result += boost::uuids::detail::to_char(hi);

        const size_t lo = (*it_data) & 0x0F;
        result += boost::uuids::detail::to_char(lo);
    }
    return result;
}

} //end namespace string
} //end namespace gme


#endif //GME_UTILS_STREXT_H
