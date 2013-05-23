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

#ifndef  GME_DM_DOCPRIVATE_H
#define  GME_DM_DOCPRIVATE_H

#include "slg/rendersession.h"
#include "slgtexture.h"
#include "slgmaterial.h"
#include "slgobject.h"
#include "slgcamera.h"
#include "utils/pathext.h"
#include "cachefilm.h"
#include <boost/shared_ptr.hpp>
#include "utils/eventlisten.h"
#include <boost/function.hpp>
#include <boost/foreach.hpp>

namespace gme{

class DocPrivate
{
private:
    friend  class Doc;
    DocPrivate(void);
    ~DocPrivate(void);
    std::vector<std::string>                    m_selectionVector;
    /** @brief  auto-focus.
    **/
    bool                                    m_bAutofocus;
    /** @brief auto-target to selection.
    **/
    bool                                    m_bAutoTarget;
    void        onSelectedChanged(void);
    /** @brief 记录当前状态。
    **/
    int                                     m_state;
    CacheFilm                               m_cacheFilm;

    enum{
        ST_STOPPED,
        ST_RUNNING,
        ST_PAUSED
    };
    ///@brief 指示了加载时的film文件。
    std::string                       m_filmPathfile;
    ///@brief 指示了film是否被有效更新过。
    bool                              m_fileFilmValid;
    ///@brief 指示了路径校正对象。
    boost::filesystem::gme_ext::fileFetcher m_fileFetcher;
public:
    typedef boost::function<void (int,int)>     type_imagesize_handler;
    typedef boost::function<void (void)>        type_state_handler;
    typedef boost::function<void (const std::string&)>  type_selection_handler;

    enum{
        STATE_OPEN,
        STATE_CLOSE,
        STATE_PAUSE,
        STATE_AUTOFOCUS_CHANGED,
        STATE_MAX
    };

    enum{
        SEL_ITEMSELECTED,
        SEL_ITEMDELSELECTED,
        SEL_ITEMCHILDADDED, //指示某个ObjectNode下新添加了孩子。
        SEL_ITEMSELFREMOVED, //指示某个ObjectNode自身被删除。
        SEL_MAX
    };

    inline void setFileFetcher(boost::filesystem::gme_ext::fileFetcher::type_fn_getfile fn)
    {
        m_fileFetcher.setGetFunction(fn);
    }

    inline std::string findFile(const std::string &srcPath,const std::string &basePath)
    {
        return m_fileFetcher.findFile(srcPath,basePath);
    }

    inline  CacheFilm&  cachefilm(void)
    {
        return m_cacheFilm;
    }

    inline  const std::string& filmFilePath(void)
    {
        return m_filmPathfile;
    }

    inline  void filmFilePath(const std::string &path,bool bFilmValid = true)
    {
        m_filmPathfile = path;
        m_fileFilmValid = bFilmValid;
    }

    ///@brief 将当前渲染状态保存到film状态文件中。
    bool    saveFilm(const std::string &path);

    inline  bool    isRunning(void)const
    {
        return (m_state == ST_RUNNING);
    }
    inline  bool    isStop(void)const
    {
        return (m_state == ST_STOPPED);
    }
    inline  bool    isPause(void)const
    {
        return (m_state == ST_PAUSED);
    }
    void    start(void);
    inline void    stop(void)
    {
        closeScene();
    }
    void    pause(void);

    ///@todo: 需要一个材质转化专家系统来支持材质转化。
	//
    boost::shared_ptr<slg::RenderSession>   m_session;

	ExtraTextureManager               texManager;
	ExtraMaterialManager              matManager;
	ExtraObjectManager                objManager;
	ExtraCameraManager                camManager;
    SingleEventListen<type_imagesize_handler>   imageSize_Evt;
    EventListen<int,type_state_handler>         state_Evt;
    EventListen<int,type_selection_handler>     selection_Evt;

    ///@brief 指示保存时是否需要强制导出。(由于我们导入了多submesh的模型).
    bool                              forceExport;
private:
    inline  void    clearAllListen(void)
    {
        imageSize_Evt.clear();
        state_Evt.clear();
        selection_Evt.clear();
    }
public:
    bool  getNativeRenderInfo(RenderInfo &ri);
    inline  bool    autoTarget(void)const
    {
        return m_bAutoTarget;
    }
    inline  void    autoTarget(bool at)
    {
        m_bAutoTarget = at;
    }
    inline  bool    autoFocus(void)const
    {
        return m_bAutofocus;
    }
    inline  void    autoFocus(bool af)
    {
        if(af != m_bAutofocus)
        {
            m_bAutofocus = af;
            state_Evt.fire(STATE_AUTOFOCUS_CHANGED);
        }
    }
    ///@brief selection members.
    inline  void    clearSelection(void)
    {
        BOOST_FOREACH(const std::string &key,m_selectionVector)
        {
            fireSelection(SEL_ITEMDELSELECTED,key);
        }
        m_selectionVector.clear();
    }
    inline  bool    addSelection(const std::string &oid)
    {
        if(std::find(m_selectionVector.begin(),m_selectionVector.end(),oid) == m_selectionVector.end())
        {
            m_selectionVector.push_back(oid);
            fireSelection(SEL_ITEMSELECTED,oid);
            onSelectedChanged();
            return true;
        }
        return false;
    }
    inline  std::vector<std::string>& getSelection(void)
    {
        return m_selectionVector;
    }

    inline  bool    removeSelection(const std::string &oid)
    {
        std::vector<std::string>::iterator  it = std::find(m_selectionVector.begin(),m_selectionVector.end(),oid);
        if(it != m_selectionVector.end())
        {
            fireSelection(SEL_ITEMDELSELECTED,oid);
            m_selectionVector.erase(it);
            return true;
        }
        return false;
    }
    inline  void    fireSelection(int state,const std::string &oid)
    {
        selection_Evt.fire(state,oid);
    }
public:
    //关闭当前打开场景。
    void    closeScene(void);

    inline slg::RenderSession*  getSession(void){
        return m_session.get();
    }

    inline  void    fireStateChanged(int state)
    {
        state_Evt.fire(state);
    }

    inline  void    fireSizeChanged(void)
    {
        imageSize_Evt.fire(m_session->film->GetWidth(),m_session->film->GetHeight());
    }
};

}



#endif  //GME_DM_DOC_H
