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
#include "utils/pathext.h"
#include "dm/doc.h"
#include "dm/setting.h"
#include "slgsetting.h"
#include "docprivate.h"
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>


namespace gme{

std::string
ExtraSettingManager::nameFromFiltertype(int filter)
{
    switch(filter)
    {
    case slg::FILTER_NONE:
        return "NONE";
    case slg::FILTER_BOX:
        return "BOX";
    case slg::FILTER_GAUSSIAN:
        return "GAUSSIAN";
    case slg::FILTER_MITCHELL:
        return "MITCHELL";
    case slg::FILTER_MITCHELL_SS:
        return "MITCHELL_SS";
    }
    throw std::runtime_error("Unknown filter type");
}

std::string
ExtraSettingManager::nameFromRenderengine(int enginetype)
{
    switch(enginetype)
    {
    case slg::PATHOCL:
        return "PATHOCL";
    case slg::LIGHTCPU:
        return "LIGHTCPU";
    case slg::PATHCPU:
        return "PATHCPU";
    case slg::BIDIRCPU:
        return "BIDIRCPU";
    case slg::BIDIRHYBRID:
        return "BIDIRHYBRID";
    case slg::CBIDIRHYBRID:
        return "CBIDIRHYBRID";
    case slg::BIDIRVMCPU:
        return "BIDIRVMCPU";
    case slg::RTPATHOCL:
        return "RTPATHOCL";
    case slg::FILESAVER:
        return "FILESAVER";
    }
    throw std::runtime_error("Unknown render engine type");
}


void
ExtraSettingManager::dumpSettings(type_xml_node &parent,dumpContext &ctx)
{
    type_xml_doc *pDoc = parent.document();
    BOOST_ASSERT_MSG(pDoc != NULL,"invalid node,must from doc");

    slg::RenderSession *session = Doc::instance().pDocData->m_session.get();

    {//dump film.
        slg::Film   *film = session->film;
        type_xml_node *pSelf = pDoc->allocate_node(NS_RAPIDXML::node_element,allocate_string(pDoc,constDef::film));
        parent.append_node(pSelf);

        //width
        pSelf->append_attribute(allocate_attribute(pDoc,constDef::width,boost::lexical_cast<std::string>(film->GetWidth())));
        //height
        pSelf->append_attribute(allocate_attribute(pDoc,constDef::height,boost::lexical_cast<std::string>(film->GetHeight())));
        //gamma
        pSelf->append_attribute(allocate_attribute(pDoc,constDef::height,boost::lexical_cast<std::string>(film->GetGamma())));
        //filter type.
        pSelf->append_attribute(allocate_attribute(pDoc,constDef::filter,nameFromFiltertype(film->GetFilterType())));

        const slg::ToneMapParams *pParams = film->GetToneMapParams();
        if(pParams)
        {
            type_xml_node *pTonemap = pDoc->allocate_node(NS_RAPIDXML::node_element,allocate_string(pDoc,constDef::tonemapping));
            pSelf->append_node(pTonemap);
            switch(pParams->GetType())
            {
            case slg::TONEMAP_LINEAR:
                {
                    const slg::LinearToneMapParams *pRealParams = static_cast<const slg::LinearToneMapParams*>(pParams);
                    pTonemap->append_attribute(allocate_attribute(pDoc,constDef::type,"linear"));
                    pTonemap->append_attribute(allocate_attribute(pDoc,constDef::scale,boost::lexical_cast<std::string>(pRealParams->scale)));
                }
                break;
            case slg::TONEMAP_NONE:
                pTonemap->append_attribute(allocate_attribute(pDoc,constDef::type,"none"));
                break;
            case slg::TONEMAP_REINHARD02:
                {
                    const slg::Reinhard02ToneMapParams *pRealParams = static_cast<const slg::Reinhard02ToneMapParams*>(pParams);
                    pTonemap->append_attribute(allocate_attribute(pDoc,constDef::type,"reinhard02"));
                    pTonemap->append_attribute(allocate_attribute(pDoc,constDef::prescale,boost::lexical_cast<std::string>(pRealParams->preScale)));
                    pTonemap->append_attribute(allocate_attribute(pDoc,constDef::postscale,boost::lexical_cast<std::string>(pRealParams->postScale)));
                    pTonemap->append_attribute(allocate_attribute(pDoc,constDef::burn,boost::lexical_cast<std::string>(pRealParams->burn)));
                }
                break;
            }
            
        }
    }

    if(clHardwareInfo::instance().getPlatforms().size() == 0)
    {//dump render engine.
        type_xml_node *pEngine = pDoc->allocate_node(NS_RAPIDXML::node_element,allocate_string(pDoc,constDef::renderengine));
        parent.append_node(pEngine);
        //type
        pEngine->append_attribute(allocate_attribute(pDoc,constDef::type,nameFromRenderengine(session->renderEngine->GetEngineType())));
    }
}

static inline
void
assignFromXmlAttr(type_xml_node *pNode,const char* attr_name,std::string &target)
{
    type_xml_attr *pAttribute = pNode->first_attribute(attr_name);
    if(pAttribute)
    {
        target = pAttribute->value();
    }
}

void
ExtraSettingManager::loadSettings(ImportContext &ctx,type_xml_node &parents)
{
    {
        type_xml_node *pfilm = parents.first_node(constDef::film);
        if(pfilm)
        {
            assignFromXmlAttr(pfilm,constDef::filter,ctx.m_film_filter_type);
            assignFromXmlAttr(pfilm,constDef::width,ctx.m_width);
            assignFromXmlAttr(pfilm,constDef::height,ctx.m_height);
            assignFromXmlAttr(pfilm,constDef::gamma,ctx.m_film_gamma);
        }
        type_xml_node *pTonemapping = pfilm->first_node(constDef::tonemapping);
        if(pTonemapping)
        {
            type_xml_attr *pType = pTonemapping->first_attribute(constDef::type);
            if(pType && boost::equals(pType->value(),"reinhard02"))
            {
                slg::Reinhard02ToneMapParams *pReinhard02 = new slg::Reinhard02ToneMapParams();
                std::string value;
                assignFromXmlAttr(pTonemapping,constDef::prescale,value);
                if(!value.empty())
                    pReinhard02->preScale = boost::lexical_cast<float>(value);
                
                value.clear();
                assignFromXmlAttr(pTonemapping,constDef::postscale,value);
                if(!value.empty())
                    pReinhard02->postScale = boost::lexical_cast<float>(value);

                value.clear();
                assignFromXmlAttr(pTonemapping,constDef::burn,value);
                if(!value.empty())
                    pReinhard02->burn = boost::lexical_cast<float>(value);

                ctx.m_pTonemapParams = pReinhard02;
            }else{
                slg::LinearToneMapParams    *pLinear = new slg::LinearToneMapParams();

                std::string value;
                assignFromXmlAttr(pTonemapping,constDef::scale,value);
                if(!value.empty())
                    pLinear->scale = boost::lexical_cast<float>(value);

                ctx.m_pTonemapParams = pLinear;
            }
        }
    }

    {
        type_xml_node *pRenderEngine = parents.first_node(constDef::renderengine);
        if(pRenderEngine)
        {
            assignFromXmlAttr(pRenderEngine,constDef::type,ctx.m_renderengine_type);
        }
    }

}

void
ExtraSettingManager::dumpLights(type_xml_node &parent,dumpContext &ctx)
{
    type_xml_doc *pDoc = parent.document();
    BOOST_ASSERT_MSG(pDoc != NULL,"invalid node,must from doc");

    slg::Scene *scene = Doc::instance().pDocData->m_session->renderConfig->scene;
    if(scene->envLight)
    {
        type_xml_node *pSelf = pDoc->allocate_node(NS_RAPIDXML::node_element,allocate_string(pDoc,constDef::envlight));
        parent.append_node(pSelf);

        if(scene->envLight->GetType() == slg::TYPE_IL )
        {
            slg::InfiniteLight  *pRealLight = dynamic_cast<slg::InfiniteLight*>(scene->envLight);
            pSelf->append_attribute(allocate_attribute(pDoc,constDef::type,"infinite"));

            //get the file path and output it.

            const std::string  &imagePath = getImageMapPath(scene->imgMapCache,pRealLight->GetImageMap());

            ///@fixme we need a document path,here imagePath may base document path.
            boost::filesystem::path src_fullpath = boost::filesystem::absolute(imagePath,ctx.target);
            std::string write_file;
            if(ctx.isCopyResource())
            {//保存资源。
	            boost::filesystem::path target = ctx.target / src_fullpath.filename();
                target = boost::filesystem::gme_ext::ensureNonExistFile(target);

                boost::filesystem::copy(src_fullpath,target);
                write_file = target.filename().string();
            }else{//不保存资源，直接保存src_filepath.
                write_file = src_fullpath.string();
            }

            pSelf->append_attribute(allocate_attribute(pDoc,constDef::file,write_file));

            luxrays::Spectrum   gain = pRealLight->GetGain();
            std::string gainStr = boost::str(boost::format("%f %f %f")%gain.r%gain.g%gain.b);
            pSelf->append_attribute(allocate_attribute_withkey(pDoc,"gain",gainStr));

            std::string shiftStr = boost::str(boost::format("%f %f")%pRealLight->GetUVMapping()->uDelta%pRealLight->GetUVMapping()->vDelta);
            pSelf->append_attribute(allocate_attribute_withkey(pDoc,"shift",shiftStr));
        }else{
            BOOST_ASSERT_MSG(scene->envLight->GetType() == slg::TYPE_IL_SKY,"new type scene env light?");
            slg::SkyLight   *pRealLight = dynamic_cast<slg::SkyLight*>(scene->envLight);

            //output type.
            pSelf->append_attribute(allocate_attribute(pDoc,constDef::type,"sky"));

            //output gain.
            luxrays::Spectrum   gain = pRealLight->GetGain();
            std::string gainStr = boost::str(boost::format("%f %f %f")%gain.r%gain.g%gain.b);
            pSelf->append_attribute(allocate_attribute_withkey(pDoc,"gain",gainStr));

            //output turbidity
            float tur = pRealLight->GetTubidity();
            if(!ExtraSettingManager::isDefault_turbidity(tur))
            {
                pSelf->append_attribute(allocate_attribute_withkey(pDoc,"turbidity",boost::lexical_cast<std::string>(tur)));
            }

            //output dir
            luxrays::Vector v = pRealLight->GetSunDir();
            std::string dirStr = boost::str(boost::format("%f %f %f") %v.x % v.y % v.z);
            pSelf->append_attribute(allocate_attribute_withkey(pDoc,"dir",dirStr));
        }
    }
    if(scene->sunLight)
    {
        type_xml_node *pSelf = pDoc->allocate_node(NS_RAPIDXML::node_element,allocate_string(pDoc,constDef::sunlight));
        parent.append_node(pSelf);

        //output dir
        luxrays::Vector v = scene->sunLight->GetDir();
        std::string dirStr = boost::str(boost::format("%f %f %f") %v.x % v.y % v.z);
        pSelf->append_attribute(allocate_attribute_withkey(pDoc,"dir",dirStr));

        //output gain.
        luxrays::Spectrum   gain = scene->sunLight->GetGain();
        std::string gainStr = boost::str(boost::format("%f %f %f")%gain.r%gain.g%gain.b);
        pSelf->append_attribute(allocate_attribute_withkey(pDoc,"gain",gainStr));

        //output turbidity
        float tur = scene->sunLight->GetTubidity();
        if(!ExtraSettingManager::isDefault_turbidity(tur))
        {
            pSelf->append_attribute(allocate_attribute_withkey(pDoc,"turbidity",boost::lexical_cast<std::string>(tur)));
        }

        //output relsize
        float relsize = scene->sunLight->GetRelSize();
        if(!ExtraSettingManager::isDefault_relSize(relsize))
        {
            pSelf->append_attribute(allocate_attribute_withkey(pDoc,"relsize",boost::lexical_cast<std::string>(relsize)));
        }
    }
}

void
ExtraSettingManager::createLights(ImportContext &ctx,type_xml_node &parents)
{
    slg::Scene  *scene = ctx.scene();
    type_xml_node   *pLights = find_child(&parents,constDef::envlight);
    if(pLights)
    {//loading env lights.
        type_xml_attr   *pAttr = pLights->first_attribute(constDef::type);
        if(pAttr)
        {
            std::stringstream    ss;
            int type = slg::TYPE_IL_SKY;
            if(boost::iequals(pAttr->value(),"infinite"))
            {
                type_xml_attr   *pFile = pLights->first_attribute(constDef::file);
                if(pFile)
                {
                    type = slg::TYPE_IL;
                    boost::filesystem::path fullpath = boost::filesystem::absolute(pFile->value(),ctx.docBasepath());
                    ss << "scene.infinitelight.file = " << fullpath.string() << std::endl;
                    type_xml_attr   *prop = pLights->first_attribute("gain");
                    if(prop)
                    {
                        ss << "scene.infinitelight.gain = " << prop->value() << std::endl;
                    }
                    prop = pLights->first_attribute("shift");
                    if(prop)
                    {
                        ss << "scene.infinitelight.shift = " << prop->value() << std::endl;
                    }
                }
            }else
            {
                type_xml_attr   *prop = pLights->first_attribute("dir");
                if(prop)
                {
                    ss << "scene.skylight.dir = " << prop->value() << std::endl;
                }
                prop = pLights->first_attribute("gain");
                if(prop)
                {
                    ss << "scene.infinitelight.gain = " << prop->value() << std::endl;
                }
                prop = pLights->first_attribute("turbidity");
                if(prop)
                {
                    ss << "scene.infinitelight.turbidity = " << prop->value() << std::endl;
                }
            }
            if(!ss.str().empty())
            {
                if(scene->envLight)
                {
                    delete scene->envLight;
                    scene->envLight = NULL;
                }
                if(type == slg::TYPE_IL)
                {
                    scene->AddInfiniteLight(ss.str());
                }else{
                    scene->AddSkyLight(ss.str());
                }
            }
        }
    }

    pLights = find_child(&parents,constDef::sunlight);
    if(pLights)
    {
        type_xml_attr   *pAttr = pLights->first_attribute("dir");
        if(pAttr)
        {
            std::stringstream   ss;
            ss << "scene.sunlight.dir = " << pAttr->value() << std::endl;

            type_xml_attr *prop = pLights->first_attribute("gain");
            if(prop)
            {
                ss << "scene.sunlight.gain = " << prop->value() << std::endl;
            }
            prop = pLights->first_attribute("turbidity");
            if(prop)
            {
                ss << "scene.sunlight.turbidity = " << prop->value() << std::endl;
            }
            prop = pLights->first_attribute("relsize");
            if(prop)
            {
                ss << "scene.sunlight.relsize = " << prop->value() << std::endl;
            }
            scene->AddSunLight(ss.str());
        }
    }
}


int
ExtraSettingManager::getLighterNumber(slg::Scene *scene)
{
    int litNum = 0;
    if(scene->envLight)
        litNum++;
    if(scene->sunLight)
        litNum++;

    BOOST_FOREACH(u_int offset,scene->meshTriLightDefsOffset)
    {
        if(offset != NULL_INDEX)
        {
            litNum++;
        }
    }
    return litNum;
}

bool
ExtraSettingManager::hasTwoOrMoreLighter(slg::Scene *scene)
{
    int litNum = 0;
    if(scene->envLight)
        litNum++;
    if(scene->sunLight)
        litNum++;
    if(litNum >= 2)
        return true;

    //u_int NULL_INDEX = (u_int)0xFFFFFFFF;
    BOOST_FOREACH(u_int offset,scene->meshTriLightDefsOffset)
    {
        if(offset != NULL_INDEX && ++litNum >= 2)
        {
            return true;
        }
    }
    return false;
}


const std::string&
ExtraSettingManager::getImageMapPath(const slg::ImageMapCache &imcache,const slg::ImageMap *im)
{
    return imcache.GetPath(im);
}



}
