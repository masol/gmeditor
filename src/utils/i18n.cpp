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
#include "utils/i18n.h"
#include "utils/modulepath.h"
#include <boost/thread/tss.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>

#if WIN32
#ifndef NOMINMAX
	#define NOMINMAX
#endif 
#include <windows.h>
#endif


namespace gme
{

struct  Buffer_Info{
protected:
    /*一个通用缓冲区的大小*/
    static const size_t BUFFER_UNIT = 1024;
    char    *buffer;
    size_t buffer_len;
public:
    Buffer_Info() : buffer(new char[BUFFER_UNIT]),buffer_len(BUFFER_UNIT)
    {
    }
    ~Buffer_Info(){
        if(buffer){
            delete[] buffer;
            buffer = NULL;
        }
        buffer_len = 0;
    }
public:
    inline  size_t getBufferLen(void)const{
        return buffer_len;
    }
    inline  char*   getBuffer(size_t size = 0){
        if(size < buffer_len){
            return buffer;
        }
        size_t nbuf_len = BUFFER_UNIT * (size / BUFFER_UNIT + 1);
        delete[] buffer;
        buffer = new char[nbuf_len];
        buffer_len = nbuf_len;
        return buffer;
    }
};

#ifdef GME_NOTHREAD_SAFE
    static inline Buffer_Info&   getBufferInfo(void)
    {
        static Buffer_Info   lv_info;
        return lv_info;
    }
#else
    static inline Buffer_Info&   getBufferInfo(void)
    {
        static boost::thread_specific_ptr<Buffer_Info>       lv_infoStorage;
        if(!lv_infoStorage.get())
            lv_infoStorage.reset(new Buffer_Info());
        return *(lv_infoStorage.get());
    }
#endif


const char*
I18n::gettext(const char * msg)
{
    std::string ctx = boost::locale::gettext(msg);
    if(ctx.empty())
        return msg;
    char *line = getBufferInfo().getBuffer();
    if(ctx.length() >= getBufferInfo().getBufferLen() )
    {
        line = getBufferInfo().getBuffer(ctx.length() + 1);
    }
    strcpy(line,ctx.c_str());
    return line;

#if 0
    if(!m_pResBundle || !m_converter)
        return msg;

    int len;
    const UChar * uc = NULL;

    char *line = getBufferInfo().getBuffer();

    UErrorCode status = U_ZERO_ERROR;

    uc = ures_getStringByKey(m_pResBundle,msg, &len, &status);
    if(U_FAILURE(status))
    {
        return msg;
    }

    size_t outputLen = ucnv_fromUChars(m_converter, line,getBufferInfo().getBufferLen(), uc, len, &status);
    if(outputLen >= getBufferInfo().getBufferLen() )
    {
        line = getBufferInfo().getBuffer(outputLen + 1);
        outputLen = ucnv_fromUChars(m_converter, line,getBufferInfo().getBufferLen(),uc,len, &status);
    }
    if(U_FAILURE(status))
    {
        return msg;
    }

    return line;
#endif
}

bool
I18n::setLocale(const std::string &locale, const char* encoding)
{
    bool rt = false;
    //我们输出的目标一定是utf-8.然后由wx转义为系统接受的格式。
	boost::filesystem::path i18npath = ModulePath::instance().modulePath();
	i18npath /= "i18n";
	if(boost::filesystem::is_directory(i18npath))
    {
        boost::locale::generator gen;
        gen.add_messages_path(i18npath.string());
        gen.add_messages_domain("gme");
        //我们输出的目标一定是utf-8.然后由wx转义为系统接受的格式。
        m_translate_locale = gen(locale + '.' + encoding);
        rt = true;
    }
    return rt;
}



void
I18n::destroy(void)
{
}

void
I18n::assignSystemInfo(const char* lang)
{
    std::vector< std::string >    localArray;
    boost::split(localArray,lang,boost::is_any_of("."),boost::token_compress_on);
    if(localArray.size() == 2)
    {
        m_system_local = localArray[0];
        m_system_encoding = localArray[1];
    }else{
        //default value.
        m_system_local = "en_US";
        m_system_encoding = "UTF-8";
    }
}

void
I18n::initSystemInfo(void)
{
    const char* lang = NULL;
    if(!lang)
        lang = std::getenv("LC_CTYPE");
    if(!lang)
        lang = std::getenv("LC_ALL");
    if(!lang)
        lang = std::getenv("LANG");
#ifndef WIN32
    if(!lang)
        lang = "C";
    assignSystemInfo(lang);
    return;
#else
    if(lang) {
        assignSystemInfo(lang);
        return;
    }
    char buf[10];
    if(GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_SISO639LANGNAME,buf,sizeof(buf))==0)
    {
        assignSystemInfo("C");
        return;
    }
    std::string lc_name = buf;
    if(GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_SISO3166CTRYNAME,buf,sizeof(buf))!=0) {
        lc_name += "_";
        lc_name += buf;
    }
    if(GetLocaleInfoA(LOCALE_USER_DEFAULT,LOCALE_IDEFAULTANSICODEPAGE,buf,sizeof(buf))!=0) {
        if(atoi(buf)==0)
            lc_name+=".UTF-8";
        else {
            lc_name +=".windows-";
            lc_name +=buf;
        }
    }
    else {
        lc_name += "UTF-8";
    }
    assignSystemInfo(lc_name.c_str());
#endif
}

I18n::I18n()
{
    setLocale(this->getSystemLocal(),"UTF-8");
}

I18n::~I18n()
{
    destroy();
}


}
