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
#include "dm/xmlutil.h"

namespace slg{
class Scene;
}

namespace gme{

class ObjectNode;
class ImportContext;

class DocIO : public DocScopeLocker
{
public:
    typedef boost::function<void (void)>     type_state_handler;
protected:
    /** @brief 从场景中加载额外信息。
    **/
    void    loadExtraFromScene(void);
    void    loadExtraFromSlgSceneFile(const std::string pathstring);
    bool    exportSpoloScene(const std::string &pathstring,bool bExportRes);

    /** @brief load cfg format implement.
    **/
    bool    loadSlgScene(const std::string &path);
    /** @brief load sps format implement
    **/
    bool    loadSpsScene(const std::string &path);
    /** @brief load assimp foramt implement.
    **/
    bool    loadAssimpScene(const std::string &path);
    ///@brief after scene loaded, init and create scene.
    void    initAndStartScene(slg::Scene *scene,ImportContext *pctx);
public:
    bool    loadScene(const std::string &path);
    const std::string& getLastLoadedPath(void);
    void    setLastLoadedPath(const std::string &path);
    bool    exportScene(const std::string &path,bool bExportResource = true);

    void    onSceneLoaded(type_state_handler handler);
    void    onSceneClosed(type_state_handler handler);
    void    onSceneModified(type_state_handler handler);

    bool    isModified(void);
public:
    /** @brief 更新node所指示对象的关联资源，将所有资源拷贝到指定目录，并修改node中的关联索引。
     * @details 静态方法，不需要锁定文档对象。
    **/
    //static  bool    copyResource(type_node &node,const std::string &path);
    ///@brief 针对原始xml做渲染优化，优化之后编辑将可能出各类问题。
    //static  void  optimizeForRender(const type_doc &src,type_doc &dest);
};

}

#endif //GME_DM_DOCIO_H

