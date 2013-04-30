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
#include "dm/docobj.h"
#include "slg/slg.h"
#include "docprivate.h"
#include "slgobject.h"
#include <boost/assert.hpp>

namespace gme{

ObjectNode&
DocObj::getRootObject()
{
    return pDocData->objManager.getRoot();
}

bool
DocObj::deleteModel(const std::string &id)
{
    return pDocData->objManager.removeMesh(id);
}

void
DocObj::clearSelection(void)
{
    pDocData->clearSelection();
}

bool
DocObj::select(const std::string &id)
{
    return pDocData->addSelection(id);
}

bool
DocObj::deselect(const std::string &id)
{
    return pDocData->removeSelection(id);
}

void
DocObj::onSelectionAdded(type_selection_handler handler)
{
    pDocData->selection_Evt.addEventListen(DocPrivate::SEL_ITEMADDED,handler);
}

void
DocObj::onSelectionRemoved(type_selection_handler handler)
{
    pDocData->selection_Evt.addEventListen(DocPrivate::SEL_ITEMREMOVED,handler);
}

const std::vector<std::string>&
DocObj::getSelection(void)
{
    return pDocData->getSelection();
}

} //end namespace gme.
