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

#ifndef  GME_DM_LOCALSETTING_H
#define  GME_DM_LOCALSETTING_H

#include "utils/option.h"
#include <Eigen/Core>

namespace gme{
namespace LocalSetting{

    Eigen::Vector3f     String2Vector(const std::string &value);
    std::string         Vector2String(const Eigen::Vector3f& vec);

    struct  Film
    {
        static  inline void getSize(float &w,float &h)
       {
            w = Option::instance().get("image.width",640.0f);
            h = Option::instance().get("image.height",480.0f);
        }
        static  inline void setSize(float w,float h)
        {
            gme::Option::instance().put("image.width",w);
            gme::Option::instance().put("image.height",h);
        }
    };

    struct  EnvironmentHDR
    {
        ///@todo implement scene.infinitelight.transformation setter/getter.
        ///@todo implement scene.infinitelight.shift setter/getter.
    public:
        static  inline bool hasHDR(void)
        {
            return Option::instance().is_existed("scene.infinitelight.file");
        }
        static  inline std::string  getFile(void)
        {
            return Option::instance().get<std::string>("scene.infinitelight.file","");
        }
        static  inline void  setFile(const std::string &file)
        {
            return Option::instance().put("scene.infinitelight.file",file);
        }
        static  inline bool isDefaultGamma(float g)
        {
            return (g == 2.2f);
        }
        static  inline float getGamma(void)
        {
            return Option::instance().get("scene.infinitelight.gamma",2.2f);
        }
        static  inline void setGamma(float g)
        {
            return Option::instance().put("scene.infinitelight.gamma",g);
        }
        static  inline std::string getGain(void)
        {
            return Option::instance().get("scene.infinitelight.gain","1.0 1.0 1.0");
        }
        static  inline void setGain(const std::string &value)
        {
            return Option::instance().put("scene.infinitelight.gain",value);
        }
    };

    struct  EnvironmentSky
    {
        ///@todo implement scene.skylight.transformation setter/getter.
    public:
        static  inline bool hasSky(void)
        {
            return Option::instance().is_existed("scene.skylight.dir");
        }
        static  inline std::string  getDir(void)
        {
            return Option::instance().get<std::string>("scene.skylight.dir","1.0 1.0 1.0");
        }
        static  inline void  setDir(const std::string &dir)
        {
            return Option::instance().put("scene.skylight.dir",dir);
        }
        static  inline bool  isDefaultTurbidity(float v)
        {
            return (v == 2.2f);
        }
        static  inline float getTurbidity(void)
        {
            return Option::instance().get("scene.skylight.turbidity",2.2f);
        }
        static  inline void setTurbidity(float t)
        {
            return Option::instance().put("scene.skylight.turbidity",t);
        }
        static  inline std::string getGain(void)
        {
            return Option::instance().get("scene.skylight.gain","1.0 1.0 1.0");
        }
        static  inline void setGain(const std::string &value)
        {
            return Option::instance().put("scene.skylight.gain",value);
        }
    };

    struct  EnvironmentSun
    {
        ///@todo implement scene.sunlight.transformation setter/getter.
    public:
        static  inline bool hasSun(void)
        {
            return Option::instance().is_existed("scene.sunlight.dir");
        }
        static  inline std::string  getDir(void)
        {
            return Option::instance().get<std::string>("scene.sunlight.dir","1.0 1.0 1.0");
        }
        static  inline void  setDir(const std::string &dir)
        {
            return Option::instance().put("scene.sunlight.dir",dir);
        }
        static  inline bool  isDefaultTurbidity(float v)
        {
            return (v == 2.2f);
        }
        static  inline float getTurbidity(void)
        {
            return Option::instance().get("scene.sunlight.turbidity",2.2f);
        }
        static  inline void setTurbidity(float t)
        {
            return Option::instance().put("scene.sunlight.turbidity",t);
        }
        static  inline bool  isDefaultRelsize(float v)
        {
            return (v == 1.0f);
        }
        static  inline float getRelsize(void)
        {
            return Option::instance().get("scene.sunlight.relsize",1.0f);
        }
        static  inline void setRelsize(float t)
        {
            return Option::instance().put("scene.sunlight.relsize",t);
        }
        static  inline std::string getGain(void)
        {
            return Option::instance().get("scene.sunlight.gain","1.0 1.0 1.0");
        }
        static  inline void setGain(const std::string &value)
        {
            return Option::instance().put("scene.sunlight.gain",value);
        }
    };

}
}

#endif //GME_DM_LOCALSETTING_H
