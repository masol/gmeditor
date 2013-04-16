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
#include "dm/objectnode.h"
#include "docprivate.h"
#include "slg/slg.h"


namespace gme{

std::string
ObjectNode::idto_string(const boost::uuids::uuid &u)
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


}
