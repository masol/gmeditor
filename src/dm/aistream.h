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

#ifndef  GME_DM_AISTREAM_H
#define  GME_DM_AISTREAM_H

#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "utils/pathext.h"

namespace gme{


class AiIOSystem : public Assimp::IOSystem
{
protected:
    std::string             m_lastPath;
public:
    static inline AiIOSystem*     create(void)
    {
       return new AiIOSystem();
    }
    AiIOSystem(void)
    {
    }

    virtual 	~AiIOSystem()
    {
    }

    virtual void 	Close (Assimp::IOStream *pFile)
    {
        delete pFile;
    }

    virtual bool 	ComparePaths (const char *one, const char *second) const
    {
        boost::filesystem::path pathone(one);
        boost::filesystem::path pathsecond(second);
        return (pathone == pathsecond);
    }
    bool 	ComparePaths (const std::string &one, const std::string &second) const
    {
        return ComparePaths(one.c_str(),second.c_str());
    }
    AI_FORCE_INLINE bool 	Exists (const std::string &pFile) const
    {
        return boost::filesystem::exists(pFile);
    }
    virtual bool 	Exists (const char *pFile) const
    {
        return boost::filesystem::exists(pFile);
    }
    virtual char 	getOsSeparator () const
    {
        return boost::filesystem::gme_ext::getSeparator();
    }
    virtual Assimp::IOStream * 	Open (const char *pFile, const char *pMode="rb")
    {
        std::string   file(pFile);
        return Open(file,pMode);
    }
    Assimp::IOStream * 	Open (const std::string &pFile, const std::string &pMode=std::string("rb"));
};


}


#endif //GME_DM_AISTREAM_H

