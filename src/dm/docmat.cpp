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
#include <boost/assert.hpp>
#include <boost/scope_exit.hpp>
#include <boost/filesystem/fstream.hpp>
#include "slg/slg.h"
#include "slg/sdl/material.h"
#include "utils/rapidxml_print.hpp"
#include "dm/docmat.h"
#include "docprivate.h"
#include "slgmaterial.h"


namespace gme{
//texture
const   int  DocMat::CONST_FLOAT = slg::CONST_FLOAT;
const   int  DocMat::CONST_FLOAT3 = slg::CONST_FLOAT3;
const   int  DocMat::IMAGEMAP = slg::IMAGEMAP;
const   int  DocMat::SCALE_TEX = slg::SCALE_TEX;
const   int  DocMat::FRESNEL_APPROX_N = slg::FRESNEL_APPROX_N;
const   int  DocMat::FRESNEL_APPROX_K = slg::FRESNEL_APPROX_K;
const   int  DocMat::MIX_TEX = slg::MIX_TEX;
const   int  DocMat::ADD_TEX = slg::ADD_TEX;
const   int  DocMat::CHECKERBOARD2D = slg::CHECKERBOARD2D;
const   int  DocMat::CHECKERBOARD3D = slg::CHECKERBOARD3D;
const   int  DocMat::FBM_TEX = slg::FBM_TEX;
const   int  DocMat::MARBLE = slg::MARBLE;
const   int  DocMat::DOTS = slg::DOTS;
const   int  DocMat::BRICK = slg::BRICK;
const   int  DocMat::WINDY = slg::WINDY;
const   int  DocMat::WRINKLED = slg::WRINKLED;
const   int  DocMat::UV_TEX = slg::UV_TEX;
const   int  DocMat::BAND_TEX = slg::BAND_TEX;
const   int  DocMat::TEX_DISABLE = slg::BAND_TEX + 0x1000;
const   int  DocMat::TEX_IES = DocMat::TEX_DISABLE + 1;

std::string
DocMat::texGetTypeNameFromType(int type)
{
    switch(type)
    {
    case slg::CONST_FLOAT:
        return "constfloat1";
    case slg::CONST_FLOAT3:
        return "constfloat3";
    case slg::IMAGEMAP:
        return "imagemap";
    case slg::SCALE_TEX:
        return "scale";
    case slg::FRESNEL_APPROX_N:
        return "fresnelapproxn";
    case slg::FRESNEL_APPROX_K:
        return "fresnelapproxk";
    case slg::MIX_TEX:
        return "mix";
    case slg::ADD_TEX:
        return "add";
    case slg::CHECKERBOARD2D:
        return "checkerboard2d";
    case slg::CHECKERBOARD3D:
        return "checkerboard3d";
    case slg::FBM_TEX:
        return "fbm";
    case slg::MARBLE:
        return "marble";
    case slg::DOTS:
        return "dots";
    case slg::BRICK:
        return "brick";
    case slg::WINDY:
        return "windy";
    case slg::WRINKLED:
        return "wrinkled";
    case slg::UV_TEX:
        return "uv";
    case slg::BAND_TEX:
        return "band";
    }
    BOOST_ASSERT_MSG(false,"unreachable code!");
    return "";
}

int
DocMat::brickbondTypeFromName(const std::string &strval)
{
    if(strval == "flemish")
    {
        return slg::FLEMISH;
    }else if(strval == "running")
    {
        return slg::RUNNING;
    }else if(strval == "english")
    {
        return slg::ENGLISH;
    }else if(strval == "basket")
    {
        return slg::BASKET;
    }else if(strval == "herringbone")
    {
        return slg::HERRINGBONE;
    }else if(strval == "chain link")
    {
        return slg::KETTING;
    }else{
        BOOST_ASSERT_MSG(false,"invalid brickbond");
    }
    return slg::RUNNING;
}


int
DocMat::texGetTypeFromTypeName(const std::string &name)
{
    if(name == "constfloat1")
    {
        return slg::CONST_FLOAT;
    }else if(name == "constfloat3")
    {
        return slg::CONST_FLOAT3;
    }else if(name == "imagemap")
    {
        return slg::IMAGEMAP;
    }else if(name == "scale")
    {
        return slg::SCALE_TEX;
    }else if(name == "fresnelapproxn")
    {
        return slg::FRESNEL_APPROX_N;
    }else if(name == "fresnelapproxk")
    {
        return slg::FRESNEL_APPROX_K;
    }else if(name == "mix")
    {
        return slg::MIX_TEX;
    }else if(name == "add")
    {
        return slg::ADD_TEX;
    }else if(name == "checkerboard2d")
    {
        return slg::CHECKERBOARD2D;
    }else if(name == "checkerboard3d")
    {
        return slg::CHECKERBOARD3D;
    }else if(name == "fbm")
    {
        return slg::FBM_TEX;
    }else if(name == "marble")
    {
        return slg::MARBLE;
    }else if(name == "dots")
    {
        return slg::DOTS;
    }else if(name == "brick")
    {
        return slg::BRICK;
    }else if(name == "windy")
    {
        return slg::WINDY;
    }else if(name == "wrinkled")
    {
        return slg::WRINKLED;
    }else if(name == "uv")
    {
        return slg::UV_TEX;
    }else if(name == "band")
    {
        return slg::BAND_TEX;
    }
    //BOOST_ASSERT_MSG(false,"unreachable code!");
    return INVALID_TEXTURE;
}



//material
const   int  DocMat::MATTE = slg::MATTE;
const   int  DocMat::MIRROR = slg::MIRROR;
const   int  DocMat::GLASS = slg::GLASS;
const   int  DocMat::METAL = slg::METAL;
const   int  DocMat::ARCHGLASS = slg::ARCHGLASS;
const   int  DocMat::MIX = slg::MIX;
const   int  DocMat::NULLMAT = slg::NULLMAT;
const   int  DocMat::MATTETRANSLUCENT = slg::MATTETRANSLUCENT;
const   int  DocMat::GLOSSY2 = slg::GLOSSY2;
const   int  DocMat::METAL2 = slg::METAL2;
//const   int  DocMat::GLOSSY2_ANISOTROPIC = slg::GLOSSY2_ANISOTROPIC;
//const   int  DocMat::GLOSSY2_ABSORPTION = slg::GLOSSY2_ABSORPTION;
//const   int  DocMat::GLOSSY2_INDEX = slg::GLOSSY2_INDEX;
//const   int  DocMat::GLOSSY2_MULTIBOUNCE = slg::GLOSSY2_MULTIBOUNCE;
//const   int  DocMat::METAL2_ANISOTROPIC = slg::METAL2_ANISOTROPIC;


std::string
DocMat::getTypeNameFromType(int type)
{
    switch(type)
    {
    case MATTE:
        return "matte";
    case MIRROR:
        return "mirror";
    case GLASS:
        return "glass";
    case METAL:
        return "metal";
    case ARCHGLASS:
        return "archglass";
    case MIX:
        return "mix";
    case NULLMAT:
        return "null";
    case MATTETRANSLUCENT:
        return "mattetranslucent";
    case GLOSSY2:
        return "glossy2";
    case METAL2:
        return "metal2";
    }
    BOOST_ASSERT_MSG(false,"unreachable code!");
    return "";
}

int
DocMat::getTypeFromTypeName(const std::string &name)
{
    if(name == "matte")
    {
        return MATTE;
    }else if(name == "mirror")
    {
        return MIRROR;
    }else if(name == "glass")
    {
        return GLASS;
    }else if(name == "metal")
    {
        return METAL;
    }else if(name == "archglass")
    {
        return ARCHGLASS;
    }else if(name == "mix")
    {
        return MIX;
    }else if(name == "null")
    {
        return NULLMAT;
    }else if(name == "mattetranslucent")
    {
        return MATTETRANSLUCENT;
    }else if(name == "glossy2")
    {
        return GLOSSY2;
    }else if(name == "metal2")
    {
        return METAL2;
    }
    //BOOST_ASSERT_MSG(false,"unreachable code!");
    return INVALID_MATERIAL;
}


//std::string&
//DocMat::getMatName(const std::string& id)
//{
//    return pDocData->matManager.get(id);
//}

bool
DocMat::saveMaterial(const std::string &matid,const std::string &filepath,bool bExportRes)
{
    const slg::Material* pMat = ExtraMaterialManager::getSlgMaterial(matid);
    if(!pMat)
        return false;

    boost::filesystem::ofstream   ofstream;
    ofstream.open(filepath.c_str());
    if(ofstream.is_open())
    {
        BOOST_SCOPE_EXIT( (&ofstream))
        {
            ofstream.close();
        }
        BOOST_SCOPE_EXIT_END

        saveMaterialImpl(pMat,ofstream,filepath,bExportRes);
        return true;
    }
    return false;
}

const char*     DocMat::CLIPBOARD_MAGIC = "GMEDATA";

bool
DocMat::saveMaterialToString(const std::string &matid,std::string &content)
{
    const slg::Material* pMat = ExtraMaterialManager::getSlgMaterial(matid);
    if(!pMat)
        return false;

    std::stringstream   ss;
    ss << CLIPBOARD_MAGIC;
    saveMaterialImpl(pMat,ss,boost::filesystem::current_path().string(),false);
    content = ss.str();
    return true;
}


bool
DocMat::loadMaterial(const ObjectNode *pNode,const std::string &path)
{
    const std::string &matid = pNode->matid();

    if(matid.empty())
        return false;

    slg::Material   *pOldSlgMat = ExtraMaterialManager::getSlgMaterial(matid);
    if(!pOldSlgMat)
        return false;


    boost::filesystem::ifstream file(path,std::ifstream::binary);
    if(file) {
        BOOST_SCOPE_EXIT( (&file))
        {
            file.close();
        }BOOST_SCOPE_EXIT_END
        // get length of file:
        file.seekg (0, file.end);
        std::streamoff length = file.tellg();
        file.seekg (0, file.beg);

        char * buffer = new char [length + 1];
        BOOST_SCOPE_EXIT( (buffer))
        {
            delete[] buffer;
        }BOOST_SCOPE_EXIT_END
        // read data as a block:
        file.read (buffer,length);
        // ...buffer contains the entire file...
        buffer[length] = 0;
        try{
            return loadMaterialImpl(pNode,pOldSlgMat,buffer,path);
        }catch(std::exception &e)
        {
            Doc::SysLog(Doc::LOG_ERROR,boost::str(boost::format(__("在加载文件'%s'时发生异常:%s"))%path % e.what() ) );
        }
    }
    return false;
}

bool
DocMat::loadMaterialFromString(const ObjectNode *pNode,std::string &content)
{
    if(!boost::starts_with(content,CLIPBOARD_MAGIC))
        return false;

    const std::string &matid = pNode->matid();
    if(matid.empty())
        return false;

    slg::Material   *pOldSlgMat = ExtraMaterialManager::getSlgMaterial(matid);
    if(!pOldSlgMat)
        return false;


    char* buffer = const_cast<char*>(content.c_str());
    buffer = &buffer[strlen(CLIPBOARD_MAGIC)];
    return loadMaterialImpl(pNode,pOldSlgMat,buffer,boost::filesystem::current_path().string());
}


bool
DocMat::loadMaterialImpl(const ObjectNode *pNode,slg::Material *pOldSlgMat,char* matContent,const std::string &path)
{
    const std::string &matid = pNode->matid();
    type_xml_doc    doc;
    const int flag = NS_RAPIDXML::parse_no_element_values | NS_RAPIDXML::parse_trim_whitespace;

    doc.parse<flag>(matContent);
    type_xml_node   *pMat = doc.first_node(constDef::material);
    if(pMat)
    {
        //模版中不能有id,删除所有id属性。
        recursion_remove_attribute(pMat,constDef::id);
        SlgUtil::Editor editor(pDocData->getSession());
        ImportContext   ctx(pDocData->getSession()->renderConfig->scene,path);


        std::stringstream ss;
        std::string     realMatId = matid;
        if(pDocData->matManager.loadMaterial(ss,ctx,realMatId,*pMat))
        {
            pDocData->matManager.onMaterialRemoved(pOldSlgMat);

            luxrays::Properties prop;
            prop.LoadFromString(ss.str());
            //in any case,we need update root material.
            editor.scene()->UpdateMaterial(matid,prop);
            const slg::Material *newMat = editor.scene()->matDefs.GetMaterial(matid);

            SlgMaterial2Name mat2name;
            SlgTexture2Name tex2name;
            pDocData->matManager.updateMaterialInfo(newMat,mat2name,tex2name);
            editor.addAction(ctx.getAction());
            pDocData->fireSelection(DocPrivate::SEL_ITEMMATUPDATED,pNode->id());
            pDocData->setModified();
            return true;
        }
    }
    return false;
}

void
DocMat::saveMaterialImpl(const slg::Material* pMat,std::ostream &os,const std::string &filepath,bool bExportRes)
{
    ///boost自带的rapidxml没有print函数.....加入到utils中。
    os << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
    std::ostream_iterator<char>   oit(os);
    type_xml_doc    xmldoc;

    int flags = dumpContext::DUMP_NORMAL;
    if(bExportRes)
    {
        flags = (dumpContext::DUMP_SAVECTM | dumpContext::DUMP_COPYRES);
    }
    boost::filesystem::path path(filepath);
    boost::filesystem::path root_path = path.parent_path();
    dumpContext     ctx(flags,root_path);

    pDocData->matManager.dump(xmldoc,pMat,ctx);

    //模版中不能有id,删除所有id属性。
    recursion_remove_attribute(&xmldoc,constDef::id);

    rapidxml::print(oit,xmldoc);
}


int
DocMat::updateProperty(const std::vector<std::string> &keyPath,const std::string &value,type_xml_node &parent,boost::function<bool (std::string &)> &getImageFile)
{
    int ret = UPDATE_DENY;
    if(keyPath.size() > 0)
    {
        // get material first.
        const std::string &matId = keyPath[0];
        slg::Material   *pMat = ExtraMaterialManager::getSlgMaterial(matId);
        if(pMat)
        {
            try{
                SlgUtil::Editor     editor(pDocData->m_session.get());
                SlgUtil::UpdateContext  context(editor,parent,value,keyPath,getImageFile);
                ExtraMaterialManager &matManager = pDocData->matManager;
                matManager.dump(context.props,pMat);
                if(matManager.updateMaterial(context,pMat,1) && !context.bVeto)
                {
                    matManager.onMaterialRemoved(pMat);
//                    std::cerr << "context.props = " << context.props.ToString() << std::endl;

                    //in any case,we need update root material.
                    editor.scene()->UpdateMaterial(matId,context.props);
                    const slg::Material *newMat = editor.scene()->matDefs.GetMaterial(matId);

                    SlgMaterial2Name mat2name;
                    SlgTexture2Name tex2name;
                    matManager.updateMaterialInfo(newMat,mat2name,tex2name);


                    if(context.bGenNode)
                    {//开始创建parent.
                        dumpContext dumpCtx(dumpContext::DUMP_NORMAL,boost::filesystem::current_path());
                        if(context.idIsMat)
                        {
                            pMat = matManager.getSlgMaterial(context.updatedId);
                            BOOST_ASSERT_MSG(pMat,"material with updatedId not exist?");
                            matManager.dump(parent,pMat,dumpCtx);
                            ret = UPDATE_REFRESH_MAT;
                        }else{
                            const slg::Texture *pTex = ExtraMaterialManager::getTextureFromKeypath(newMat,keyPath,1);
                            if(pTex)
                            {//注意Null Texture.
                                pDocData->texManager.dump(parent,constDef::texture,pTex,dumpCtx);
                            }
                            ret = UPDATE_REFRESH_TEX;
                        }
                    }else{
                        ret = UPDATE_ACCEPT;
                    }

                    if(context.bVeto)
                    {
                        editor.resetAction();
                    }else{
                        pDocData->setModified();
                        editor.addAction(slg::MATERIALS_EDIT);
                        if (ExtraMaterialManager::materialIsLight(newMat))
                            editor.addAction(slg::AREALIGHTS_EDIT);
                    }
                }
            }catch(std::exception e)
            {
                //error occupy.we stop and start session here. caused by opencl
                pDocData->m_session->Stop();
                pDocData->m_session->Start();
                pDocData->cachefilm().invalidate();
            }
            //ret = ExtraMaterialManager::updateMaterial(editor,pMat,keyPath,0,value,parent);
        }
    }

    return ret;
}


bool
DocMat::updateMaterial(const std::string &id,const std::string &slgMatDef)
{
    slg::Scene  *scene;
    if(pDocData->m_session.get() && (scene = pDocData->m_session->renderConfig->scene))
    {
        luxrays::Properties props;
        props.LoadFromString(slgMatDef);

        //first,define texture.
        scene->DefineTextures(props);

        //then, update material.
        scene->UpdateMaterial(id,props);

        //last,check the material used by othter material and update it!
        pDocData->setModified();
        return true;
    }
	return false;
}

type_xml_node*
DocMat::getMaterial(const std::string &id,type_xml_node &parent)
{
    slg::Material* pMat = ExtraMaterialManager::getSlgMaterial(id);
    if(pMat)
    {
        dumpContext   ctx(dumpContext::DUMP_NORMAL,boost::filesystem::current_path());
        return pDocData->matManager.dump(parent,pMat,ctx);
    }
    return NULL;
}


} //end namespace gme.
