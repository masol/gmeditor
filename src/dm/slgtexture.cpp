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
#include "dm/doc.h"
#include "slgtexture.h"
#include "docprivate.h"
#include "slg/slg.h"
#include "luxrays/luxrays.h"
#include "luxrays/core/exttrianglemesh.h"
#include <boost/filesystem.hpp>
#include "utils/MD5.h"
#include "utils/strext.h"
#include "slgutils.h"
#include <boost/format.hpp>

namespace gme{

SlgTexture2Name::SlgTexture2Name(void)
{
    slg::Scene  *scene = Doc::instance().pDocData->getSession()->renderConfig->scene;
    m_textureNameArray = scene->texDefs.GetTextureNames ();
    m_texIdx2NameIdx.resize(m_textureNameArray.size());
    u_int   nameIdx = 0;
    for(std::vector< std::string >::const_iterator it = m_textureNameArray.begin(); it < m_textureNameArray.end(); ++it,++nameIdx)
    {
        m_texIdx2NameIdx[scene->texDefs.GetTextureIndex(*it)] = nameIdx;
    }
}

const std::string&
SlgTexture2Name::getTextureName(const slg::Texture* ptex)
{
    slg::Scene  *scene = Doc::instance().pDocData->getSession()->renderConfig->scene;
    u_int matNameIdx = m_texIdx2NameIdx[scene->texDefs.GetTextureIndex(ptex)];
    return m_textureNameArray[matNameIdx];
}

void
ExtraTextureManager::updateTextureInfo(const slg::Texture *pTex,SlgTexture2Name &tex2name)
{
    const std::string &id = tex2name.getTextureName(pTex);
    if(!id.empty())
    {
        m_tex2id[pTex] = id;
    }

    switch(pTex->GetType())
    {
    case slg::CONST_FLOAT3:
        break;
    case slg::CONST_FLOAT:
        break;
    case slg::IMAGEMAP:
        break;
    case slg::SCALE_TEX:
        {
            const slg::ScaleTexture*   pRealTex = dynamic_cast<const slg::ScaleTexture*>(pTex);
            updateTextureInfo(pRealTex->GetTexture1(),tex2name);
            updateTextureInfo(pRealTex->GetTexture2(),tex2name);
        }
        break;
    case slg::FRESNEL_APPROX_N:
        {
            const slg::FresnelApproxNTexture*   pRealTex = dynamic_cast<const slg::FresnelApproxNTexture*>(pTex);
            updateTextureInfo(pRealTex->GetTexture(),tex2name);
        }
        break;
    case slg::FRESNEL_APPROX_K:
        {
            const slg::FresnelApproxKTexture*   pRealTex = dynamic_cast<const slg::FresnelApproxKTexture*>(pTex);
            updateTextureInfo(pRealTex->GetTexture(),tex2name);
        }
        break;
    case slg::MIX_TEX:
        {
            const slg::MixTexture*   pRealTex = dynamic_cast<const slg::MixTexture*>(pTex);
            updateTextureInfo(pRealTex->GetTexture1(),tex2name);
            updateTextureInfo(pRealTex->GetTexture2(),tex2name);
            updateTextureInfo(pRealTex->GetAmountTexture(),tex2name);
        }
        break;
    case slg::ADD_TEX:
        {
            const slg::AddTexture*   pRealTex = dynamic_cast<const slg::AddTexture*>(pTex);
            updateTextureInfo(pRealTex->GetTexture1(),tex2name);
            updateTextureInfo(pRealTex->GetTexture2(),tex2name);
        }
        break;
    case slg::CHECKERBOARD2D:
        {
            const slg::CheckerBoard2DTexture*   pRealTex = dynamic_cast<const slg::CheckerBoard2DTexture*>(pTex);
            updateTextureInfo(pRealTex->GetTexture1(),tex2name);
            updateTextureInfo(pRealTex->GetTexture2(),tex2name);
        }
        break;
    case slg::CHECKERBOARD3D:
        {
            const slg::CheckerBoard3DTexture*   pRealTex = dynamic_cast<const slg::CheckerBoard3DTexture*>(pTex);
            updateTextureInfo(pRealTex->GetTexture1(),tex2name);
            updateTextureInfo(pRealTex->GetTexture2(),tex2name);
        }
        break;
    case slg::FBM_TEX:
    case slg::MARBLE:
        //has no child.
        break;
    case slg::DOTS:
        {
            const slg::DotsTexture*   pRealTex = dynamic_cast<const slg::DotsTexture*>(pTex);
            updateTextureInfo(pRealTex->GetInsideTex(),tex2name);
            updateTextureInfo(pRealTex->GetOutsideTex(),tex2name);
        }
        break;
    case slg::BRICK:
        {
            const slg::BrickTexture*   pRealTex = dynamic_cast<const slg::BrickTexture*>(pTex);
            updateTextureInfo(pRealTex->GetTexture1(),tex2name);
            updateTextureInfo(pRealTex->GetTexture2(),tex2name);
            updateTextureInfo(pRealTex->GetTexture3(),tex2name);
        }
        break;
    case slg::WINDY:
    case slg::WRINKLED:
    case slg::UV_TEX:
        //has no child
        break;
    case slg::BAND_TEX:
        {
            const slg::BandTexture*   pRealTex = dynamic_cast<const slg::BandTexture*>(pTex);
            updateTextureInfo(pRealTex->GetAmountTexture(),tex2name);
        }
        break;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code");
    }
}


void
ExtraTextureManager::onTextureRemoved(const slg::Texture *pTex)
{
    switch(pTex->GetType())
    {
    case slg::CONST_FLOAT3:
        break;
    case slg::CONST_FLOAT:
        break;
    case slg::IMAGEMAP:
///@fixme: we can not erase slgname2filepath,because the texture-refresh will exec to here.in this situation slgname2filepath
///is valid,only pTex changed.but,how to distinguish it from id destroy? the best solution may be callback from slg::texDefs.
//        {
//            const std::string&  id = m_tex2id[pTex];
//            if(id.length())
//            {
//                m_slgname2filepath_map.erase(id);
//            }
//        }
        break;
    case slg::SCALE_TEX:
        {
            const slg::ScaleTexture*   pRealTex = dynamic_cast<const slg::ScaleTexture*>(pTex);
            onTextureRemoved(pRealTex->GetTexture1());
            onTextureRemoved(pRealTex->GetTexture2());
        }
        break;
    case slg::FRESNEL_APPROX_N:
        {
            const slg::FresnelApproxNTexture*   pRealTex = dynamic_cast<const slg::FresnelApproxNTexture*>(pTex);
            onTextureRemoved(pRealTex->GetTexture());
        }
        break;
    case slg::FRESNEL_APPROX_K:
        {
            const slg::FresnelApproxKTexture*   pRealTex = dynamic_cast<const slg::FresnelApproxKTexture*>(pTex);
            onTextureRemoved(pRealTex->GetTexture());
        }
        break;
    case slg::MIX_TEX:
        {
            const slg::MixTexture*   pRealTex = dynamic_cast<const slg::MixTexture*>(pTex);
            onTextureRemoved(pRealTex->GetTexture1());
            onTextureRemoved(pRealTex->GetTexture2());
            onTextureRemoved(pRealTex->GetAmountTexture());
        }
        break;
    case slg::ADD_TEX:
        {
            const slg::AddTexture*   pRealTex = dynamic_cast<const slg::AddTexture*>(pTex);
            onTextureRemoved(pRealTex->GetTexture1());
            onTextureRemoved(pRealTex->GetTexture2());
        }
        break;
    case slg::CHECKERBOARD2D:
        {
            const slg::CheckerBoard2DTexture*   pRealTex = dynamic_cast<const slg::CheckerBoard2DTexture*>(pTex);
            onTextureRemoved(pRealTex->GetTexture1());
            onTextureRemoved(pRealTex->GetTexture2());
        }
        break;
    case slg::CHECKERBOARD3D:
        {
            const slg::CheckerBoard3DTexture*   pRealTex = dynamic_cast<const slg::CheckerBoard3DTexture*>(pTex);
            onTextureRemoved(pRealTex->GetTexture1());
            onTextureRemoved(pRealTex->GetTexture2());
        }
        break;
    case slg::FBM_TEX:
    case slg::MARBLE:
        //has no child.
        break;
    case slg::DOTS:
        {
            const slg::DotsTexture*   pRealTex = dynamic_cast<const slg::DotsTexture*>(pTex);
            onTextureRemoved(pRealTex->GetInsideTex());
            onTextureRemoved(pRealTex->GetOutsideTex());
        }
        break;
    case slg::BRICK:
        {
            const slg::BrickTexture*   pRealTex = dynamic_cast<const slg::BrickTexture*>(pTex);
            onTextureRemoved(pRealTex->GetTexture1());
            onTextureRemoved(pRealTex->GetTexture2());
            onTextureRemoved(pRealTex->GetTexture3());
        }
        break;
    case slg::WINDY:
    case slg::WRINKLED:
    case slg::UV_TEX:
        //no child
        break;
    case slg::BAND_TEX:
        {
            const slg::BandTexture*   pRealTex = dynamic_cast<const slg::BandTexture*>(pTex);
            onTextureRemoved(pRealTex->GetAmountTexture());
        }
        break;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code");
    }
    m_tex2id.erase(pTex);
}

void
ExtraTextureManager::appendTex2IdFromSlg()
{
    slg::Scene  *scene = Doc::instance().pDocData->getSession()->renderConfig->scene;
    std::vector<std::string> textureNameArray = scene->texDefs.GetTextureNames ();

    u_int   nameIdx = 0;
    for(std::vector< std::string >::const_iterator it = textureNameArray.begin(); it < textureNameArray.end(); ++it,++nameIdx)
    {
        m_tex2id[scene->texDefs.GetTexture(scene->texDefs.GetTextureIndex(*it))] = textureNameArray[nameIdx];
    }
}

void
ExtraTextureManager::loadExtraFromProps(luxrays::Properties &props)
{
    slg::Scene  *scene = Doc::instance().pDocData->getSession()->renderConfig->scene;
    for(u_int idx = 0 ; idx < scene->texDefs.GetSize(); idx++)
    {
        slg::Texture* pTex = scene->texDefs.GetTexture(idx);
        if(pTex->GetType() == slg::IMAGEMAP)
        {
            const std::string &slgname = Doc::instance().pDocData->texManager.getTextureId(pTex);
            std::string path = props.GetString("scene.textures." + slgname + ".file","");
            if(path.length())
            {
                boost::filesystem::path   filepath = boost::filesystem::canonical(path);
                this->addPath(slgname,filepath.string());
            }
        }
    }
}

type_xml_node*
ExtraTextureManager::dumpTextureMapping3D(type_xml_node &parent,const slg::TextureMapping3D *ptm3d,dumpContext &ctx)
{
    type_xml_doc *pDoc = parent.document();
    BOOST_ASSERT_MSG(pDoc != NULL,"invalid node,must from doc");

    type_xml_node *pSelf = pDoc->allocate_node(NS_RAPIDXML::node_element,constDef::mapping);
    parent.append_node(pSelf);

    conditional_md5 md5(ctx);

    int type = ptm3d->GetType ();
    md5.update((const unsigned char*)(void*)&type,sizeof(type));

    switch( type )
    {
    case slg::UVMAPPING3D:
        {
            const slg::UVMapping3D    *pRealMapper = dynamic_cast<const slg::UVMapping3D*>(ptm3d);

			std::string uvmapping3d("uvmapping3d");
            pSelf->append_attribute(allocate_attribute(pDoc,constDef::type,uvmapping3d));

            std::stringstream   ss;
            SlgUtil::OutputSlgmat(ss,pRealMapper->worldToLocal.m);
			std::string		trans = ss.str();

            pSelf->append_attribute(allocate_attribute(pDoc,constDef::transformation,trans));

            md5.update(uvmapping3d.c_str(),(MD5::size_type)uvmapping3d.length());
            md5.update(trans.c_str(),(MD5::size_type)trans.length());
        }
        break;
    case slg::GLOBALMAPPING3D:
        {
            const slg::GlobalMapping3D    *pRealMapper = dynamic_cast<const slg::GlobalMapping3D*>(ptm3d);

			std::string globalmapping3d("globalmapping3d");
			pSelf->append_attribute(allocate_attribute(pDoc,constDef::type,globalmapping3d));

            std::stringstream   ss;
            SlgUtil::OutputSlgmat(ss,pRealMapper->worldToLocal.m);
			std::string		trans = ss.str();

            pSelf->append_attribute(allocate_attribute(pDoc,constDef::transformation,trans));

            md5.update(globalmapping3d.c_str(),(MD5::size_type)globalmapping3d.length());
            md5.update(trans.c_str(),(MD5::size_type)trans.length());
        }
        break;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code");
    }

    if(md5.isGenerateMD5())
    {
        std::string md5Value =md5.hexdigest();
        pSelf->append_attribute(allocate_attribute(pDoc,constDef::ctxmd5,md5Value));
    }

	return pSelf;
}

void
ExtraTextureManager::importTextureMapping3D(std::ostream &o,type_xml_node &self,const std::string &id)
{
    type_xml_attr *pTypeAttr = self.first_attribute(constDef::type);
    if(pTypeAttr)
    {
        if(boost::iequals("uvmapping3d",pTypeAttr->value()))
        {
            o << "scene.textures." << id << ".mapping.type = uvmapping3d" << std::endl;

            type_xml_attr *pAttr = self.first_attribute(constDef::transformation);
            if(pAttr)
                o << "scene.textures." << id << ".mapping.transformation = " << pAttr->value() << std::endl;
        }else if(boost::iequals("globalmapping3d",pTypeAttr->value()))
        {
            o << "scene.textures." << id << ".mapping.type = globalmapping3d" << std::endl;

            type_xml_attr *pAttr = self.first_attribute(constDef::transformation);
            if(pAttr)
                o << "scene.textures." << id << ".mapping.transformation = " << pAttr->value() << std::endl;
        }
    }
}

void
ExtraTextureManager::importTextureMapping2D(std::ostream &o,type_xml_node &self,const std::string &id)
{
    type_xml_attr *pTypeAttr = self.first_attribute(constDef::type);
    if(pTypeAttr)
    {
        if(boost::iequals("uvmapping2d",pTypeAttr->value()))
        {
            o << "scene.textures." << id << ".mapping.type = uvmapping2d" << std::endl;

            type_xml_attr *pAttr = self.first_attribute("uvscale");
            if(pAttr)
                o << "scene.textures." << id << ".mapping.uvscale = " << pAttr->value() << std::endl;

            pAttr = self.first_attribute("uvdelta");
            if(pAttr)
                o << "scene.textures." << id << ".mapping.uvdelta = " << pAttr->value() << std::endl;
        }
    }
}



type_xml_node*
ExtraTextureManager::dumpTextureMapping2D(type_xml_node &parent,const slg::TextureMapping2D *ptm2d,dumpContext &ctx)
{
    type_xml_doc *pDoc = parent.document();
    BOOST_ASSERT_MSG(pDoc != NULL,"invalid node,must from doc");

    type_xml_node *pSelf = pDoc->allocate_node(NS_RAPIDXML::node_element,constDef::mapping);
    parent.append_node(pSelf);


    conditional_md5 md5(ctx);

    int type = ptm2d->GetType ();
    md5.update((const unsigned char*)(void*)&type,sizeof(type));

    switch( type )
    {
    case slg::UVMAPPING2D:
        {
            const slg::UVMapping2D    *pRealMapper = dynamic_cast<const slg::UVMapping2D*>(ptm2d);

            pSelf->append_attribute(allocate_attribute(pDoc,constDef::type,"uvmapping2d"));

            std::string value = boost::lexical_cast<std::string>(pRealMapper->uScale) + ' ' + boost::lexical_cast<std::string>(pRealMapper->vScale);
            pSelf->append_attribute(allocate_attribute_withkey(pDoc,"uvscale",value));

            value = boost::lexical_cast<std::string>(pRealMapper->uDelta) + ' ' + boost::lexical_cast<std::string>(pRealMapper->vDelta);
            pSelf->append_attribute(allocate_attribute_withkey(pDoc,"uvdelta",value));

            md5.update((const unsigned char*)(void*)&pRealMapper->uScale,sizeof(pRealMapper->uScale));
            md5.update((const unsigned char*)(void*)&pRealMapper->vScale,sizeof(pRealMapper->vScale));
            md5.update((const unsigned char*)(void*)&pRealMapper->uDelta,sizeof(pRealMapper->uDelta));
            md5.update((const unsigned char*)(void*)&pRealMapper->vDelta,sizeof(pRealMapper->vDelta));
        }
        break;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code");
    }

    if(md5.isGenerateMD5())
    {
        std::string md5Value =md5.hexdigest();
        pSelf->append_attribute(allocate_attribute(pDoc,constDef::ctxmd5,md5Value));
    }

    return pSelf;
}

std::string
ExtraTextureManager::getBondnameFromType(slg::MasonryBond type)
{
    switch(type)
    {
    case slg::FLEMISH:
        return "flemish";
    case slg::RUNNING:
        return "running";
    case slg::ENGLISH:
        return "english";
    case slg::HERRINGBONE:
        return "herringbone";
    case slg::BASKET:
        return "basket";
    case slg::KETTING:
        return "chain link";
    }
    BOOST_ASSERT_MSG(false,"unreachable code");
    return "";
}

void
ExtraTextureManager::dumpTextureMapping3D(const slg::TextureMapping3D *pMapping,luxrays::Properties &props,const std::string &prefix)
{
    props.Load(pMapping->ToProperties(prefix));
}

void
ExtraTextureManager::dumpTextureMapping2D(const slg::TextureMapping2D *pMapping,luxrays::Properties &props,const std::string &prefix)
{
    if(!pMapping)
        return;
    switch(pMapping->GetType())
    {
    case slg::UVMAPPING2D:
        {
            const slg::UVMapping2D *pMapping2D = dynamic_cast<const slg::UVMapping2D*>(pMapping);
            if(pMapping2D && !ExtraTextureManager::UVMapping2D_isDefault(pMapping2D))
            {
                props.SetString(prefix + constDef::type,"uvmapping2d");
                if(!UVMapping2D_isScaleDefault(pMapping2D))
                {
                    std::string value = boost::str(boost::format("%.4f %.4f") %pMapping2D->uScale % pMapping2D->vScale);
                    props.SetString(prefix + "uvscale",value);
                }
                if(!UVMapping2D_isDeltaDefault(pMapping2D))
                {
                    std::string value = boost::str(boost::format("%.4f %.4f") %pMapping2D->uDelta % pMapping2D->vDelta);
                    props.SetString(prefix + "uvdelta",value);
                }
            }
        }
        break;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code");
    }
}

std::string
ExtraTextureManager::dumpImagemap(luxrays::Properties &prop,const slg::ImageMapTexture* pImageTex)
{
    const std::string &id = this->getTextureId(pImageTex);
    const std::string *pfullpath = this->queryPath(id);
    BOOST_ASSERT_MSG(!id.empty() && pfullpath,"image texture without id!");

    std::string     prefix = "scene.textures.";
    prefix = prefix + id + '.';
    prop.SetString(prefix + constDef::type,"imagemap");
    prop.SetString(prefix + constDef::file,*pfullpath);

    if(!ImageMapTexture_isGammaDefault(pImageTex->GetImageMap()->GetGamma()))
        prop.SetString(prefix + "gamma",boost::lexical_cast<std::string>(pImageTex->GetImageMap()->GetGamma()));

    if(!ImageMapTexture_isGainDefault(pImageTex->GetGain()))
        prop.SetString(prefix + "gain",boost::lexical_cast<std::string>(pImageTex->GetGain()));

    //update mapping2d.
    dumpTextureMapping2D(pImageTex->GetTextureMapping(),prop,prefix + ".mapping.");
    return id;
}

std::string
ExtraTextureManager::dump(luxrays::Properties &prop,const slg::Texture* pTex)
{
    switch(pTex->GetType())
    {
    case slg::CONST_FLOAT3:
        {
            luxrays::Spectrum color = dynamic_cast<const slg::ConstFloat3Texture*>(pTex)->GetColor();
            return boost::str(boost::format("%f %f %f") % color.r % color.g % color.b );
        }
    case slg::CONST_FLOAT:
        {
            float value = dynamic_cast<const slg::ConstFloatTexture*>(pTex)->GetValue();
            return boost::lexical_cast<std::string>( value );
        }
    case slg::IMAGEMAP:
        {
            ExtraTextureManager &texManager = Doc::instance().pDocData->texManager;
            return texManager.dumpImagemap(prop,dynamic_cast<const slg::ImageMapTexture*>(pTex));
        }
    case slg::SCALE_TEX:
        {
            std::string id = Doc::instance().pDocData->texManager.dumpCompTex(prop,pTex);
            const slg::ScaleTexture *pRealTex = dynamic_cast<const slg::ScaleTexture*>(pTex);

            prop.SetString("scene.textures." + id + ".texture1",ExtraTextureManager::dump(prop,pRealTex->GetTexture1()));
            prop.SetString("scene.textures." + id + ".texture2",ExtraTextureManager::dump(prop,pRealTex->GetTexture2()));
            return id;
        }
    case slg::FRESNEL_APPROX_N:
        {
            std::string id = Doc::instance().pDocData->texManager.dumpCompTex(prop,pTex);
            const slg::FresnelApproxNTexture *pRealTex = dynamic_cast<const slg::FresnelApproxNTexture*>(pTex);

            prop.SetString("scene.textures." + id + ".texture",ExtraTextureManager::dump(prop,pRealTex->GetTexture()));
            return id;
        }
    case slg::FRESNEL_APPROX_K:
        {
            std::string id = Doc::instance().pDocData->texManager.dumpCompTex(prop,pTex);
            const slg::FresnelApproxKTexture *pRealTex = dynamic_cast<const slg::FresnelApproxKTexture*>(pTex);

            prop.SetString("scene.textures." + id + ".texture",ExtraTextureManager::dump(prop,pRealTex->GetTexture()));
            return id;
        }
    case slg::MIX_TEX:
        {
            std::string id = Doc::instance().pDocData->texManager.dumpCompTex(prop,pTex);
            const slg::MixTexture *pRealTex = dynamic_cast<const slg::MixTexture*>(pTex);

            prop.SetString("scene.textures." + id + ".amount",ExtraTextureManager::dump(prop,pRealTex->GetAmountTexture()));
            prop.SetString("scene.textures." + id + ".texture1",ExtraTextureManager::dump(prop,pRealTex->GetTexture1()));
            prop.SetString("scene.textures." + id + ".texture2",ExtraTextureManager::dump(prop,pRealTex->GetTexture2()));
            return id;
        }
    case slg::ADD_TEX:
        {
            std::string id = Doc::instance().pDocData->texManager.dumpCompTex(prop,pTex);
            const slg::AddTexture *pRealTex = dynamic_cast<const slg::AddTexture*>(pTex);

            prop.SetString("scene.textures." + id + ".texture1",ExtraTextureManager::dump(prop,pRealTex->GetTexture1()));
            prop.SetString("scene.textures." + id + ".texture2",ExtraTextureManager::dump(prop,pRealTex->GetTexture2()));
            return id;
        }
    case slg::CHECKERBOARD2D:
        {
            std::string id = Doc::instance().pDocData->texManager.dumpCompTex(prop,pTex);
            const slg::CheckerBoard2DTexture *pRealTex = dynamic_cast<const slg::CheckerBoard2DTexture*>(pTex);

            prop.SetString("scene.textures." + id + ".texture1",ExtraTextureManager::dump(prop,pRealTex->GetTexture1()));
            prop.SetString("scene.textures." + id + ".texture2",ExtraTextureManager::dump(prop,pRealTex->GetTexture2()));

            //update mapping2d.
            dumpTextureMapping2D(pRealTex->GetTextureMapping(),prop,"scene.textures." + id + ".mapping.");
            return id;
        }
    case slg::CHECKERBOARD3D:
        {
            std::string id = Doc::instance().pDocData->texManager.dumpCompTex(prop,pTex);
            const slg::CheckerBoard3DTexture *pRealTex = dynamic_cast<const slg::CheckerBoard3DTexture*>(pTex);

            prop.SetString("scene.textures." + id + ".texture1",ExtraTextureManager::dump(prop,pRealTex->GetTexture1()));
            prop.SetString("scene.textures." + id + ".texture2",ExtraTextureManager::dump(prop,pRealTex->GetTexture2()));

            //update mapping3d.
            dumpTextureMapping3D(pRealTex->GetTextureMapping(),prop,"scene.textures." + id + ".mapping");
            return id;
        }
    case slg::FBM_TEX:
        {
            std::string id = Doc::instance().pDocData->texManager.dumpCompTex(prop,pTex);
            const slg::FBMTexture *pRealTex = dynamic_cast<const slg::FBMTexture*>(pTex);

            prop.SetString("scene.textures." + id + ".octaves",boost::lexical_cast<std::string>( pRealTex->GetOctaves() ));
            prop.SetString("scene.textures." + id + ".roughness",boost::lexical_cast<std::string>( pRealTex->GetOmega() ));

            //update mapping3d.
            dumpTextureMapping3D(pRealTex->GetTextureMapping(),prop,"scene.textures." + id + ".mapping");
            return id;
        }
    case slg::MARBLE:
        {
            std::string id = Doc::instance().pDocData->texManager.dumpCompTex(prop,pTex);
            const slg::MarbleTexture *pRealTex = dynamic_cast<const slg::MarbleTexture*>(pTex);

            prop.SetString("scene.textures." + id + ".octaves",boost::lexical_cast<std::string>( pRealTex->GetOctaves() ));
            prop.SetString("scene.textures." + id + ".roughness",boost::lexical_cast<std::string>( pRealTex->GetOmega() ));
            prop.SetString("scene.textures." + id + ".scale",boost::lexical_cast<std::string>( pRealTex->GetScale() ));
            prop.SetString("scene.textures." + id + ".variation",boost::lexical_cast<std::string>( pRealTex->GetVariation() ));

            //update mapping3d.
            dumpTextureMapping3D(pRealTex->GetTextureMapping(),prop,"scene.textures." + id + ".mapping");
            return id;
        }
    case slg::DOTS:
        {
            std::string id = Doc::instance().pDocData->texManager.dumpCompTex(prop,pTex);
            const slg::DotsTexture *pRealTex = dynamic_cast<const slg::DotsTexture*>(pTex);

            prop.SetString("scene.textures." + id + ".inside",ExtraTextureManager::dump(prop,pRealTex->GetInsideTex()));
            prop.SetString("scene.textures." + id + ".outside",ExtraTextureManager::dump(prop,pRealTex->GetOutsideTex()));

            //update mapping2d.
            dumpTextureMapping2D(pRealTex->GetTextureMapping(),prop,"scene.textures." + id + ".mapping.");
            return id;
        }
    case slg::BRICK:
        {
            std::string id = Doc::instance().pDocData->texManager.dumpCompTex(prop,pTex);
            const slg::BrickTexture *pRealTex = dynamic_cast<const slg::BrickTexture*>(pTex);

            prop.SetString("scene.textures." + id + ".bricktex",ExtraTextureManager::dump(prop,pRealTex->GetTexture1()));
            prop.SetString("scene.textures." + id + ".mortartex",ExtraTextureManager::dump(prop,pRealTex->GetTexture2()));
            prop.SetString("scene.textures." + id + ".brickmodtex",ExtraTextureManager::dump(prop,pRealTex->GetTexture3()));

            prop.SetString("scene.textures." + id + ".brickbond",getBondnameFromType(pRealTex->GetBond()));
            prop.SetString("scene.textures." + id + ".brickwidth",boost::lexical_cast<std::string>(pRealTex->GetBrickWidth()));
            prop.SetString("scene.textures." + id + ".brickheight",boost::lexical_cast<std::string>(pRealTex->GetBrickHeight()));
            prop.SetString("scene.textures." + id + ".brickdepth",boost::lexical_cast<std::string>(pRealTex->GetBrickDepth()));
            prop.SetString("scene.textures." + id + ".mortarsize",boost::lexical_cast<std::string>(pRealTex->GetMortarSize()));
            prop.SetString("scene.textures." + id + ".brickrun",boost::lexical_cast<std::string>(pRealTex->GetRun()));
            prop.SetString("scene.textures." + id + ".brickbevel",boost::lexical_cast<std::string>(pRealTex->GetBevelWidth() / pRealTex->GetBrickWidth()));

            //update mapping3d.
            dumpTextureMapping3D(pRealTex->GetTextureMapping(),prop,"scene.textures." + id + ".mapping");
            return id;
        }
        break;
    case slg::WINDY:
        {
            std::string id = Doc::instance().pDocData->texManager.dumpCompTex(prop,pTex);
            const slg::WindyTexture *pRealTex = dynamic_cast<const slg::WindyTexture*>(pTex);
            //update mapping3d.
            dumpTextureMapping3D(pRealTex->GetTextureMapping(),prop,"scene.textures." + id + ".mapping");
            return id;
        }
    case slg::WRINKLED:
    {
        std::string id = Doc::instance().pDocData->texManager.dumpCompTex(prop,pTex);
        const slg::WrinkledTexture *pRealTex = dynamic_cast<const slg::WrinkledTexture*>(pTex);

        prop.SetString("scene.textures." + id + ".octaves",boost::lexical_cast<std::string>( pRealTex->GetOctaves() ));
        prop.SetString("scene.textures." + id + ".roughness",boost::lexical_cast<std::string>( pRealTex->GetOmega() ));

        //update mapping3d.
        dumpTextureMapping3D(pRealTex->GetTextureMapping(),prop,"scene.textures." + id + ".mapping");
        return id;
    }
    case slg::UV_TEX:
    {
        std::string id = Doc::instance().pDocData->texManager.dumpCompTex(prop,pTex);
        const slg::UVTexture *pRealTex = dynamic_cast<const slg::UVTexture*>(pTex);
        //update mapping2d.
        dumpTextureMapping2D(pRealTex->GetTextureMapping(),prop,"scene.textures." + id + ".mapping.");
        return id;
    }
    case slg::BAND_TEX:
        {
            BOOST_ASSERT_MSG(false,"not implement");
        }
        break;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code");
    }
    return "";
}


type_xml_node*
ExtraTextureManager::dump(type_xml_node &parent,const std::string &tag,const slg::Texture *pTex,dumpContext &ctx)
{
    type_xml_doc *pDoc = parent.document();
    BOOST_ASSERT_MSG(pDoc != NULL,"invalid node,must from doc");

    type_xml_node *pSelf = pDoc->allocate_node(NS_RAPIDXML::node_element,allocate_string(pDoc,tag));
    parent.append_node(pSelf);

    conditional_md5 md5(ctx);

    std::string texTypeName = DocMat::texGetTypeNameFromType(pTex->GetType());
    pSelf->append_attribute(allocate_attribute(pDoc,constDef::type,texTypeName));
    md5.update(texTypeName.c_str(),(MD5::size_type)texTypeName.length());

    switch(pTex->GetType())
    {
    case slg::CONST_FLOAT3:
        {
            luxrays::Spectrum color = dynamic_cast<const slg::ConstFloat3Texture*>(pTex)->GetColor();

            pSelf->append_attribute(allocate_attribute_withkey(pDoc,"r",boost::lexical_cast<std::string>( color.r )));
            pSelf->append_attribute(allocate_attribute_withkey(pDoc,"g",boost::lexical_cast<std::string>( color.g )));
            pSelf->append_attribute(allocate_attribute_withkey(pDoc,"b",boost::lexical_cast<std::string>( color.b )));

            md5.update((unsigned char*)(void*)&color,sizeof(color));
        }
        break;
    case slg::CONST_FLOAT:
        {
            float value = dynamic_cast<const slg::ConstFloatTexture*>(pTex)->GetValue();

            pSelf->append_attribute(allocate_attribute_withkey(pDoc,"value",boost::lexical_cast<std::string>( value )));
            md5.update((unsigned char*)(void*)&value,sizeof(value));
        }
        break;
    case slg::IMAGEMAP:
        {
            // 只有imageTexture才需要name.
            const std::string &TextureId = m_tex2id[pTex];
            BOOST_ASSERT_MSG(TextureId.length() > 0, "invalid texture id!!!");
            pSelf->append_attribute(allocate_attribute(pDoc,constDef::id,boost::lexical_cast<std::string>( TextureId )));

            const slg::ImageMapTexture*   pImageMap = dynamic_cast<const slg::ImageMapTexture*>(pTex);
            BOOST_ASSERT_MSG(pImageMap,"Texture Type Panic!");
            float gain = pImageMap->GetGain();
            pSelf->append_attribute(allocate_attribute_withkey(pDoc,"gain",boost::lexical_cast<std::string>( gain )));

            md5.update((unsigned char*)(void*)&gain,sizeof(gain));

            const slg::ImageMap   *pMap = pImageMap->GetImageMap();
            float gamma = pMap->GetGamma();
            pSelf->append_attribute(allocate_attribute_withkey(pDoc,"gamma",boost::lexical_cast<std::string>( gamma )));
            md5.update((unsigned char*)(void*)&gamma,sizeof(gamma));

            //开始保存图像。
            //保存了图像文件的路径。
            std::string     write_file;
            const slg::ImageMap   *pImage = pImageMap->GetImageMap();
            std::string imageCtxMd5_value;

            if(md5.isGenerateMD5())
            {
                MD5             imageCtxMd5;

                imageCtxMd5.update((unsigned char*)(void*)pImage->GetPixels(),pImage->GetWidth() * pImage->GetHeight() * pImage->GetChannelCount() * sizeof(float));
                imageCtxMd5_value =imageCtxMd5.finalize().hexdigest();
                md5.update(imageCtxMd5_value.c_str(),(MD5::size_type)imageCtxMd5_value.length());

                pSelf->append_attribute(allocate_attribute_withkey(pDoc,"imagemd5",imageCtxMd5_value));

                if(ctx.isOptmizeTex())
                {
                    const std::string *pSameCtxPath = ctx.queryTexFilepath(imageCtxMd5_value);
                    if(pSameCtxPath)
                    {//文件内容可以被复用。
                        write_file = *pSameCtxPath;
                    }
                }
            }


            if(write_file.length() == 0)
            {//文件尚未被获取。
                //获取原始文件地址。
                const std::string *porigPath = Doc::instance().pDocData->texManager.queryPath(getTextureId(pTex));
                if(porigPath && porigPath->length())
                {//原始文件被记录。
                    if(ctx.isCopyResource())
                    {//保存资源。
                        boost::filesystem::path     origpath(*porigPath);
                        boost::filesystem::path   target = ctx.target;
                        target /= origpath.filename();

                        target = boost::filesystem::gme_ext::ensureNonExistFile(target);
                        //pImage->WriteImage(target.string());
                        //直接拷贝原始文件。
                        boost::filesystem::copy(*porigPath,target);
                        write_file = target.filename().string();
                    }else{//不保存资源，直接保存m_filepath.
                        write_file = *porigPath;
                    }
                }else{//没有定义文件名。此时直接保存资源。为了防止丢失信息，保存为hdr格式。
                    boost::filesystem::path target_model = ctx.target / "image%%%%%%.exr";
                    boost::filesystem::path target = boost::filesystem::unique_path(target_model);
                    pImage->WriteImage(target.string());
                    write_file = target.filename().string();
                }
                if(imageCtxMd5_value.length())
                    ctx.addTexMapper(imageCtxMd5_value,write_file);
            }

            pSelf->append_attribute(allocate_attribute_withkey(pDoc,"file",write_file));

            //输出TextureMapping2D.
            type_xml_node *pChild = dumpTextureMapping2D(*pSelf,pImageMap->GetTextureMapping(),ctx);
            md5.updateChild(pChild);
        }
        break;
    case slg::SCALE_TEX:
        {
            const slg::ScaleTexture*   pRealTex = dynamic_cast<const slg::ScaleTexture*>(pTex);

            type_xml_node *pChild1 = dump(*pSelf,"texture1",pRealTex->GetTexture1(),ctx);
            type_xml_node *pChild2 = dump(*pSelf,"texture2",pRealTex->GetTexture2(),ctx);
            md5.updateChild(pChild1);
            md5.updateChild(pChild2);
        }
        break;
        case slg::FRESNEL_APPROX_N:
        {
            const slg::FresnelApproxNTexture*   pRealTex = dynamic_cast<const slg::FresnelApproxNTexture*>(pTex);
            type_xml_node *pChild = dump(*pSelf,"texture",pRealTex->GetTexture(),ctx);
            md5.updateChild(pChild);
        }
        break;
        case slg::FRESNEL_APPROX_K:
        {
            const slg::FresnelApproxKTexture*   pRealTex = dynamic_cast<const slg::FresnelApproxKTexture*>(pTex);
            type_xml_node *pChild = dump(*pSelf,"texture",pRealTex->GetTexture(),ctx);
            md5.updateChild(pChild);
        }
        break;
    case slg::MIX_TEX:
        {
            const slg::MixTexture*   pRealTex = dynamic_cast<const slg::MixTexture*>(pTex);
            type_xml_node *pChild = dump(*pSelf,"texture1",pRealTex->GetTexture1(),ctx);
            md5.updateChild(pChild);

            pChild = dump(*pSelf,"texture2",pRealTex->GetTexture2(),ctx);
            md5.updateChild(pChild);

            pChild = dump(*pSelf,constDef::amount,pRealTex->GetAmountTexture(),ctx);
            md5.updateChild(pChild);
        }
        break;
    case slg::ADD_TEX:
        {
            const slg::AddTexture*   pRealTex = dynamic_cast<const slg::AddTexture*>(pTex);
            type_xml_node *pChild1 = dump(*pSelf,"texture1",pRealTex->GetTexture1(),ctx);
            type_xml_node *pChild2 = dump(*pSelf,"texture2",pRealTex->GetTexture2(),ctx);

            md5.updateChild(pChild1);
            md5.updateChild(pChild2);
        }
        break;
    case slg::CHECKERBOARD2D:
        {
            const slg::CheckerBoard2DTexture*   pRealTex = dynamic_cast<const slg::CheckerBoard2DTexture*>(pTex);
            type_xml_node *pChild1 = dump(*pSelf,"texture1",pRealTex->GetTexture1(),ctx);
            type_xml_node *pChild2 = dump(*pSelf,"texture2",pRealTex->GetTexture2(),ctx);
            type_xml_node *pMapping = dumpTextureMapping2D(*pSelf,pRealTex->GetTextureMapping(),ctx);
            md5.updateChild(pChild1);
            md5.updateChild(pChild2);
            md5.updateChild(pMapping);
        }
        break;
    case slg::CHECKERBOARD3D:
        {
            const slg::CheckerBoard3DTexture*   pRealTex = dynamic_cast<const slg::CheckerBoard3DTexture*>(pTex);

            type_xml_node *child = dump(*pSelf,"texture1",pRealTex->GetTexture1(),ctx);
            md5.updateChild(child);

            child = dump(*pSelf,"texture2",pRealTex->GetTexture2(),ctx);
            md5.updateChild(child);

            child = dumpTextureMapping3D(*pSelf,pRealTex->GetTextureMapping(),ctx);
            md5.updateChild(child);
        }
        break;
    case slg::FBM_TEX:
        {
            const slg::FBMTexture*   pRealTex = dynamic_cast<const slg::FBMTexture*>(pTex);

			int   octaves = pRealTex->GetOctaves();
			float roughness = pRealTex->GetOmega();
			pSelf->append_attribute(allocate_attribute_withkey(pDoc,"octaves",boost::lexical_cast<std::string>( octaves )));
			pSelf->append_attribute(allocate_attribute_withkey(pDoc,"roughness",boost::lexical_cast<std::string>( roughness )));
            type_xml_node *pMapping = dumpTextureMapping3D(*pSelf,pRealTex->GetTextureMapping(),ctx);

            md5.update((unsigned char*)(void*)&octaves,sizeof(octaves));
            md5.update((unsigned char*)(void*)&roughness,sizeof(roughness));
            md5.updateChild(pMapping);
        }
        break;
    case slg::MARBLE:
        {
            const slg::MarbleTexture*   pRealTex = dynamic_cast<const slg::MarbleTexture*>(pTex);
			int   octaves = pRealTex->GetOctaves();
			float roughness = pRealTex->GetOmega();
			float scale = pRealTex->GetScale();
			float variation = pRealTex->GetVariation();

			pSelf->append_attribute(allocate_attribute_withkey(pDoc,"octaves",boost::lexical_cast<std::string>( octaves )));
			pSelf->append_attribute(allocate_attribute_withkey(pDoc,"roughness",boost::lexical_cast<std::string>( roughness )));
			pSelf->append_attribute(allocate_attribute_withkey(pDoc,"scale",boost::lexical_cast<std::string>( scale )));
			pSelf->append_attribute(allocate_attribute_withkey(pDoc,"variation",boost::lexical_cast<std::string>( variation )));
            md5.update((unsigned char*)(void*)&octaves,sizeof(octaves));
            md5.update((unsigned char*)(void*)&roughness,sizeof(roughness));
            md5.update((unsigned char*)(void*)&scale,sizeof(scale));
            md5.update((unsigned char*)(void*)&variation,sizeof(variation));

            type_xml_node *pMapping = dumpTextureMapping3D(*pSelf,pRealTex->GetTextureMapping(),ctx);
            md5.updateChild(pMapping);
        }
        break;
    case slg::DOTS:
        {
            const slg::DotsTexture*   pRealTex = dynamic_cast<const slg::DotsTexture*>(pTex);

            type_xml_node *pChild1 = dump(*pSelf,"inside",pRealTex->GetInsideTex(),ctx);
            type_xml_node *pChild2 = dump(*pSelf,"outside",pRealTex->GetOutsideTex(),ctx);
            type_xml_node *pMapping = dumpTextureMapping2D(*pSelf,pRealTex->GetTextureMapping(),ctx);

            md5.updateChild(pChild1);
            md5.updateChild(pChild2);
            md5.updateChild(pMapping);
        }
        break;
    case slg::BRICK:
        {
            const slg::BrickTexture*   pRealTex = dynamic_cast<const slg::BrickTexture*>(pTex);

			std::string brickbond = getBondnameFromType(pRealTex->GetBond());
			float brickwidth = pRealTex->GetBrickWidth();
			float brickheight = pRealTex->GetBrickHeight();
			float brickdepth = pRealTex->GetBrickDepth();
			float mortarsize = pRealTex->GetMortarSize();
			float brickrun = pRealTex->GetRun();
			float brickbevel = pRealTex->GetBevelWidth() / pRealTex->GetBrickWidth();

			pSelf->append_attribute(allocate_attribute_withkey(pDoc,"brickbond",brickbond));
			pSelf->append_attribute(allocate_attribute_withkey(pDoc,"brickwidth",boost::lexical_cast<std::string>( brickwidth )));
			pSelf->append_attribute(allocate_attribute_withkey(pDoc,"brickheight",boost::lexical_cast<std::string>( brickheight )));
			pSelf->append_attribute(allocate_attribute_withkey(pDoc,"brickdepth",boost::lexical_cast<std::string>( brickdepth )));
			pSelf->append_attribute(allocate_attribute_withkey(pDoc,"mortarsize",boost::lexical_cast<std::string>( mortarsize )));
			pSelf->append_attribute(allocate_attribute_withkey(pDoc,"brickrun",boost::lexical_cast<std::string>( brickrun )));
			pSelf->append_attribute(allocate_attribute_withkey(pDoc,"brickbevel",boost::lexical_cast<std::string>( brickbevel )));
            md5.update(brickbond.c_str(),(MD5::size_type)brickbond.length());
            md5.update((unsigned char*)(void*)&brickwidth,sizeof(brickwidth));
            md5.update((unsigned char*)(void*)&brickheight,sizeof(brickheight));
            md5.update((unsigned char*)(void*)&brickdepth,sizeof(brickdepth));
            md5.update((unsigned char*)(void*)&mortarsize,sizeof(mortarsize));
            md5.update((unsigned char*)(void*)&brickrun,sizeof(brickrun));
            md5.update((unsigned char*)(void*)&brickbevel,sizeof(brickbevel));

            type_xml_node *pChild1 = dump(*pSelf,"bricktex",pRealTex->GetTexture1(),ctx);
            type_xml_node *pChild2 = dump(*pSelf,"mortartex",pRealTex->GetTexture2(),ctx);
            type_xml_node *pChild3 = dump(*pSelf,"brickmodtex",pRealTex->GetTexture3(),ctx);
            type_xml_node *pMapping = dumpTextureMapping3D(*pSelf,pRealTex->GetTextureMapping(),ctx);
            md5.updateChild(pChild1);
            md5.updateChild(pChild2);
            md5.updateChild(pChild3);
            md5.updateChild(pMapping);
        }
        break;
    case slg::WINDY:
        {
            const slg::WindyTexture*   pRealTex = dynamic_cast<const slg::WindyTexture*>(pTex);
            type_xml_node *pMapping = dumpTextureMapping3D(*pSelf,pRealTex->GetTextureMapping(),ctx);
            md5.updateChild(pMapping);
        }
        break;
    case slg::WRINKLED:
        {
            const slg::WrinkledTexture*   pRealTex = dynamic_cast<const slg::WrinkledTexture*>(pTex);
			int   octaves = pRealTex->GetOctaves();
			float roughness = pRealTex->GetOmega();
			pSelf->append_attribute(allocate_attribute_withkey(pDoc,"octaves",boost::lexical_cast<std::string>( octaves )));
			pSelf->append_attribute(allocate_attribute_withkey(pDoc,"roughness",boost::lexical_cast<std::string>( roughness )));
            md5.update((unsigned char*)(void*)&octaves,sizeof(octaves));
            md5.update((unsigned char*)(void*)&roughness,sizeof(roughness));

			type_xml_node *pMapping = dumpTextureMapping3D(*pSelf,pRealTex->GetTextureMapping(),ctx);
            md5.updateChild(pMapping);
        }
        break;
    case slg::UV_TEX:
        {
            const slg::UVTexture*   pRealTex = dynamic_cast<const slg::UVTexture*>(pTex);
			type_xml_node *pMapping = dumpTextureMapping2D(*pSelf,pRealTex->GetTextureMapping(),ctx);
            md5.updateChild(pMapping);
        }
        break;
    case slg::BAND_TEX:
        {
            const slg::BandTexture*   pRealTex = dynamic_cast<const slg::BandTexture*>(pTex);
            const std::vector< float > &offsets =pRealTex->GetOffsets();
            const std::vector< luxrays::Spectrum > &values = pRealTex->GetValues();
            for(size_t idx = 0; idx < offsets.size(); idx++)
            {
				float	offset_value = offsets[idx];
				std::stringstream ss;
				ss << values[idx].r << ' ' << values[idx].g << ' ' << values[idx].b;
				std::string  value_str = ss.str();

				pSelf->append_attribute(allocate_attribute_withkey(pDoc,"offset" + boost::lexical_cast<std::string>( idx ),
					boost::lexical_cast<std::string>( offset_value )));
				pSelf->append_attribute(allocate_attribute_withkey(pDoc,"value" + boost::lexical_cast<std::string>( idx ),
					value_str));

                md5.update((unsigned char*)(void*)&offset_value,sizeof(offset_value));
                md5.update(value_str.c_str(),(MD5::size_type)value_str.length());
            }

            type_xml_node *pChild1 = dump(*pSelf,"amount",pRealTex->GetAmountTexture(),ctx);
            md5.updateChild(pChild1);
        }
        break;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code");
    }
    if(md5.isGenerateMD5())
    {
        std::string     md5_value = md5.hexdigest();
        pSelf->append_attribute(allocate_attribute(pDoc,constDef::ctxmd5,md5_value));
    }
    return pSelf;
}

//static inline
//void extractNameAttr(type_xml_node &self,const std::string &id)
//{
//    type_xml_attr  *pNameAttr = self.first_attribute(constDef::name);
//    if(pNameAttr)
//}

static inline
void appendAttribute(type_xml_node &self,const std::string& id,std::ostream &o,const std::string& tag)
{
    type_xml_attr   *pAttr = self.first_attribute(tag.c_str());
    if(pAttr)
        o << "scene.textures." << id << '.' << tag << " = " << pAttr->value() << std::endl;
}


std::string
ExtraTextureManager::createTexture(ImportContext &ctx,type_xml_node &self)
{
    std::string     result;
    type_xml_attr  *pTypeAttr = self.first_attribute(constDef::type);
    if(pTypeAttr)
    {
        const char *typeStr = pTypeAttr->value();
        int type = DocMat::texGetTypeFromTypeName(typeStr);
        switch(type)
        {
        case slg::CONST_FLOAT3:
            {
                type_xml_attr *rAttr = self.first_attribute("r");
                type_xml_attr *gAttr = self.first_attribute("g");
                type_xml_attr *bAttr = self.first_attribute("b");
                if(rAttr && gAttr && bAttr)
                {
                    result = boost::str(boost::format("%s %s %s") % rAttr->value() % gAttr->value() % bAttr->value());
                }
            }
            break;
        case slg::CONST_FLOAT:
            {
                type_xml_attr  *valAttr = self.first_attribute("value");
                if(valAttr)
                {
                    result = valAttr->value();
                }
            }
            break;
        case slg::IMAGEMAP:
            {
                type_xml_attr  *fileAttr = self.first_attribute("file");
                if(fileAttr)
                {
                    std::string     id = getIdFromNode(self);

                    std::string fullpath;
                    const char* basepath = getFilepathFromDocument(self);
                    if(basepath)
                        fullpath = boost::filesystem::canonical(fileAttr->value(),basepath).string();
                    else
                        fullpath = boost::filesystem::canonical(fileAttr->value()).string();

                    if(boost::filesystem::exists(fullpath))
                    {
                        std::stringstream      ss;
                        ss << "scene.textures." << id << ".type = imagemap" << std::endl;
                        ss << "scene.textures." << id << ".file = " << fullpath << std::endl;

                        appendAttribute(self,id,ss,"gamma");
                        appendAttribute(self,id,ss,"gain");

                        m_slgname2filepath_map[id] = fullpath;
                        result = defineAndUpdate(id,ctx.scene(),ss.str());

                        ctx.addAction(slg::IMAGEMAPS_EDIT);
                        ctx.addAction(slg::MATERIAL_TYPES_EDIT);
                    }
                }
            }
            break;
        case slg::SCALE_TEX:
            {
                type_xml_node   *pTexture1 = self.first_node("texture1");
                type_xml_node   *pTexture2 = self.first_node("texture2");
                if(pTexture1 && pTexture2)
                {
                    std::stringstream       ss;
                    std::string id = getIdFromNode(self);
                    ss << "scene.textures." << id << ".type = scale" << std::endl;
                    ss << "scene.textures." << id << ".texture1 = " << createTexture(ctx,*pTexture1) << std::endl;
                    ss << "scene.textures." << id << ".texture2 = " << createTexture(ctx,*pTexture2) << std::endl;

                    result = defineAndUpdate(id,ctx.scene(),ss.str());
                }
            }
            break;
        case slg::FRESNEL_APPROX_N:
            {
                type_xml_node   *pTexture = self.first_node("texture");
                if(pTexture)
                {
                    std::stringstream       ss;
                    std::string id = getIdFromNode(self);

                    ss << "scene.textures." << id << ".type = fresnelapproxn" << std::endl;
                    ss << "scene.textures." << id << ".texture = " << createTexture(ctx,*pTexture) << std::endl;

                    result = defineAndUpdate(id,ctx.scene(),ss.str());
                }
            }
            break;
        case slg::FRESNEL_APPROX_K:
            {
                type_xml_node   *pTexture = self.first_node("texture");
                if(pTexture)
                {
                    std::stringstream       ss;
                    std::string id = getIdFromNode(self);

                    ss << "scene.textures." << id << ".type = fresnelapproxk" << std::endl;
                    ss << "scene.textures." << id << ".texture = " << createTexture(ctx,*pTexture) << std::endl;

                    result = defineAndUpdate(id,ctx.scene(),ss.str());
                }
            }
            break;
        case slg::MIX_TEX:
            {
                type_xml_node   *pTexture1 = self.first_node("texture1");
                type_xml_node   *pTexture2 = self.first_node("texture2");
                type_xml_node   *pAmount = self.first_node("amount");

                if(pTexture1 && pTexture2 && pAmount)
                {
                    std::stringstream       ss;
                    std::string id = getIdFromNode(self);

                    ss << "scene.textures." << id << ".type = mix" << std::endl;
                    ss << "scene.textures." << id << ".amount = " << createTexture(ctx,*pAmount) << std::endl;
                    ss << "scene.textures." << id << ".texture1 = " << createTexture(ctx,*pTexture1) << std::endl;
                    ss << "scene.textures." << id << ".texture2 = " << createTexture(ctx,*pTexture2) << std::endl;

                    result = defineAndUpdate(id,ctx.scene(),ss.str());
                }
            }
            break;
        case slg::ADD_TEX:
            {
                type_xml_node   *pTexture1 = self.first_node("texture1");
                type_xml_node   *pTexture2 = self.first_node("texture2");

                if(pTexture1 && pTexture2)
                {
                    std::stringstream       ss;
                    std::string id = getIdFromNode(self);

                    ss << "scene.textures." << id << ".type = add" << std::endl;
                    ss << "scene.textures." << id << ".texture1 = " << createTexture(ctx,*pTexture1) << std::endl;
                    ss << "scene.textures." << id << ".texture2 = " << createTexture(ctx,*pTexture2) << std::endl;

                    result = defineAndUpdate(id,ctx.scene(),ss.str());
                }
            }
            break;
        case slg::CHECKERBOARD2D:
            {
                type_xml_node   *pTexture1 = self.first_node("texture1");
                type_xml_node   *pTexture2 = self.first_node("texture2");
                if(pTexture1 && pTexture2)
                {
                    std::stringstream       ss;
                    std::string id = getIdFromNode(self);

                    ss << "scene.textures." << id << ".type = checkerboard2d" << std::endl;
                    ss << "scene.textures." << id << ".texture1 = " << createTexture(ctx,*pTexture1) << std::endl;
                    ss << "scene.textures." << id << ".texture2 = " << createTexture(ctx,*pTexture2) << std::endl;

                    type_xml_node   *pMapping = self.first_node(constDef::mapping);
                    if(pMapping)
                    {
                        importTextureMapping2D(ss,*pMapping,id);
                    }

                    result = defineAndUpdate(id,ctx.scene(),ss.str());
                }
            }
            break;
        case slg::CHECKERBOARD3D:
            {
                type_xml_node   *pTexture1 = self.first_node("texture1");
                type_xml_node   *pTexture2 = self.first_node("texture2");

                if(pTexture1 && pTexture2)
                {
                    std::stringstream       ss;
                    std::string id = getIdFromNode(self);

                    ss << "scene.textures." << id << ".type = checkerboard3d" << std::endl;
                    ss << "scene.textures." << id << ".texture1 = " << createTexture(ctx,*pTexture1) << std::endl;
                    ss << "scene.textures." << id << ".texture2 = " << createTexture(ctx,*pTexture2) << std::endl;

                    type_xml_node   *pMapping = self.first_node(constDef::mapping);
                    if(pMapping)
                    {
                        importTextureMapping3D(ss,*pMapping,id);
                    }

                    result = defineAndUpdate(id,ctx.scene(),ss.str());
                }
            }
            break;
        case slg::FBM_TEX:
            {
                std::stringstream   ss;
                std::string id = getIdFromNode(self);

                ss << "scene.textures." << id << ".type = fbm" << std::endl;

                appendAttribute(self,id,ss,"octaves");
                appendAttribute(self,id,ss,"roughness");

                type_xml_node   *pMapping = self.first_node(constDef::mapping);
                if(pMapping)
                {
                    importTextureMapping3D(ss,*pMapping,id);
                }

                result = defineAndUpdate(id,ctx.scene(),ss.str());
            }
            break;
        case slg::MARBLE:
            {
                std::stringstream   ss;
                std::string id = getIdFromNode(self);

                ss << "scene.textures." << id << ".type = marble" << std::endl;

                appendAttribute(self,id,ss,"octaves");
                appendAttribute(self,id,ss,"roughness");
                appendAttribute(self,id,ss,"scale");
                appendAttribute(self,id,ss,"variation");

                type_xml_node   *pMapping = self.first_node(constDef::mapping);
                if(pMapping)
                {
                    importTextureMapping3D(ss,*pMapping,id);
                }

                result = defineAndUpdate(id,ctx.scene(),ss.str());
            }
            break;
        case slg::DOTS:
            {
                type_xml_node   *pinside = self.first_node("inside");
                type_xml_node   *poutside = self.first_node("outside");

                if(pinside && poutside)
                {
                    std::stringstream       ss;
                    std::string id = getIdFromNode(self);

                    ss << "scene.textures." << id << ".type = dots" << std::endl;
                    ss << "scene.textures." << id << ".inside = " << createTexture(ctx,*pinside) << std::endl;
                    ss << "scene.textures." << id << ".outside = " << createTexture(ctx,*poutside) << std::endl;

                    type_xml_node   *pMapping = self.first_node(constDef::mapping);
                    if(pMapping)
                    {
                        importTextureMapping2D(ss,*pMapping,id);
                    }

                    result = defineAndUpdate(id,ctx.scene(),ss.str());
                }
            }
            break;
        case slg::BRICK:
            {
                type_xml_node   *bricktex = self.first_node("bricktex");
                type_xml_node   *mortartex = self.first_node("mortartex");
                type_xml_node   *brickmodtex = self.first_node("brickmodtex");

                if(bricktex && mortartex && brickmodtex)
                {
                    std::stringstream       ss;
                    std::string id = getIdFromNode(self);

                    ss << "scene.textures." << id << ".type = brick" << std::endl;
                    ss << "scene.textures." << id << ".bricktex = " << createTexture(ctx,*bricktex) << std::endl;
                    ss << "scene.textures." << id << ".mortartex = " << createTexture(ctx,*mortartex) << std::endl;
                    ss << "scene.textures." << id << ".brickmodtex = " << createTexture(ctx,*brickmodtex) << std::endl;

                    appendAttribute(self,id,ss,"brickbond");
                    appendAttribute(self,id,ss,"brickwidth");
                    appendAttribute(self,id,ss,"brickheight");
                    appendAttribute(self,id,ss,"brickdepth");
                    appendAttribute(self,id,ss,"mortarsize");
                    appendAttribute(self,id,ss,"brickrun");
                    appendAttribute(self,id,ss,"brickbevel");

                    type_xml_node   *pMapping = self.first_node(constDef::mapping);
                    if(pMapping)
                    {
                        importTextureMapping3D(ss,*pMapping,id);
                    }

                    result = defineAndUpdate(id,ctx.scene(),ss.str());
                }
            }
            break;
        case slg::WINDY:
            {
                std::stringstream       ss;
                std::string id = getIdFromNode(self);
                ss << "scene.textures." << id << ".type = windy" << std::endl;

                type_xml_node   *pMapping = self.first_node(constDef::mapping);
                if(pMapping)
                {
                    importTextureMapping3D(ss,*pMapping,id);
                }

                result = defineAndUpdate(id,ctx.scene(),ss.str());
            }
            break;
        case slg::WRINKLED:
            {
                std::stringstream       ss;
                std::string id = getIdFromNode(self);
                ss << "scene.textures." << id << ".type = wrinkled" << std::endl;

                appendAttribute(self,id,ss,"octaves");
                appendAttribute(self,id,ss,"roughness");


                type_xml_node   *pMapping = self.first_node(constDef::mapping);
                if(pMapping)
                {
                    importTextureMapping3D(ss,*pMapping,id);
                }

                result = defineAndUpdate(id,ctx.scene(),ss.str());
            }
            break;
        case slg::UV_TEX:
            {
                std::stringstream       ss;
                std::string id = getIdFromNode(self);
                ss << "scene.textures." << id << ".type = uv" << std::endl;

                type_xml_node   *pMapping = self.first_node(constDef::mapping);
                if(pMapping)
                {
                    importTextureMapping2D(ss,*pMapping,id);
                }

                result = defineAndUpdate(id,ctx.scene(),ss.str());
            }
            break;
        case slg::BAND_TEX:
            {
//                type_xml_node   *amount = self.first_node("amount");
//
//                if(amount)
//                {
//                    ss << "scene.textures." << id << ".type = band" << std::endl;
//                    ss << "scene.textures." << id << ".amount = " << createTexture(ctx,*amount) << std::endl;
//                }
                BOOST_ASSERT_MSG(false,"NOT IMPLEMENT");
            }
            break;
        }
    }

    if(result.length() == 0)
    {//fall back to greay.
        result = "0.75 0.75 0.75";
    }
    return result;
}

std::string
ExtraTextureManager::buildDefaultTexture(SlgUtil::UpdateContext &ctx,const slg::Texture *pTex,int type)
{
    //first,remove all texture definition from ctx.
    const std::string &id = this->getTextureId(pTex);
    if(!id.empty())
    {
        std::string prefix = "scene.textures." + id;
        std::vector< std::string >  texKeys = ctx.props.GetAllKeys (prefix);
        BOOST_FOREACH(const std::string &key,texKeys)
        {
            ctx.props.Delete(key);
        }
        //不需要删除附加资源，最后在onTextureRemoved调用中会把id相关资源(路径以及tex2id项)删除。
    }

    switch(type)
    {
    case slg::CONST_FLOAT3:
        return "1.0 1.0 1.0";
    case slg::CONST_FLOAT:
        return "1.0";
    case slg::IMAGEMAP:
        {//we need generate new id.
            std::string fullpath;
            if(ctx.getImageFilepath(fullpath))
            {
                std::string     id = string::uuid_to_string(boost::uuids::random_generator()());
                std::stringstream   ss;
                ss << "scene.textures." << id << ".type = imagemap" << std::endl;
                ss << "scene.textures." << id << ".file = " << fullpath << std::endl;

//                ctx.props.SetString("scene.textures." + id + ".type","imagemap");
//                ctx.props.SetString("scene.textures." + id + ".file",fullpath);

                ctx.editor.scene()->DefineTextures(ss.str());

                ctx.editor.addAction(slg::IMAGEMAPS_EDIT);
                ctx.editor.addAction(slg::MATERIAL_TYPES_EDIT);
//                ctx.editor.needRefresh(true);

                m_slgname2filepath_map[id] = fullpath;
                m_tex2id[ctx.editor.scene()->texDefs.GetTexture(id)] = id;

                return id;
            }else{
                ctx.bVeto = true;
                return "";
            }
        }
    case slg::SCALE_TEX:
    case slg::FRESNEL_APPROX_N:
    case slg::FRESNEL_APPROX_K:
    case slg::MIX_TEX:
    case slg::ADD_TEX:
    case slg::CHECKERBOARD2D:
    case slg::CHECKERBOARD3D:
    case slg::FBM_TEX:
    case slg::MARBLE:
    case slg::DOTS:
    case slg::BRICK:
    case slg::WINDY:
    case slg::WRINKLED:
    case slg::UV_TEX:
        {
            std::string     id = string::uuid_to_string(boost::uuids::random_generator()());
            std::stringstream   ss;
            ss << "scene.textures." << id << ".type = " << DocMat::texGetTypeNameFromType(type) << std::endl;
            ctx.editor.scene()->DefineTextures(ss.str());
            m_tex2id[ctx.editor.scene()->texDefs.GetTexture(id)] = id;
            return id;
        }
    case slg::BAND_TEX:
        BOOST_ASSERT_MSG(false,"not implement");
        break;
    default:
        if(type == DocMat::TEX_DISABLE)
        {
            //return empty is enough.
            break;
        }else if(type == DocMat::TEX_IES)
        {//process ies here.
        }
        BOOST_ASSERT_MSG(false,"unreachable code");
    }
    return "";
}

slg::Texture*
ExtraTextureManager::getSlgTexture(const std::string &id)
{
    return Doc::instance().pDocData->m_session->renderConfig->scene->texDefs.GetTexture(id);
}

const slg::Texture*
ExtraTextureManager::getTextureFromKeypath(const slg::Texture *pTex,const std::vector<std::string> &keyPath,size_t curIdx)
{
    if(curIdx == keyPath.size())
        return pTex;
    const std::string &curKey = keyPath[curIdx];
    switch(pTex->GetType())
    {
    case slg::CONST_FLOAT3:
    case slg::CONST_FLOAT:
    case slg::IMAGEMAP:
        return pTex;
    case slg::SCALE_TEX:
        if(curKey == "texture1")
        {
            return getTextureFromKeypath(dynamic_cast<const slg::ScaleTexture*>(pTex)->GetTexture1(),keyPath,curIdx+1);
        }else if(curKey == "texture2")
        {
            return getTextureFromKeypath(dynamic_cast<const slg::ScaleTexture*>(pTex)->GetTexture2(),keyPath,curIdx+1);
        }else{
            BOOST_ASSERT_MSG(false,"unreachable code");
        }
        break;
    case slg::FRESNEL_APPROX_N:
        if(curKey == constDef::texture)
        {
            return getTextureFromKeypath(dynamic_cast<const slg::FresnelApproxNTexture*>(pTex)->GetTexture(),keyPath,curIdx+1);
        }else{
            BOOST_ASSERT_MSG(false,"unreachable code");
        }
        break;
    case slg::FRESNEL_APPROX_K:
        if(curKey == constDef::texture)
        {
            return getTextureFromKeypath(dynamic_cast<const slg::FresnelApproxKTexture*>(pTex)->GetTexture(),keyPath,curIdx+1);
        }else{
            BOOST_ASSERT_MSG(false,"unreachable code");
        }
        break;
    case slg::MIX_TEX:
        if(curKey == constDef::amount)
        {
            return getTextureFromKeypath(dynamic_cast<const slg::MixTexture*>(pTex)->GetAmountTexture(),keyPath,curIdx+1);
        }else if(curKey == "texture1")
        {
            return getTextureFromKeypath(dynamic_cast<const slg::MixTexture*>(pTex)->GetTexture1(),keyPath,curIdx+1);
        }else if(curKey == "texture2")
        {
            return getTextureFromKeypath(dynamic_cast<const slg::MixTexture*>(pTex)->GetTexture2(),keyPath,curIdx+1);
        }else{
            BOOST_ASSERT_MSG(false,"unreachable code");
        }
        break;
    case slg::ADD_TEX:
        if(curKey == "texture1")
        {
            return getTextureFromKeypath(dynamic_cast<const slg::AddTexture*>(pTex)->GetTexture1(),keyPath,curIdx+1);
        }else if(curKey == "texture2")
        {
            return getTextureFromKeypath(dynamic_cast<const slg::AddTexture*>(pTex)->GetTexture2(),keyPath,curIdx+1);
        }else{
            BOOST_ASSERT_MSG(false,"unreachable code");
        }
        break;
    case slg::CHECKERBOARD2D:
        if(curKey == "texture1")
        {
            return getTextureFromKeypath(dynamic_cast<const slg::CheckerBoard2DTexture*>(pTex)->GetTexture1(),keyPath,curIdx+1);
        }else if(curKey == "texture2")
        {
            return getTextureFromKeypath(dynamic_cast<const slg::CheckerBoard2DTexture*>(pTex)->GetTexture2(),keyPath,curIdx+1);
        }
        //for mapping2d.
        return pTex;
    case slg::CHECKERBOARD3D:
        if(curKey == "texture1")
        {
            return getTextureFromKeypath(dynamic_cast<const slg::CheckerBoard3DTexture*>(pTex)->GetTexture1(),keyPath,curIdx+1);
        }else if(curKey == "texture2")
        {
            return getTextureFromKeypath(dynamic_cast<const slg::CheckerBoard3DTexture*>(pTex)->GetTexture2(),keyPath,curIdx+1);
        }
        //for mapping3d.
        return pTex;
    case slg::FBM_TEX:
    case slg::MARBLE:
        //not a compose.
        return pTex;
    case slg::DOTS:
        if(curKey == "inside")
        {
            return getTextureFromKeypath(dynamic_cast<const slg::DotsTexture*>(pTex)->GetInsideTex(),keyPath,curIdx+1);
        }else if(curKey == "outside")
        {
            return getTextureFromKeypath(dynamic_cast<const slg::DotsTexture*>(pTex)->GetOutsideTex(),keyPath,curIdx+1);
        }
        //for mapping2d.
        return pTex;
    case slg::BRICK:
        if(curKey == "bricktex")
        {
            return getTextureFromKeypath(dynamic_cast<const slg::BrickTexture*>(pTex)->GetTexture1(),keyPath,curIdx+1);
        }else if(curKey == "mortartex")
        {
            return getTextureFromKeypath(dynamic_cast<const slg::BrickTexture*>(pTex)->GetTexture2(),keyPath,curIdx+1);
        }else if(curKey == "brickmodtex")
        {
            return getTextureFromKeypath(dynamic_cast<const slg::BrickTexture*>(pTex)->GetTexture3(),keyPath,curIdx+1);
        }
        //for mapping3d.
        return pTex;
    case slg::WINDY:
    case slg::WRINKLED:
    case slg::UV_TEX:
        //for mapping, no child
        return pTex;
    case slg::BAND_TEX:
        BOOST_ASSERT_MSG(false,"not implement");
        break;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code");
    }
    throw std::runtime_error("reach unreachable code!");
}

bool
ExtraTextureManager::checkTextureMapping2DUpdate(SlgUtil::UpdateContext &ctx,luxrays::Properties &newProps,const std::string &curKey,const std::string &mapping_prefix,const slg::TextureMapping2D *pMapping)
{
    const slg::UVMapping2D *pMap2D = dynamic_cast<const slg::UVMapping2D*>(pMapping);
    BOOST_ASSERT_MSG(pMap2D != NULL,"new uv-mapping type?");
    bool    bMatch = true;
    if(curKey == "uscale")
    {
        newProps.SetString(mapping_prefix + "uvscale",boost::str(boost::format("%s %f")%ctx.value%pMap2D->vScale));
    }else if(curKey == "vscale")
    {
        newProps.SetString(mapping_prefix + "uvscale",boost::str(boost::format("%f %s")%pMap2D->uScale%ctx.value));
    }else if(curKey == "udelta")
    {
        newProps.SetString(mapping_prefix + "uvdelta",boost::str(boost::format("%s %f")%ctx.value%pMap2D->vDelta));
    }else if(curKey == "vdelta")
    {
        newProps.SetString(mapping_prefix + "uvdelta",boost::str(boost::format("%f %s")%pMap2D->uDelta%ctx.value));
    }else{
        bMatch = false;
    }
    return bMatch;
}

struct  updateCompTexture
{
    ExtraTextureManager     &texManager;
    const std::string       &oldId;
    SlgUtil::UpdateContext  &context;
    luxrays::Properties     newProps;
    std::string             prefix;
    std::string             newId;
    updateCompTexture(ExtraTextureManager &tm,const slg::Texture *pTex,SlgUtil::UpdateContext  &ctx) : texManager(tm),oldId(tm.getTextureId(pTex)),context(ctx)
    {
        BOOST_ASSERT_MSG(!oldId.empty(),"invalid scale texture!");
        prefix = "scene.textures.";
        newId = SlgUtil::propReplaceNewId(context.props,newProps,oldId,prefix);
    }

