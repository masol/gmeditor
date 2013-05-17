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
#include "envlightpage.h"
#include "dm/docio.h"
#include "dm/docsetting.h"
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/format.hpp>
#include <wx/propgrid/advprops.h>
#include "../stringutil.h"
#include "../mainframe.h"
#include "luxrays/utils/properties.h"
#include "slg/slg.h"
#include "slg/sdl/scene.h"


BEGIN_EVENT_TABLE(gme::EnvLightPage, gme::EnvLightPage::inherit)
    EVT_PG_CHANGING( wxID_ANY, gme::EnvLightPage::OnPropertyChanging )
    EVT_PG_CHANGED( wxID_ANY, gme::EnvLightPage::OnPropertyChange )
END_EVENT_TABLE()


namespace gme{


bool
EnvLightPage::OnEnvtypeChanged(wxPGProperty* p,unsigned int type)
{
    bool    bChangeOk = false;
    DocSetting   setting;
    if(type == slg::TYPE_IL)
    {//
        std::string imagePath;
        if(MainFrame::getImageFilepathFunc()(imagePath))
        {
            bChangeOk = setting.changeHDRfile(imagePath);
            //refresh childs.
            this->removeChild(p);
            appendEnvLight(p,setting);
        }
    }else if(type == DISABLE_LIGHTER)
    {
        if(setting.disableEnv())
        {//refresh childs.
            bChangeOk = true;
            this->removeChild(p);
            appendEnvLight(p,setting);
        }
    }else{
        BOOST_ASSERT_MSG(type == slg::TYPE_IL_SKY,"new type of env light?");
        if(setting.changeSkyEnv())
        {//refresh childs.
            bChangeOk = true;
            this->removeChild(p);
            appendEnvLight(p,setting);
        }
    }
    return bChangeOk;
}

void
EnvLightPage::OnPropertyChanging( wxPropertyGridEvent& event )
{
    wxPGProperty* p = event.GetProperty();
    std::string     id(p->GetName().c_str());
    wxAny any_value = event.GetValue();
    if(boost::equals(id,constDef::type))
    {//环境光类型被选择。
        BOOST_ASSERT(any_value.CheckType<int>());
        int type = any_value.As<int>();

        if(!OnEnvtypeChanged(p,(unsigned int)type) && event.CanVeto())
        {
            event.Veto();
        }

        std::cerr << "change type to " << event.GetValue().GetString().c_str() << std::endl;
    }else if(boost::equals(id,"type.file"))
    {
        DocSetting  setting;
        std::string     filepath(event.GetValue().GetString().c_str());
        if(!setting.changeHDRfile(filepath) && event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,"type.gain") || boost::equals(id,"type.skygain"))
    {
        DocSetting  setting;
        BOOST_ASSERT(any_value.CheckType<wxColour>());
        wxColour color = any_value.As<wxColour>();
        luxrays::Spectrum g;
        g.r = (float)color.Red() / 255.0f;
        g.g = (float)color.Green() / 255.0f;
        g.b = (float)color.Blue() / 255.0f;
        if(!setting.changeEnvGain(g) && event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,"type.udelta"))
    {
        DocSetting  setting;
        float delta = any_value.As<float>();
        if(!setting.changeEnvUDelta(delta) && event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,"type.vdelta"))
    {
        DocSetting  setting;
        float delta = any_value.As<float>();
        if(!setting.changeEnvVDelta(delta) && event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,"type.dir"))
    {
        DocSetting  setting;
        BOOST_ASSERT(any_value.CheckType<wxColour>());
        wxColour color = any_value.As<wxColour>();
        luxrays::Vector v;
        v.x = (float)color.Red() / 255.0f;
        v.y = (float)color.Green() / 255.0f;
        v.z = (float)color.Blue() / 255.0f;
        if(!setting.changeSkyDir(v) && event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,"type.turbidity"))
    {
        DocSetting  setting;
        BOOST_ASSERT(any_value.CheckType<float>());
        float t = any_value.As<float>();
        if(!setting.changeSkyTurbidity(t) && event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,"enable"))
    {
        DocSetting  setting;
        BOOST_ASSERT(any_value.CheckType<bool>());
        bool enable = any_value.As<bool>();
        bool bSetOk = false;
        if(enable)
            bSetOk = setting.enableSun();
        else
            bSetOk = setting.disableSun();

        if(bSetOk)
        {
            this->removeChild(p);
            this->appendSunLight(p,setting);
        }else if(event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,"enable.sundir"))
    {
        DocSetting  setting;
        BOOST_ASSERT(any_value.CheckType<wxColour>());
        wxColour color = any_value.As<wxColour>();
        luxrays::Vector v;
        v.x = (float)color.Red() / 255.0f;
        v.y = (float)color.Green() / 255.0f;
        v.z = (float)color.Blue() / 255.0f;
        if(!setting.changeSunDir(v) && event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,"enable.sungain"))
    {
        DocSetting  setting;
        BOOST_ASSERT(any_value.CheckType<wxColour>());
        wxColour color = any_value.As<wxColour>();
        luxrays::Spectrum g;
        g.r = (float)color.Red() / 255.0f;
        g.g = (float)color.Green() / 255.0f;
        g.b = (float)color.Blue() / 255.0f;
        if(!setting.changeSunGain(g) && event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,"enable.sunturb"))
    {
        DocSetting  setting;
        BOOST_ASSERT(any_value.CheckType<float>());
        float t = any_value.As<float>();
        if(!setting.changeSunTurbidity(t) && event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,"enable.relsize"))
    {
        DocSetting  setting;
        BOOST_ASSERT(any_value.CheckType<float>());
        float t = any_value.As<float>();
        if(!setting.changeSunRelsize(t) && event.CanVeto())
        {
            event.Veto();
        }
    }else{
        BOOST_ASSERT_MSG(false,"unknow property!");
    }
//    std::cerr << "EnvLightPage::OnPropertyChanging('" << p->GetName().c_str() << "', to value '" << event.GetValue().GetString().c_str() << "')" << std::endl;
}

void
EnvLightPage::OnPropertyChange( wxPropertyGridEvent& event )
{
}

EnvLightPage::EnvLightPage()
{
    DocIO   dio;
    dio.onSceneClosed(boost::bind(&EnvLightPage::onDocumentClosed,this));
    dio.onSceneLoaded(boost::bind(&EnvLightPage::onDocumentOpend,this));
}


EnvLightPage::~EnvLightPage()
{
}

void
EnvLightPage::onDocumentClosed(void)
{
    clearPage();
}


void
EnvLightPage::onDocumentOpend(void)
{
    clearPage();
    buildPage();
}

void
EnvLightPage::clearPage(void)
{
    //清空当前页面。
    this->Clear();
}

void
EnvLightPage::appendSunLight(wxPGProperty* pEnable,DocSetting &setting)
{
    DECLARE_WXCONVERT;
    slg::Scene *scene = setting.getScene();

    this->RefreshProperty(pEnable);
    if(!scene->sunLight) //sunlight not enabled!
        return;

    luxrays::Vector v = scene->sunLight->GetDir();
    v = v / v.Length();
    wxColour    color(255,255,255);
    color.Set(v.x * 255.0f,v.y * 255.0f,v.z * 255.0f);
    this->AppendIn(pEnable,new wxColourProperty(gmeWXT("方向"),"sundir",color));
    //turbidity
    this->AppendIn(pEnable,new wxFloatProperty(gmeWXT("turbidity"),"sunturb",scene->sunLight->GetTubidity()));
    //relsize
    this->AppendIn(pEnable,new wxFloatProperty(gmeWXT("relsize"),"relsize",scene->sunLight->GetRelSize()));
    //sungain
    luxrays::Spectrum c = scene->sunLight->GetGain();
    color.Set(c.r * 255.0f,c.g * 255.0f,c.b * 255.0f);
    this->AppendIn(pEnable,new wxColourProperty(gmeWXT("增益"),"sungain",color));
}

void
EnvLightPage::appendEnvLight(wxPGProperty* pType,DocSetting &setting)
{
    DECLARE_WXCONVERT;
    slg::Scene *scene = setting.getScene();
    this->RefreshProperty(pType);
    if(!scene->envLight) //disable option.
        return;
    switch(scene->envLight->GetType())
    {
    case slg::TYPE_IL:
    {
        slg::InfiniteLight *pRealLight = dynamic_cast<slg::InfiniteLight*>(scene->envLight);
        wxString  filename(setting.getHDRLighterPath().c_str(),gme_wx_utf8_conv);
        this->AppendIn(pType,new wxImageFileProperty(gmeWXT("文件"),constDef::file,filename));
        wxColour    color(255,255,255);
        luxrays::Spectrum c = pRealLight->GetGain();
        color.Set(c.r * 255.0f,c.g * 255.0f,c.b * 255.0f);
        this->AppendIn(pType,new wxColourProperty(gmeWXT("增益"),"gain",color));

        this->AppendIn(pType,new wxFloatProperty(gmeWXT("uDelta"),"udelta",pRealLight->GetUVMapping()->uDelta));
        this->AppendIn(pType,new wxFloatProperty(gmeWXT("vDelta"),"vdelta",pRealLight->GetUVMapping()->vDelta));
        break;
    }
    case slg::TYPE_IL_SKY:
    {
        slg::SkyLight *pRealLight = dynamic_cast<slg::SkyLight*>(scene->envLight);

        ///@todo 不使用颜色控件，使用其它控件。
        luxrays::Vector v = pRealLight->GetSunDir();
        v = v / v.Length();
        wxColour    color(255,255,255);
        color.Set(v.x * 255.0f,v.y * 255.0f,v.z * 255.0f);
        this->AppendIn(pType,new wxColourProperty(gmeWXT("方向"),"dir",color));

        luxrays::Spectrum c = pRealLight->GetGain();
        color.Set(c.r * 255.0f,c.g * 255.0f,c.b * 255.0f);
        this->AppendIn(pType,new wxColourProperty(gmeWXT("增益"),"skygain",color));

        this->AppendIn(pType,new wxFloatProperty(gmeWXT("turbidity"),"turbidity",pRealLight->GetTubidity()));
        break;
    }
    default:
        BOOST_ASSERT_MSG(false,"new type of env light?");
    }
}

void
EnvLightPage::buildPage(void)
{
    DocSetting  setting;
    type_xml_doc    doc;
    slg::Scene *scene = setting.getScene();
    if(scene)
    {
        DECLARE_WXCONVERT;

        wxPGChoices soc;
        soc.Add( gmeWXT("HDR环境"),slg::TYPE_IL);
        soc.Add( gmeWXT("阳光"),slg::TYPE_IL_SKY);
        soc.Add( gmeWXT("disable"),DISABLE_LIGHTER);

        wxPGProperty* pCate = this->Append(new wxPropertyCategory(gmeWXT("环境光"),"light"));

        int type = DISABLE_LIGHTER;
        if(scene->envLight)
        {
            type = scene->envLight->GetType();
        }
        wxPGProperty* pType = new wxEnumProperty(gmeWXT("类型"),"type", soc);
        this->SetPropertyValue(pType,type);
        this->AppendIn(pCate,pType);

        appendEnvLight(pType,setting);

        wxPGProperty* pSunCate = this->Append(new wxPropertyCategory(gmeWXT("阳光"),"sunlight"));

        wxPGProperty *pEnable = new wxBoolProperty(gmeWXT("开启"),"enable",scene->sunLight != NULL);
        this->AppendIn(pSunCate,pEnable);
        this->appendSunLight(pEnable,setting);
    }
}



}



