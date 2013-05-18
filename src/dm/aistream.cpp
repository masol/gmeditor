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
#include "aistream.h"


namespace gme
{

class AiStream : public Assimp::IOStream
{
    friend class AiIOSystem;
protected:
    boost::filesystem::fstream  m_stream;
    AiStream(const std::string &filepath,const std::string &mode)
    {
        std::ios_base::openmode om = std::ios_base::in;
        if(mode.find('w') != std::string::npos)
        {
            om |= std::ios_base::out;
        }
        if(mode.find('r') != std::string::npos)
        {
            om |= std::ios_base::in;
        }
        if(mode.find('b') != std::string::npos)
        {
            om |= std::ios_base::binary;
        }
        if(mode.find('+') != std::string::npos)
        {
//            om |= std::ios_base::trunc;
        }
        if(mode.find('a') != std::string::npos)
        {
            om &= ~std::ios_base::trunc;
        }
        m_stream.open(filepath.c_str(),om);
    }
public:
    size_t  getFileSize(void)
    {
        size_t oldpos = m_stream.tellg();
        m_stream.seekg (0, m_stream.end);
        size_t fileSize = m_stream.tellg();
        m_stream.seekg (oldpos, m_stream.beg);
        return fileSize;
    }

    virtual size_t 	FileSize () const
    {
        if(m_stream.is_open())
        {
            return const_cast<AiStream*>(this)->getFileSize();
        }
        return 0;
    }
    virtual void 	Flush ()
    {
        if(m_stream.is_open())
        {
            m_stream.flush();
        }
    }
    virtual size_t 	Read (void *pvBuffer, size_t pSize, size_t pCount)
    {
        if(m_stream.is_open())
        {
            m_stream.read((char*)pvBuffer,pSize * pCount);
            return m_stream.gcount();
        }
        return 0;
    }
    virtual aiReturn 	Seek (size_t pOffset, aiOrigin pOrigin)
    {
        if(m_stream.is_open())
        {
            std::ios_base::seekdir  sd;
            if(pOrigin == aiOrigin_SET)
            {
                sd = std::ios_base::beg;
            }else if(pOrigin == aiOrigin_CUR)
            {
                sd = std::ios_base::cur;
            }else if(pOrigin == aiOrigin_END)
            {
                sd = std::ios_base::end;
            }else{
                BOOST_ASSERT_MSG(false,"invalid position value");
            }
            m_stream.seekg(pOffset,sd);
            return aiReturn_SUCCESS;
        }
        return aiReturn_FAILURE;
    }
    size_t  getTell(void)
    {
        return m_stream.tellg();
    }
    virtual size_t 	Tell () const
    {
        if(m_stream.is_open())
        {
            return const_cast<AiStream*>(this)->getTell();
        }
        return 0;
    }

    virtual size_t 	Write (const void *pvBuffer, size_t pSize, size_t pCount)
    {
        if(m_stream.is_open())
        {
            m_stream.write((char*)pvBuffer,pSize * pCount);
            return m_stream.gcount();
        }
        return 0;
    }
    virtual 	~AiStream ()
    {
        if(m_stream.is_open())
        {
            m_stream.close();
        }
    }
};

Assimp::IOStream*
AiIOSystem::Open(const std::string &pFile, const std::string &pMode)
{
    //for object mtl reading.
    if(m_lastPath.empty())
    {
        m_lastPath = boost::filesystem::current_path().string();
    }
    boost::filesystem::path path = boost::filesystem::absolute(pFile,m_lastPath);
    m_lastPath = path.parent_path().string();
    return new AiStream(path.string(),pMode);
}

}

