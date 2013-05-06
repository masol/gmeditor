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
#include "dm/setting.h"
#include "dm/docsetting.h"
#include "slg/slg.h"
#include "docprivate.h"
#include "slgmaterial.h"
#include "slgsetting.h"
#include <boost/assert.hpp>


namespace gme
{

//void
//DocSetting::getEnvLight(type_xml_node &parent)
//{
//    dumpContext ctx(dumpContext::DUMP_NORMAL,boost::filesystem::current_path());
//    ExtraSettingManager::dump(parent,ctx);
//}

const std::string&
DocSetting::getHDRLighterPath(void)
{
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene)
    {
        slg::Scene  *scene = session->renderConfig->scene;
        if(scene->envLight && scene->envLight->GetType () == slg::TYPE_IL)
        {
            return ExtraSettingManager::getImageMapPath(scene->imgMapCache,dynamic_cast<slg::InfiniteLight*>(scene->envLight)->GetImageMap());
        }
    }
    throw std::runtime_error("invlaid image map");
}


slg::Scene*
DocSetting::getScene(void)
{
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->film)
    {
        return session->renderConfig->scene;
    }
    return NULL;
}

bool
DocSetting::changeSkyEnv(void)
{
    bool    bSetOK = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene)
    {
        slg::Scene  *scene = session->renderConfig->scene;

        session->Stop();
        if(scene->envLight)
        {
            BOOST_ASSERT_MSG(scene->envLight->GetType () != slg::TYPE_IL_SKY,"change to same type?");
            delete scene->envLight;
            scene->envLight = NULL;
        }
        luxrays::Properties prop;
        prop.SetString("scene.skylight.dir","0.0 0.0 1.0");
        scene->AddSkyLight(prop);
        session->Start();
        bSetOK = true;
    }
    return bSetOK;
}

bool
DocSetting::enableSun(void)
{
    bool    bSetOK = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene)
    {
        slg::Scene  *scene = session->renderConfig->scene;
        if(!scene->sunLight)
        {
            //SlgUtil::Editor editor(session);
            session->Stop();
            luxrays::Properties prop;
            prop.SetString("scene.sunlight.dir","0.0 0.0 1.0");
            scene->AddSunLight(prop);
            //editor.addAction(slg::SUNLIGHT_EDIT);
            session->Start();
            bSetOK = true;
        }
    }
    return bSetOK;
}

bool
DocSetting::disableSun(void)
{
    bool    bSetOK = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene)
    {
        slg::Scene  *scene = session->renderConfig->scene;
        if(scene->sunLight)
        {
            session->Stop();
            delete scene->sunLight;
            scene->sunLight = NULL;
            session->Start();
            bSetOK = true;
        }
    }
    return bSetOK;
}

bool
DocSetting::disableEnv(void)
{
    bool    bSetOK = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene)
    {
        slg::Scene  *scene = session->renderConfig->scene;
        if(scene->envLight)
        {
            session->Stop();
            delete scene->envLight;
            scene->envLight = NULL;
            session->Start();
            bSetOK = true;
        }
    }
    return bSetOK;
}

bool
DocSetting::changeSunTurbidity(float t)
{
    bool    bSetOK = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene->sunLight)
    {
        SlgUtil::Editor editor(session);
        session->renderConfig->scene->sunLight->SetTurbidity(t);
        editor.addAction(slg::SUNLIGHT_EDIT);
        return true;
    }
    return false;
}

bool
DocSetting::changeSunRelsize(float s)
{
    bool    bSetOK = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene->sunLight)
    {
        SlgUtil::Editor editor(session);
        session->renderConfig->scene->sunLight->SetRelSize(s);
        editor.addAction(slg::SUNLIGHT_EDIT);
        return true;
    }
    return false;
}

bool
DocSetting::changeSunGain(const luxrays::Spectrum &g)
{
    bool    bSetOK = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene->sunLight)
    {
        SlgUtil::Editor editor(session);
        session->renderConfig->scene->sunLight->SetGain(g);
        editor.addAction(slg::SUNLIGHT_EDIT);
        return true;
    }
    return false;
}

bool
DocSetting::changeSunDir(const luxrays::Vector &dir)
{
    bool    bSetOK = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene->sunLight)
    {
        SlgUtil::Editor editor(session);
        session->renderConfig->scene->sunLight->SetDir(dir);
        editor.addAction(slg::SUNLIGHT_EDIT);
        return true;
    }
    return false;
}


bool
DocSetting::changeSkyDir(const luxrays::Vector &dir)
{
    bool    bSetOK = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene->envLight)
    {
        slg::SkyLight  *pRealLight = dynamic_cast<slg::SkyLight *>(session->renderConfig->scene->envLight);
        if(pRealLight)
        {
            SlgUtil::Editor editor(session);
            pRealLight->SetSunDir(dir);
            editor.addAction(slg::SKYLIGHT_EDIT);
            return true;
        }
    }
    return false;
}

bool
DocSetting::changeSkyTurbidity(float t)
{
    bool    bSetOK = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene->envLight)
    {
        slg::SkyLight  *pRealLight = dynamic_cast<slg::SkyLight *>(session->renderConfig->scene->envLight);
        if(pRealLight)
        {
            SlgUtil::Editor editor(session);
            pRealLight->SetTurbidity(t);
            editor.addAction(slg::SKYLIGHT_EDIT);
            return true;
        }
    }
    return false;
}

