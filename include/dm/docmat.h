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

#ifndef  GME_DM_DOCMAT_H
#define  GME_DM_DOCMAT_H


#include "dm/doc.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/any.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <string>

namespace gme{

class DocMat : public DocScopeLocker
{
public:
    typedef    boost::property_tree::basic_ptree<std::string, boost::any>       type_material_def;
    std::string getMatName(const boost::uuids::uuid& id);
    int         getMatType(const boost::uuids::uuid& id);
    boost::any  getMatProperty(const boost::uuids::uuid& id,const std::string &prop);
    bool        setMatProperty(const boost::uuids::uuid& id,const std::string &prop,boost::any &value);
    void        setMaterial(const boost::uuids::uuid& id,type_material_def &matdef);
    boost::uuids::uuid  addMaterial(type_material_def &matdef);
};

}

#endif //GME_DM_DOCMAT_H