    inline  bool    checkTextureMapping2DUpdate(const std::string &curKey,const slg::TextureMapping2D *pMapping)
    {
        return texManager.checkTextureMapping2DUpdate(context,newProps,curKey,prefix + newId + '.' + constDef::mapping + '.',pMapping);
    }

    inline  bool    checkUpdateField(const std::string &curKey,const std::string &subString)
    {
        if(curKey == subString)
        {
            newProps.SetString(prefix + newId + '.' + subString ,context.value);
            return true;
        }
        return false;
    }

    inline  bool    checkUpdateChild(const std::string &curKey,const std::string &subString,const slg::Texture *pChildTex,size_t nextIdx)
    {
        if(curKey == subString)
        {
            std::string value = texManager.updateTexture(context,pChildTex,nextIdx);
            newProps.SetString(prefix + newId + '.' + subString ,value);
            return true;
        }
        return false;
    }

    inline std::string     getResult(void)
    {
        if(!context.bVeto)
        {//如果用户没有终止。定义并更新贴图。
            context.editor.scene()->DefineTextures(newProps);
            texManager.m_tex2id[context.editor.scene()->texDefs.GetTexture(newId)] = newId;
            // this is fixed by david. slg的递归处理有bug,需要强制刷新，这在多GPU显卡环境下是噩梦....
//            context.editor.needRefresh(true);
            return newId;
        }
        return "";
    }
};

std::string
ExtraTextureManager::updateTexture(SlgUtil::UpdateContext &ctx,const slg::Texture *pTex,size_t curIdx)
{
    if(curIdx == ctx.keyPath.size())
    {//最后一级。这里就必然是material type.
        int type = boost::lexical_cast<int>(ctx.value);

//        std::cerr << "boost::lexical_cast<int>(ctx.value)=" << type << std::endl;

        ctx.updatedId = buildDefaultTexture(ctx,pTex,type);
        ///@fixme: 修改slg代码以禁止强制刷新。
//        ctx.editor.needRefresh(true);
        ctx.idIsMat = false;
        ctx.bGenNode = true;
        return ctx.updatedId;
    }else
    {
        const std::string &curKey = ctx.keyPath[curIdx];
        switch(pTex->GetType())
        {
        case slg::CONST_FLOAT3:
            BOOST_ASSERT_MSG(curKey == "value","invalid key!");
            return ctx.value;
        case slg::CONST_FLOAT:
            BOOST_ASSERT_MSG(curKey == "value","invalid key!");
            return ctx.value;
        case slg::IMAGEMAP:
            {//need implement a replaceId to operate ctx.props.
                const std::string  &oldId = this->getTextureId(pTex);
                luxrays::Properties newProps;
                std::string prefix = "scene.textures.";
                std::string newId = SlgUtil::propReplaceNewId(ctx.props,newProps,oldId,"scene.textures.");

                bool    bNeedRefresh = false;

                if(curKey == constDef::file)
                {
                    newProps.SetString(prefix + newId + ".file",ctx.value);
                    m_slgname2filepath_map[newId] = ctx.value;
                    ctx.editor.addAction(slg::IMAGEMAPS_EDIT);
                    ctx.editor.addAction(slg::MATERIAL_TYPES_EDIT);
                    bNeedRefresh = true;
                }else if(curKey == "gamma")
                {
                    newProps.SetString(prefix + newId + ".gamma",ctx.value);
                }else if(curKey == "gain")
                {
                    newProps.SetString(prefix + newId + ".gain",ctx.value);
                }else if(checkTextureMapping2DUpdate(ctx,newProps,curKey,
                    prefix + newId + '.' + constDef::mapping + '.',
                    dynamic_cast<const slg::ImageMapTexture*>(pTex)->GetTextureMapping()))
                {
                }else{
                    BOOST_ASSERT_MSG(false,"unreachable code.");
                }

                //std::cerr << newProps.ToString() << std::endl;
                ctx.editor.scene()->DefineTextures(newProps);

                //ctx.editor.needRefresh(bNeedRefresh);

                m_tex2id[ctx.editor.scene()->texDefs.GetTexture(newId)] = newId;
                this->addPath(newId,m_slgname2filepath_map[oldId]);
                m_slgname2filepath_map.erase(oldId);
                return newId;
            }
            break;
        case slg::SCALE_TEX:
            {
                const slg::ScaleTexture *pRealTex = dynamic_cast<const slg::ScaleTexture*>(pTex);

                updateCompTexture   uct(*this,pTex,ctx);

                if(uct.checkUpdateChild(curKey,"texture1",pRealTex->GetTexture1(),curIdx+1))
                {
                }else if(uct.checkUpdateChild(curKey,"texture2",pRealTex->GetTexture2(),curIdx+1))
                {
                }else{
                    BOOST_ASSERT_MSG(false,"unreachable code.");
                }

                return uct.getResult();
            }
        case slg::FRESNEL_APPROX_N:
            {
                const slg::FresnelApproxNTexture *pRealTex = dynamic_cast<const slg::FresnelApproxNTexture*>(pTex);

                updateCompTexture   uct(*this,pTex,ctx);

                if(uct.checkUpdateChild(curKey,constDef::texture,pRealTex->GetTexture(),curIdx+1))
                {
                }else{
                    BOOST_ASSERT_MSG(false,"unreachable code.");
                }

                return uct.getResult();
            }
            break;
        case slg::FRESNEL_APPROX_K:
            {
                const slg::FresnelApproxKTexture *pRealTex = dynamic_cast<const slg::FresnelApproxKTexture*>(pTex);

                updateCompTexture   uct(*this,pTex,ctx);

                if(uct.checkUpdateChild(curKey,constDef::texture,pRealTex->GetTexture(),curIdx+1))
                {
                }else{
                    BOOST_ASSERT_MSG(false,"unreachable code.");
                }

                return uct.getResult();
            }
        case slg::MIX_TEX:
            {
                const slg::MixTexture *pRealTex = dynamic_cast<const slg::MixTexture*>(pTex);

                updateCompTexture   uct(*this,pTex,ctx);

                if(uct.checkUpdateChild(curKey,constDef::amount,pRealTex->GetAmountTexture(),curIdx+1))
                {
                }else if(uct.checkUpdateChild(curKey,"texture1",pRealTex->GetTexture1(),curIdx+1))
                {
                }else if(uct.checkUpdateChild(curKey,"texture2",pRealTex->GetTexture2(),curIdx+1))
                {
                }else{
                    BOOST_ASSERT_MSG(false,"unreachable code.");
                }
                return uct.getResult();
            }
        case slg::ADD_TEX:
            {
                const slg::AddTexture *pRealTex = dynamic_cast<const slg::AddTexture*>(pTex);

                updateCompTexture   uct(*this,pTex,ctx);

                if(uct.checkUpdateChild(curKey,"texture1",pRealTex->GetTexture1(),curIdx+1))
                {
                }else if(uct.checkUpdateChild(curKey,"texture2",pRealTex->GetTexture2(),curIdx+1))
                {
                }else{
                    BOOST_ASSERT_MSG(false,"unreachable code.");
                }
                return uct.getResult();
            }
        case slg::CHECKERBOARD2D:
            {
                const slg::CheckerBoard2DTexture *pRealTex = dynamic_cast<const slg::CheckerBoard2DTexture*>(pTex);

                updateCompTexture   uct(*this,pTex,ctx);

                if(uct.checkUpdateChild(curKey,"texture1",pRealTex->GetTexture1(),curIdx+1))
                {
                }else if(uct.checkUpdateChild(curKey,"texture2",pRealTex->GetTexture2(),curIdx+1))
                {
                }else if(uct.checkTextureMapping2DUpdate(curKey,pRealTex->GetTextureMapping()))
                {
                }else{
                    BOOST_ASSERT_MSG(false,"unreachable code.");
                }
                return uct.getResult();
            }
        case slg::CHECKERBOARD3D:
            {
                const slg::CheckerBoard3DTexture *pRealTex = dynamic_cast<const slg::CheckerBoard3DTexture*>(pTex);

                updateCompTexture   uct(*this,pTex,ctx);

                if(uct.checkUpdateChild(curKey,"texture1",pRealTex->GetTexture1(),curIdx+1))
                {
                }else if(uct.checkUpdateChild(curKey,"texture2",pRealTex->GetTexture2(),curIdx+1))
                {
                }/*else if(uct.checkTextureMapping3DUpdate(curKey,pRealTex->GetTextureMapping()))
                {
                }*/else{
                    BOOST_ASSERT_MSG(false,"not implement.");
                }
                return uct.getResult();
            }
        case slg::FBM_TEX:
            {
//                const slg::FBMTexture *pRealTex = dynamic_cast<const slg::FBMTexture*>(pTex);

                updateCompTexture   uct(*this,pTex,ctx);

                if(uct.checkUpdateField(curKey,"octaves"))
                {
                }else if(uct.checkUpdateField(curKey,"roughness"))
                {
                }/*else if(uct.checkTextureMapping3DUpdate(curKey,pRealTex->GetTextureMapping()))
                {
                }*/else{
                    BOOST_ASSERT_MSG(false,"not implement.");
                }
                return uct.getResult();
            }
        case slg::MARBLE:
            {
//                const slg::MarbleTexture *pRealTex = dynamic_cast<const slg::MarbleTexture*>(pTex);

                updateCompTexture   uct(*this,pTex,ctx);

                if(uct.checkUpdateField(curKey,"octaves"))
                {
                }else if(uct.checkUpdateField(curKey,"roughness"))
                {
                }else if(uct.checkUpdateField(curKey,"scale"))
                {
                }else if(uct.checkUpdateField(curKey,"variation"))
                {
                }/*else if(uct.checkTextureMapping3DUpdate(curKey,pRealTex->GetTextureMapping()))
                {
                }*/else{
                    BOOST_ASSERT_MSG(false,"not implement.");
                }
                return uct.getResult();
            }
        case slg::DOTS:
            {
                const slg::DotsTexture *pRealTex = dynamic_cast<const slg::DotsTexture*>(pTex);

                updateCompTexture   uct(*this,pTex,ctx);

                if(uct.checkUpdateChild(curKey,"inside",pRealTex->GetInsideTex(),curIdx+1))
                {
                }else if(uct.checkUpdateChild(curKey,"outside",pRealTex->GetOutsideTex(),curIdx+1))
                {
                }else if(uct.checkTextureMapping2DUpdate(curKey,pRealTex->GetTextureMapping()))
                {
                }else{
                    BOOST_ASSERT_MSG(false,"not implement.");
                }
                return uct.getResult();
            }
        case slg::BRICK:
        {
            const slg::BrickTexture *pRealTex = dynamic_cast<const slg::BrickTexture*>(pTex);

            updateCompTexture   uct(*this,pTex,ctx);

            if(uct.checkUpdateChild(curKey,"bricktex",pRealTex->GetTexture1(),curIdx+1))
            {
            }else if(uct.checkUpdateChild(curKey,"mortartex",pRealTex->GetTexture2(),curIdx+1))
            {
            }else if(uct.checkUpdateChild(curKey,"brickmodtex",pRealTex->GetTexture3(),curIdx+1))
            {
            }else if(curKey == "brickbond"){
                int type = boost::lexical_cast<int>(ctx.value);
                uct.newProps.SetString(uct.prefix + uct.newId + '.' + "brickbond" ,getBondnameFromType((slg::MasonryBond)type));
            }else if(uct.checkUpdateField(curKey,"brickwidth")){
            }else if(uct.checkUpdateField(curKey,"brickheight")){
            }else if(uct.checkUpdateField(curKey,"brickdepth")){
            }else if(uct.checkUpdateField(curKey,"mortarsize")){
            }else if(uct.checkUpdateField(curKey,"brickrun")){
            }else if(uct.checkUpdateField(curKey,"brickbevel")){
            }/*else if(uct.checkTextureMapping3DUpdate(curKey,pRealTex->GetTextureMapping()))
            {
            }*/else{
                BOOST_ASSERT_MSG(false,"not implement.");
            }
            return uct.getResult();
        }
        case slg::WINDY:
        {
//            const slg::WindyTexture *pRealTex = dynamic_cast<const slg::WindyTexture*>(pTex);
            updateCompTexture   uct(*this,pTex,ctx);
            /*if(uct.checkTextureMapping3DUpdate(curKey,pRealTex->GetTextureMapping()))
            {
            }else{
                BOOST_ASSERT_MSG(false,"not implement.");
            }*/
            return uct.getResult();
        }
        case slg::WRINKLED:
        {
//            const slg::WindyTexture *pRealTex = dynamic_cast<const slg::WindyTexture*>(pTex);
            updateCompTexture   uct(*this,pTex,ctx);

            if(uct.checkUpdateField(curKey,"octaves"))
            {
            }else if(uct.checkUpdateField(curKey,"roughness"))
            {
            }/*else if(uct.checkTextureMapping3DUpdate(curKey,pRealTex->GetTextureMapping()))
            {
            }*/else{
                BOOST_ASSERT_MSG(false,"not implement.");
            }
            return uct.getResult();
        }
        case slg::UV_TEX:
        {
            const slg::UVTexture *pRealTex = dynamic_cast<const slg::UVTexture*>(pTex);
            updateCompTexture   uct(*this,pTex,ctx);

            if(uct.checkTextureMapping2DUpdate(curKey,pRealTex->GetTextureMapping()))
            {
            }else{
                BOOST_ASSERT_MSG(false,"not implement.");
            }
            return uct.getResult();
        }
        case slg::BAND_TEX:
            BOOST_ASSERT_MSG(false,"not implement");
            break;
        default:
            BOOST_ASSERT_MSG(false,"unreachable code");
        }
    }
    throw std::runtime_error("reach unreachable code!");
}

bool
ExtraTextureManager::defineImageMapTexture(ImportContext &ctx,const std::string &src,std::string &id)
{
    std::string fullpath = ctx.findFile(src,true);
    if(fullpath.length())
    {
        std::stringstream   tex;
        if(id.length() == 0)
            id = string::uuid_to_string(boost::uuids::random_generator()());

        tex << "scene.textures." << id << ".type = imagemap" << std::endl;
        tex << "scene.textures." << id << ".file = " << fullpath << std::endl;
        ctx.scene()->DefineTextures(tex.str());

        m_slgname2filepath_map[id] = fullpath;
        m_tex2id[ctx.scene()->texDefs.GetTexture(id)] = id;

        ctx.addAction(slg::IMAGEMAPS_EDIT);
        ctx.addAction(slg::MATERIAL_TYPES_EDIT);
        return true;
    }
    return false;
}



}