bool
DocSetting::changeEnvUDelta(float d)
{
    bool    bSetOK = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene->envLight)
    {
        slg::InfiniteLight *pRealLight = dynamic_cast<slg::InfiniteLight*>(session->renderConfig->scene->envLight);
        if(pRealLight)
        {
            SlgUtil::Editor editor(session);
            pRealLight->GetUVMapping()->uDelta = d;
            editor.addAction(slg::INFINITELIGHT_EDIT);
            return true;
        }
    }
    return false;
}

bool
DocSetting::changeEnvVDelta(float d)
{
    bool    bSetOK = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene->envLight)
    {
        slg::InfiniteLight *pRealLight = dynamic_cast<slg::InfiniteLight*>(session->renderConfig->scene->envLight);
        if(pRealLight)
        {
            SlgUtil::Editor editor(session);
            pRealLight->GetUVMapping()->vDelta = d;
            editor.addAction(slg::INFINITELIGHT_EDIT);
            return true;
        }
    }
    return false;
}

bool
DocSetting::changeEnvGain(const luxrays::Spectrum &g)
{
    bool    bSetOK = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene->envLight)
    {
        SlgUtil::Editor editor(session);
        session->renderConfig->scene->envLight->SetGain(g);
        if(session->renderConfig->scene->envLight->GetType() == slg::TYPE_IL)
        {
            editor.addAction(slg::INFINITELIGHT_EDIT);
        }else{
            editor.addAction(slg::SKYLIGHT_EDIT);
        }
        return true;
    }
    return false;
}


bool
DocSetting::changeHDRfile(const std::string &fullpath)
{
    bool    bSetOK = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene)
    {
        slg::Scene  *scene = session->renderConfig->scene;
        if(scene->envLight)
        {
            luxrays::Properties prop;
            if(scene->envLight->GetType () == slg::TYPE_IL)
            {
                luxrays::Properties prop = scene->envLight->ToProperties(scene->imgMapCache);
            }
            prop.SetString("scene.infinitelight.file",fullpath);

            //SlgUtil::Editor editor(session);
            session->Stop();
            delete scene->envLight;
            scene->envLight = NULL;
            scene->AddInfiniteLight(prop);
            session->Start();
            //editor.addAction(slg::INFINITELIGHT_EDIT);
            bSetOK = true;
        }
    }
    return bSetOK;
}



// get toneMap properties
type_xml_node*
DocSetting::getToneMap(type_xml_node &parent)
{
	type_xml_doc *pDoc = parent.document();
    BOOST_ASSERT_MSG(pDoc != NULL,"invalid node,must from doc");

	slg::RenderSession* session = pDocData->getSession();
    if(session && session->film)
    {
		slg::Film *film = session->film;
        type_xml_node * pSelf = pDoc->allocate_node(NS_RAPIDXML::node_element,"tonemap");
		parent.append_node(pSelf);

		int type = film->GetToneMapParams()->GetType();
		std::string typeName = getToneMapTypeNameByName(type);
		pSelf->append_attribute(pDoc->allocate_attribute(constDef::type,allocate_string(pDoc,typeName)));
		type_xml_node *pParams = pDoc->allocate_node(NS_RAPIDXML::node_element,allocate_string(pSelf->document(),typeName));
		pSelf->append_node(pParams);
		if(typeName == "linear")
		{
			slg::LinearToneMapParams *params = (slg::LinearToneMapParams *)film->GetToneMapParams()->Copy();
			pParams->append_attribute(allocate_attribute_withkey(pDoc,"scale",boost::lexical_cast<std::string>( params->scale )));
			delete params;
		}
		else if(typeName == "reinhard02")
		{
			slg::Reinhard02ToneMapParams *params = (slg::Reinhard02ToneMapParams *)film->GetToneMapParams()->Copy();
			pParams->append_attribute(allocate_attribute_withkey(pDoc,"burn",boost::lexical_cast<std::string>( params->burn )));
			pParams->append_attribute(allocate_attribute_withkey(pDoc,"postScale",boost::lexical_cast<std::string>( params->postScale )));
			pParams->append_attribute(allocate_attribute_withkey(pDoc,"preScale",boost::lexical_cast<std::string>( params->preScale )));
			delete params;
		}
		return pSelf;
	}
	return NULL;
}

std::string
DocSetting::getToneMapTypeNameByName(int type)
{
	std::string typeName;
	switch(type)
	{
	case slg::TONEMAP_NONE:
		typeName = "none";
		break;
	case slg::TONEMAP_LINEAR:
		typeName = "linear";
		break;
	case slg::TONEMAP_REINHARD02:
		typeName = "reinhard02";
		break;
	default:
		break;
	}
	return typeName;
}

bool
DocSetting::getLinearScale(float &ls)
{
	bool ret = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->film)
    {
		slg::Film *film = session->film;
		if(film->GetToneMapParams()->GetType() == slg::TONEMAP_LINEAR)
		{
			slg::LinearToneMapParams *params = (slg::LinearToneMapParams *)film->GetToneMapParams()->Copy();
			ls = params->scale;
			delete params;
			ret = true;
		}
	}
    return ret;
}

bool
DocSetting::setLinearScale(float ls)
{
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->film)
    {
		slg::Film *film = session->film;
		slg::LinearToneMapParams *params = (slg::LinearToneMapParams *)film->GetToneMapParams()->Copy();
		params->scale = ls;
		film->SetToneMapParams(*params);
		delete params;
		return true;
	}
    return false;
}

}
