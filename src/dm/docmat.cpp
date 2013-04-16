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
#include "dm/docmat.h"
#include "slg/slg.h"
#include "docprivate.h"
#include <boost/assert.hpp>

namespace gme{

std::string
DocMat::getMatName(const boost::uuids::uuid& id)
{
    return pDocData->getMaterialNameInSlg(id);
}

int
DocMat::getMatType(const boost::uuids::uuid& id)
{
    return 0;
}

boost::any
DocMat::getMatProperty(const boost::uuids::uuid& id,const std::string &prop)
{
    return boost::any();
}

bool
DocMat::setMatProperty(const boost::uuids::uuid& id,const std::string &prop,boost::any &value)
{
    return false;
}

void
DocMat::setMaterial(const boost::uuids::uuid& id,type_material_def &matdef)
{
}

boost::uuids::uuid
DocMat::addMaterial(type_material_def &matdef)
{
    return boost::uuids::uuid();
}



} //end namespace gme.
