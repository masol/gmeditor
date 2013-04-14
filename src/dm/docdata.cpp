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
#include "dm/docdata.h"
#include "slg/slg.h"
#include "docprivate.h"
#include <boost/assert.hpp>

namespace gme{

const ObjectNode&
DocData::getRootObject()
{
    return pDocData->m_objectGroup;
}

std::string
DocData::getObjectName(const ObjectNode& obj)
{
    return pDocData->getObjectName(obj.m_matid);
}

std::string
DocData::getMatName(const ObjectNode& obj)
{
    return pDocData->getMaterialName(obj.m_matid);
}

} //end namespace gme.
