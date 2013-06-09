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
#include "../cmdids.h"
#include "../filedialog.h"


BEGIN_EVENT_TABLE(gme::EnvLightPage, gme::EnvLightPage::inherit)
    EVT_PG_CHANGING( wxID_ANY, gme::EnvLightPage::OnPropertyChanging )
    EVT_PG_CHANGED( wxID_ANY, gme::EnvLightPage::OnPropertyChange )
#ifdef PROPERTY_HAS_DELETE_PROPERTY
#else
    EVT_BUTTON (cmd::GID_REFRESH_MATPROP, EnvLightPage::OnRefreshPage)
#endif
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
        }
    }else if(type == DISABLE_LIGHTER)
    {
        if(setting.disableEnv())
        {//refresh childs.
            bChangeOk = true;
        }
    }else{
        BOOST_ASSERT_MSG(type == slg::TYPE_IL_SKY,"new type of env light?");
        if(setting.changeSkyEnv())
        {//refresh childs.
            bChangeOk = true;
        }
    }

    if(bChangeOk)
    {
#ifdef PROPERTY_HAS_DELETE_PROPERTY
        //refresh childs.
        this->removeChild(p);
        appendEnvLight(p,setting);
#else
        wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED, cmd::GID_REFRESH_MATPROP);
        wxPostEvent(this,evt);
#endif
    }
    return bChangeOk;
}

#ifdef PROPERTY_HAS_DELETE_PROPERTY
#else
void
EnvLightPage::OnRefreshPage(wxCommandEvent &event)
{
    wxPGProperty* id = this->m_manager->GetGrid()->GetSelection();
    if(id && this->m_manager->IsPropertyEnabled(id) )
    {
        this->m_manager->DisableProperty ( id );
        wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED, cmd::GID_REFRESH_MATPROP);
        wxPostEvent(this,evt);
    }else
    {
        this->m_manager->DeletePendingEvents();
        this->DeletePendingEvents();
        clearPage();
        buildPage();
    }
}
#endif



luxrays::Spectrum
EnvLightPage::getSepctrum(bool bSync,const std::string &prefix,float gself,const char* ored,const char* ogreen,const char* oblue)
{
    luxrays::Spectrum spectrum;
    if(bSync)
    {
        spectrum.r = spectrum.g = spectrum.b = gself;
    }
    else{
        wxPGProperty *prop;
        if(ored)
        {
            prop = this->GetProperty(prefix + ored);
            spectrum.r = (float)(double)prop->GetValue();
        }else{
            spectrum.r = gself;
        }

        if(ogreen)
        {
            prop = this->GetProperty(prefix + ogreen);
            spectrum.g = (float)(double)prop->GetValue();
        }else{
            spectrum.g = gself;
        }

        if(oblue)
        {
            prop = this->GetProperty(prefix + oblue);
            spectrum.b = (float)(double)prop->GetValue();
        }else{
            spectrum.b = gself;
        }
    }
    return spectrum;
}

int
EnvLightPage::getTypeFromId(const std::string &id)
{
    if(boost::starts_with(id,"type.gain."))
    {
        return slg::TYPE_IL;
    }else if(boost::starts_with(id,"type.skygain."))
    {
        return slg::TYPE_IL_SKY;
    }else if(boost::starts_with(id,"enable.sungain."))
    {
        return slg::TYPE_SUN;
    }
    throw std::runtime_error("invalid type.");
}

void
EnvLightPage::updateGainProperty(bool bSync,const std::string &prefix,float gself,const char* ored,const char* ogreen,const char* oblue)
{
    if(bSync)
    {
        wxPGProperty *prop;
        if(ored)
        {
            prop = this->GetProperty(prefix + ored);
            prop->SetValue(gself);
        }

        if(ogreen)
        {
            prop = this->GetProperty(prefix + ogreen);
            prop->SetValue(gself);
        }

        if(oblue)
        {
            prop = this->GetProperty(prefix + oblue);
            prop->SetValue(gself);
        }
    }
}

void
EnvLightPage::changeGainValue(wxPropertyGridEvent& event,int type,luxrays::Spectrum &spectrum,const std::string &prefix,float gself,const char* ored,const char* ogreen,const char* oblue)
{
    switch(type)
    {
    case slg::TYPE_IL:
        {
            DocSetting  setting;
            if(!setting.changeEnvGain(spectrum))
            {
                if(event.CanVeto())
                    event.Veto();
            }else{
                updateGainProperty(this->m_gainSync,prefix,gself,ored,ogreen,oblue);
            }
        }
        break;
    case slg::TYPE_IL_SKY:
        {
            DocSetting  setting;
            if(!setting.changeEnvGain(spectrum))
            {
                if(event.CanVeto())
                    event.Veto();
            }else{
                updateGainProperty(this->m_skygainSync,prefix,gself,ored,ogreen,oblue);
            }
        }
        break;
    case slg::TYPE_SUN:
        {
            DocSetting  setting;
            if(!setting.changeSunGain(spectrum))
            {
                if(event.CanVeto())
                    event.Veto();
            }else{
                updateGainProperty(this->m_sungainSync,prefix,gself,ored,ogreen,oblue);
            }
        }
        break;
    default:
        BOOST_ASSERT(false);
    }
}

