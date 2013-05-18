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
    /** @brief 从指定文件加载模型/场景，并加入到pParent指定的节点下。
      * @param pParent 指示了加入的父节点。如果为空，则加入到根节点下。
    **/
    bool    importObject(const std::string &path,ObjectNode *pParent = NULL);
    /** @brief 删除指定模型.
     *  @param id 指定模型的id.
     *  @fixme 支持递归删除全部子节点。
    **/
    bool    deleteObject(const std::string &id);
    void    deleteAllSelection(void);

    /// @brief clear the selection vector.
    void    clearSelection(void);
    /// @brief add id to selection vector.return true if event fired.
    bool    select(const std::string &id);
    /// @brief select object through filmx and filmy.
    void    select(float filmx,float filmy);
    /// @brief remove id from selection vector.return true if event fired.
    bool    deselect(const std::string &id);
    /// @brief return the selection vector.
    const std::vector<std::string>& getSelection(void);
public:
    /// @brief some child been added.
    void    onChildAdded(type_selection_handler handler);
    /// @brief self nodeobject been removed.
    void    onSelfRemoved(type_selection_handler handler);
    /// @brief set the callback when item added to selection vector.
    void    onSelected(type_selection_handler handler);
    /// @brief set the callback when item removed from selection vector.
    void    onDeselected(type_selection_handler handler);
};

}

#endif //GME_DM_DOCOBJ_H

