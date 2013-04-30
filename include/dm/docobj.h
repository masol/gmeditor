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

#ifndef  GME_DM_DOCOBJ_H
#define  GME_DM_DOCOBJ_H


#include "dm/doc.h"
#include "dm/objectnode.h"

namespace gme{

class DocObj : public DocScopeLocker
{
public:
    typedef boost::function<void (const std::string &)>     type_selection_handler;
public:
    ObjectNode&   getRootObject();
    /** @brief 删除指定模型.
     *  @param id 指定模型的id.
    **/
    bool    deleteModel(const std::string &id);

    /// @brief clear the selection vector.
    void    clearSelection(void);
    /// @brief add id to selection vector.return true if event fired.
    bool    select(const std::string &id);
    /// @brief remove id from selection vector.return true if event fired.
    bool    deselect(const std::string &id);

    /// @brief set the callback when item added to selection vector.
    void    onSelectionAdded(type_selection_handler handler);
    /// @brief set the callback when item removed from selection vector.
    void    onSelectionRemoved(type_selection_handler handler);
    /// @brief return the selection vector.
    const std::vector<std::string>& getSelection(void);
};

}

#endif //GME_DM_DOCOBJ_H