void
EnvLightPage::OnPropertyChanging( wxPropertyGridEvent& event )
{
    wxBusyCursor wait;
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
//        std::cerr << "change type to " << event.GetValue().GetString().c_str() << std::endl;
    }else if(boost::equals(id,"type.file"))
    {
        std::string     filepath(event.GetValue().GetString().c_str());
        //检查以file结束，并检查其是一个被支持的图像格式。
        if(!OpenImageDialog::isSupported(filepath))
        {//图片不被支持，直接退出。
            if(event.CanVeto())
                event.Veto();
            return;
        }

        {
            DocSetting  setting;
            if(!setting.changeHDRfile(filepath) && event.CanVeto())
            {
                event.Veto();
            }
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
#ifdef PROPERTY_HAS_DELETE_PROPERTY
            this->removeChild(p);
            this->appendSunLight(p,setting);
#else
            wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED, cmd::GID_REFRESH_MATPROP);
            wxPostEvent(this,evt);
#endif
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
    }else if(boost::starts_with(id,"type.gain") || boost::starts_with(id,"type.skygain") || boost::starts_with(id,"enable.sungain") )
    {
        if(boost::equals(id,"type.gain"))
        {
            m_gainSync = any_value.As<bool>();
        }else if(boost::equals(id,"type.skygain"))
        {
            m_skygainSync = any_value.As<bool>();
        }else if(boost::equals(id,"enable.sungain"))
        {
            m_sungainSync = any_value.As<bool>();
        }else
        {
            std::string   prefix = id.substr(0,id.length() - 2);
            float   gself = any_value.As<float>();
            const char* ored = "gr";
            const char* ogreen = "gg";
            const char* oblue = "gb";
            if(boost::ends_with(id,"gr"))
            {
                ored = NULL;
            }else if(boost::ends_with(id,"gg"))
            {
                ogreen = NULL;
            }else if(boost::ends_with(id,"gb"))
            {
                oblue = NULL;
            }else{
                BOOST_ASSERT(false);
            }

            int type = getTypeFromId(id);
            bool bSync;
            switch(type)
            {
            case slg::TYPE_IL:
                bSync = this->m_gainSync;
                break;
            case slg::TYPE_IL_SKY:
                bSync = this->m_skygainSync;
                break;
            case slg::TYPE_SUN:
                bSync = this->m_sungainSync;
                break;
            default:
                BOOST_ASSERT(false);
            }

            luxrays::Spectrum spectrum = getSepctrum(bSync,prefix,gself,ored,ogreen,oblue);
            changeGainValue(event,type,spectrum,prefix,gself,ored,ogreen,oblue);
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

    gme::MainFrame* mainfrm = dynamic_cast<gme::MainFrame*>(wxTheApp->GetTopWindow());
    if(mainfrm)
    {
        mainfrm->refreshMouseEvt();
    }
}

void
EnvLightPage::OnPropertyChange( wxPropertyGridEvent& event )
{
}

EnvLightPage::EnvLightPage()
{
    m_gainSync = true;
    m_skygainSync = true;
    m_sungainSync = true;
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
///  @biref 方向在主视图上直观调整。
//    luxrays::Vector v = scene->sunLight->GetDir();
//    v = v / v.Length();
//    wxColour    color(255,255,255);
//    color.Set(v.x * 255.0f,v.y * 255.0f,v.z * 255.0f);
//    this->AppendIn(pEnable,new wxColourProperty(gmeWXT("方向"),"sundir",color));
    //turbidity
    this->AppendIn(pEnable,this->setFloatEditor(new wxFloatProperty(gmeWXT("turbidity"),"sunturb",scene->sunLight->GetTubidity()),0.0f,10.0f ) );
    //relsize
    this->AppendIn(pEnable,this->setFloatEditor(new wxFloatProperty(gmeWXT("relsize"),"relsize",scene->sunLight->GetRelSize()) , 0.0f, 10.0f ) );
    //sungain
    luxrays::Spectrum c = scene->sunLight->GetGain();
    appendGain(pEnable,c,"sungain",this->m_sungainSync,"sun");
}

void
EnvLightPage::appendGain(wxPGProperty* parent,luxrays::Spectrum &value,const std::string &id,bool curSyncValue,const std::string &subprefix)
{
    DECLARE_WXCONVERT;
    wxPGProperty *pGain = new wxBoolProperty(gmeWXT("增益"),id,curSyncValue);
    this->AppendIn(parent,pGain);
    this->AppendIn(pGain,this->setFloatEditor(new wxFloatProperty(gmeWXT("红"),subprefix + "gr",value.r)));
    this->AppendIn(pGain,this->setFloatEditor(new wxFloatProperty(gmeWXT("绿"),subprefix + "gg",value.g)));
    this->AppendIn(pGain,this->setFloatEditor(new wxFloatProperty(gmeWXT("蓝"),subprefix + "gb",value.b)));
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
        luxrays::Spectrum c = pRealLight->GetGain();

        appendGain(pType,c,"gain",this->m_gainSync,"");

        this->AppendIn(pType,this->setFloatEditor(new wxFloatProperty(gmeWXT("uDelta"),"udelta",pRealLight->GetUVMapping()->uDelta)));
        this->AppendIn(pType,this->setFloatEditor(new wxFloatProperty(gmeWXT("vDelta"),"vdelta",pRealLight->GetUVMapping()->vDelta)));
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
        appendGain(pType,c,"skygain",this->m_skygainSync,"sg");

        this->AppendIn(pType,this->setFloatEditor(new wxFloatProperty(gmeWXT("turbidity"),"turbidity",pRealLight->GetTubidity()),0.0f,10.0f));
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



