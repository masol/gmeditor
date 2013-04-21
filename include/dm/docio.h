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

#ifndef  GME_DM_DOCIO_H
#define  GME_DM_DOCIO_H


#include "dm/doc.h"
#include <boost/uuid/uuid.hpp>

namespace gme{

class ObjectNode;

class DocIO : public DocScopeLocker
{
protected:
    /** @brief 从场景中加载额外信息。
    **/
    void    loadExtraFromScene(void);
    void    loadExtraFromSlgSceneFile(const std::string pathstring);
    bool    exportSpoloScene(const std::string &pathstring,bool bExportRes);
public:
    bool    loadScene(const std::string &path);
    /** @brief 从制定文件加载场景，并加入到pParent指定的节点下。
      * @param pParent 指示了加入的父节点。如果为空，则加入到根节点下。
    **/
    bool    importScene(const std::string &path,ObjectNode *pParent = NULL);
    bool    exportScene(const std::string &path,bool bExportResource = true);

    /** @brief 删除指定模型.
     *  @param id 指定模型的id.
    **/
    bool    deleteModel(const boost::uuids::uuid &id);
};

}

#endif //GME_DM_DOCIO_H

