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

#ifndef  GME_DM_XMLUTIL_H
#define  GME_DM_XMLUTIL_H


#include <boost/property_tree/detail/rapidxml.hpp>
#include "utils/MD5.h"
#include "utils/strext.h"
#include <boost/filesystem.hpp>
#include <boost/unordered_map.hpp>

#define    NS_RAPIDXML      boost::property_tree::detail::rapidxml

namespace gme
{
    typedef    boost::property_tree::detail::rapidxml::xml_document<char>       type_xml_doc;
    typedef	   boost::property_tree::detail::rapidxml::xml_node<char>	        type_xml_node;
    typedef	   boost::property_tree::detail::rapidxml::xml_attribute<char>		type_xml_attr;

    /** @brief xml导出/导入时使用的常量。用于减少rapidxml的内存操作。
     * @details 在objectnode.cpp中定义。
    **/
    struct  constDef{
        static  const char* object;
        static  const char* material;
        static  const char* texture;
        static  const char* id;
        static  const char* name;
        static  const char* type;
        static  const char* mapping;
        static  const char* ctxmd5;
        static  const char* transformation;
		static	const char* position;
		static	const char* file;
		static	const char* groupfile;
		static  const char* emission;
		static  const char* bumptex;
		static  const char* normaltex;
		static  const char* kd;
		static  const char* kr;
		static  const char* kt;
		static  const char* ioroutside;
		static  const char* iorinside;
		static  const char* exp;
		static  const char* amount;
		static  const char* ks;
		static  const char* uroughness;
		static  const char* vroughness;
		static  const char* ka;
		static  const char* d;
		static  const char* index;
		static  const char* n;
		static  const char* k;
		static  const char* camera;
		static  const char* active;
		static  const char* target;
		static  const char* up;
		static  const char* fieldOfView;
		static  const char* clipHither;
		static  const char* clipYon;
		static  const char* lensRadius;
		static  const char* focalDistance;
		static  const char* lights;
		static  const char* envlight;
		static  const char* sunlight;
        static  const char* film;
    };

    //inline const char* getFilepathFromDocument(const type_xml_node &node)
    //{
    //    type_xml_doc *pDoc = node.document();
    //    //BOOST_ASSERT_MSG(pDoc != NULL,"invalid node,must from doc");
    //    if(pDoc)
    //    {
    //        type_xml_attr  *fileAttr = pDoc->first_attribute(constDef::file);
    //        if(fileAttr)
    //        {
    //            return fileAttr->value();
    //        }
    //    }
    //    return NULL;
    //}

    inline std::string getIdFromNode(const type_xml_node &node)
    {
        type_xml_attr  *idAttr = node.first_attribute(constDef::id);
        if(idAttr)
        {
            return idAttr->value();
        }
        return string::uuid_to_string(boost::uuids::random_generator()());
    }

    struct  dumpContext
    {
    private:
        /** @brief 保存了图像加载后内容的md5string到文件名的映射。只用于imageTexture.
        **/
        typedef   boost::unordered_map<std::string, std::string>        type_ctx2filepath;
        type_ctx2filepath       m_texctx2filepath;

        /** @brief 保存了模型文件加载后内容的md5string到文件名的映射。这允许我们优化模型文件。
        **/
        type_ctx2filepath       m_objctx2filepath;
    public:
        ///@brief dump flags.
        static  const  int DUMP_NORMAL = 0;
        static  const  int DUMP_GENMD5 = 1;
        static  const  int DUMP_SAVECTM = 2;
        static  const  int DUMP_COPYRES = 4;
        static  const  int DUMP_OPTMIZE_TEX = 8;

        boost::filesystem::path     target;
        const int                   flags;

        inline const std::string*  queryObjFilepath(const std::string &ctxmd5)
        {
            type_ctx2filepath::iterator it = m_objctx2filepath.find(ctxmd5);
            if(it != m_objctx2filepath.end())
            {
                return &(it->second);
            }
            return NULL;
        }
        inline  void    addObjMapper(const std::string &objmd5,const std::string &path)
        {
            m_objctx2filepath[objmd5] = path;
        }

        inline const std::string*  queryTexFilepath(const std::string &ctxmd5)
        {
            type_ctx2filepath::iterator it = m_texctx2filepath.find(ctxmd5);
            if(it != m_texctx2filepath.end())
            {
                return &(it->second);
            }
            return NULL;
        }
        inline  void    addTexMapper(const std::string &imagemd5,const std::string &path)
        {
            m_texctx2filepath[imagemd5] = path;
        }
        inline  bool    isOptmizeTex(void)const
        {
            return (flags & DUMP_OPTMIZE_TEX) != 0;
        }
        inline  bool    isGenerateMD5(void)const
        {
            return (flags & DUMP_GENMD5) != 0;
        }
        inline  bool    isCopyResource(void)const
        {
            return (flags & DUMP_COPYRES) != 0;
        }
        inline  bool    isSvaeCtm(void)const
        {
            return (flags & DUMP_SAVECTM) != 0;
        }
    public:
        dumpContext(int f,const boost::filesystem::path &t) : target(t),flags(f)
        {
        }
    };

    struct  conditional_md5
    {
    private:
        bool    m_update;
        MD5     m_md5;
    public:
        conditional_md5(const dumpContext& ctx)
        {
            m_update = ctx.isGenerateMD5();
        }
        inline void update(const unsigned char *buf, MD5::size_type length)
        {
            if(m_update)
                m_md5.update(buf,length);
        }
        inline void update(const char *buf, MD5::size_type length)
        {
            if(m_update)
                m_md5.update(buf,length);
        }
        inline std::string hexdigest()
        {
            if(m_update)
                return m_md5.finalize().hexdigest();
            return "";
        }
        inline bool isGenerateMD5(void)const
        {
            return m_update;
        }
		/** @brief 从子节点中搜索ctxmd5属性，并更新进入this md5.
		**/
        inline bool updateChild(type_xml_node *pChild)
        {
       		if(isGenerateMD5())
            {
                type_xml_attr *pAttr = pChild->first_attribute(constDef::ctxmd5);
                BOOST_ASSERT_MSG(pAttr,"invalid pAttr usage.");
                if(pAttr)
                {
                    m_md5.update(pAttr->value(),(MD5::size_type)pAttr->value_size());
                    return true;
                }
            }
            return false;
        }
    };

    inline type_xml_node*    find_child(type_xml_node *pNode,const std::string &tagName)
    {
        if(tagName == pNode->name())
        {
            return pNode;
        }
        type_xml_node   *child = pNode->first_node();
        while(child)
        {
            type_xml_node* founded = find_child(child,tagName);
            if(founded)
                return founded;
            child = child->next_sibling();
        }
        return NULL;
    }

    inline char* allocate_string(type_xml_doc *pDoc,const std::string &value)
    {
        return pDoc->allocate_string(value.c_str(),value.length() + 1);
    }

    /** @brief 分配attribute节点。key是常量，并且value需要复制。
    **/
    inline type_xml_attr* allocate_attribute(type_xml_doc *pDoc,const char* key,const std::string &value)
    {
        return pDoc->allocate_attribute(key,allocate_string(pDoc,value));
    }

    inline type_xml_attr* allocate_attribute_withkey(type_xml_doc *pDoc,const std::string &key,const std::string &value)
    {
        return pDoc->allocate_attribute(allocate_string(pDoc,key),allocate_string(pDoc,value));
    }
}

#endif //GME_DM_XMLUTIL_H
