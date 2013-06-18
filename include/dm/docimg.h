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

#ifndef  GME_DM_DOCIMG_H
#define  GME_DM_DOCIMG_H


#include "dm/doc.h"
#include "utils/eventlisten.h"
#include <boost/function.hpp>

namespace gme{

struct  ImageDataBase{
    enum{
        ID_INVALID,
        ID_SCALE,
        ID_SCROLL
    };
    const   int     type;
    unsigned char* data;
    int     width;
    int     height;
    int     pitch;
    ImageDataBase(int t) : type(t)
    {
    }
};

struct  ImageDataScale : public ImageDataBase
{
    typedef ImageDataBase   inherited;
    /** @brief 返回有效区域。由getData设置。
	 *
	*/
    int                 left;
    int                 top;
    int                 right;
    int                 bottom;
    unsigned    char    default_red;
    unsigned    char    default_green;
    unsigned    char    default_blue;
    ImageDataScale() : inherited(ImageDataBase::ID_SCALE)
    {
    }
};

struct ImageDataScroll : public ImageDataBase
{
    typedef ImageDataBase   inherited;
    int     left;
    int     top;
    int     right;
    int     bottom;
    ImageDataScroll() : inherited(ImageDataBase::ID_SCROLL)
    {
    }
};

class DocImg : public DocScopeLocker
{
public:
    typedef boost::function<void (int,int)>     type_imagesize_handler;
protected:
    bool    getData(ImageDataScale *pdata,int w, int h,const float* pixels);
public:
    struct  ViewPort{
        int   x;
        int   y;
        int   width;
        int   height;
    };

    ///@brief get/set screen refresh interval in micro-second.
    int     getScreenRefreshInterval(void);
    void    setScreenRefreshInterval(int ms);

    ///@brief 绘制阳光方向。
    void    drawSkylightDir(ViewPort &vp);

    ///@brief 绘制选中的对象。
    void    drawSelectedObject(ViewPort &vp);
    enum{
        RI_NATIVE,
        RI_CONTRIBUTE,
        RI_TOTAL
    };
    bool    getRenderInfo(int type,RenderInfo &ri);
    bool    getSize(int &w,int &h);
    bool    setSize(int w,int h);
    void    onImagesizeChanged(type_imagesize_handler handler);

    ///@brief 获取渲染时间。
    double  getRenderTime(void);

    ///@brief for glrender.
    void            updateNative(void);
    const float*    getPixels(void);
    ///@brief save the image result.
    bool    saveImage(const std::string &fullpath);

    ///@brief get a image data, so we can draw thumbnail.
    bool    getImage(const std::string& filepath,int &w,int &h,int &c,const float*  &pixels);

    /**@brief for software render.
     *@deprecated : not support software render.
    **/
    bool    getData(ImageDataBase *pdata);

    bool    hasContribute(void);
    void    invalidContribute(void);
};

}

#endif //GME_DM_DOCIMG_H

