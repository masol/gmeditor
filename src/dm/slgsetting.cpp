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
#include "slgsetting.h"
#include "docprivate.h"
#include <boost/format.hpp>


namespace gme{

void
ExtraSettingManager::dump(type_xml_node &parent,dumpContext &ctx)
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
            boost::filesystem::path src_fullpath = boost::filesystem::absolute(imagePath,boost::filesystem::current_path());
            std::string write_file;
            if(ctx.isCopyResource())
            {//保存资源。
	            boost::filesystem::path target = ctx.target / src_fullpath.filename();
                target = boost::filesystem::gme_ext::ensureNonExistFile(target);

                boost::filesystem::copy(src_fullpath,target);
                write_file = target.filename().string();
            }else{//不保存资源，直接保存src_filepath.
                write_file = src_fullpath.filename().string();
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
    for (std::map<std::string, slg::ImageMap *>::const_iterator it = imcache.maps.begin(); it != imcache.maps.end(); ++it)
    {
        if (it->second == im)
            return it->first;
    }
    throw std::runtime_error("im not defined");
}



}
