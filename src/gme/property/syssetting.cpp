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
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "slg/slg.h"
#include "slg/sampler/sampler.h"
#include "slg/renderengine.h"
#include "utils/i18n.h"
#include "utils/option.h"
#include "dm/docio.h"
#include "dm/docsetting.h"
#include "dm/setting.h"
#include "syssetting.h"
#include "../stringutil.h"
#include "../cmdids.h"
#include "../mainframe.h"


BEGIN_EVENT_TABLE(gme::SysPage, gme::SysPage::inherit)
    EVT_PG_CHANGING( wxID_ANY, gme::SysPage::OnPropertyChanging )
#ifdef PROPERTY_HAS_DELETE_PROPERTY
#else
    EVT_BUTTON (cmd::GID_REFRESH_MATPROP, gme::SysPage::OnRefreshPage)
#endif
END_EVENT_TABLE()


namespace gme{


void
SysPage::onDocumentLoaded(void)
{
    clearPage();
    buildPage();
}

void
SysPage::clearPage(void)
{
    m_bInClearStage = true;
    //清空当前页面。
    this->Clear();
    m_bInClearStage = false;
}

void
SysPage::buildPage(void)
{
    m_bInClearStage = false;

    DECLARE_WXCONVERT;

    wxPGProperty* pCate = this->Append(new wxPropertyCategory(gmeWXT("系统设置"),"sys"));

    {
        DocSetting  setting;
        {//sampler
            wxPGChoices soc;
            soc.Add( gmeWXT("随机"),slg::RANDOM);
            soc.Add( gmeWXT("蒙特卡罗"),slg::METROPOLIS);
            soc.Add( gmeWXT("索博尔"),slg::SOBOL);
            wxPGProperty* pSampler = new wxEnumProperty(gmeWXT("采样方式"),"sampler", soc);
            int samplerType = setting.getSamplerType();
            this->SetPropertyValue(pSampler,samplerType);
            this->AppendIn(pCate,pSampler);

            if(samplerType == slg::METROPOLIS)
            {
                this->AppendIn(pSampler,this->setFloatEditor(new wxFloatProperty(gmeWXT("最大变化"),"largesteprate",setting.getLargesteprate()),0.001f,1.0f,10000,4) );
                this->AppendIn(pSampler,this->setFloatEditor(new wxFloatProperty(gmeWXT("变化范围"),"imagemutationrate",setting.getImagemutationrate()),0.001f,1.0f,10000,4) );
                this->AppendIn(pSampler,this->setFloatEditor(new wxFloatProperty(gmeWXT("最大拒绝数"),"maxconsecutivereject",setting.getMaxconsecutivereject()),1.0f,1024.0f,1,0) );
            }
        }

        {//engine type.
            wxPGChoices soc;
            soc.Add( gmeWXT("正向"),slg::LIGHTCPU);
            if(clHardwareInfo::instance().getPlatforms().size() == 0)
            {//no opencl found. rollback to cpu mode.
                soc.Add( gmeWXT("逆向"),slg::PATHCPU);
            }else{
                soc.Add( gmeWXT("逆向"),slg::PATHOCL);
            }
            soc.Add( gmeWXT("双向"),slg::BIDIRHYBRID);
            soc.Add( gmeWXT("组合双向"),slg::CBIDIRHYBRID);
            soc.Add( gmeWXT("压缩双向"),slg::BIDIRVMCPU);
            wxPGProperty* pEngine = new wxEnumProperty(gmeWXT("渲染方式"),"engine", soc);
            this->SetPropertyValue(pEngine,setting.getEngineType());
            this->AppendIn(pCate,pEngine);

            this->AppendIn(pCate,this->setFloatEditor(new wxFloatProperty(gmeWXT("反弹次数"),"pathdepth",setting.getPathDepth()),2.0f,64.0f,1,0) );
            this->AppendIn(pCate,this->setFloatEditor(new wxFloatProperty(gmeWXT("轮盘赌次数"),"roulettedepth",setting.getRouletteDepth()),2.0f,64.0f,1,0) );
            this->AppendIn(pCate,this->setFloatEditor(new wxFloatProperty(gmeWXT("轮盘赌系数"),"roulettecap",setting.getRouletteCap()),0.0f,1.0f,10000,4) );
        }

        {//filter type.
            wxPGChoices soc;
            soc.Add( gmeWXT("无过滤"),slg::ocl::FILTER_NONE);
            soc.Add( gmeWXT("矩形过滤"),slg::ocl::FILTER_BOX);
            soc.Add( gmeWXT("高斯过滤"),slg::ocl::FILTER_GAUSSIAN);
            soc.Add( gmeWXT("米切尔过滤"),slg::ocl::FILTER_MITCHELL);


            int   type = setting.getPathFilterType();
            wxPGProperty* pFilter = new wxEnumProperty(gmeWXT("过滤方式"),wxString(constDef::filter), soc,type);
            this->AppendIn(pCate,pFilter);
            if(type != slg::ocl::FILTER_NONE)
            {
                this->AppendIn(pFilter,this->setFloatEditor(new wxFloatProperty(gmeWXT("X步进"),"widthx",setting.getPFWidthX()),0.001f,1.5f,10000,4) );
                this->AppendIn(pFilter,this->setFloatEditor(new wxFloatProperty(gmeWXT("Y步进"),"widthy",setting.getPFWidthY()),0.001f,1.5f,10000,4) );

                switch(type)
                {
                case slg::ocl::FILTER_GAUSSIAN:
                    this->AppendIn(pFilter,this->setFloatEditor(new wxFloatProperty(gmeWXT("α系数"),constDef::alpha,setting.getPFAlpha()),0.0f,4.0f,10000,4) );
                    break;
                case slg::ocl::FILTER_MITCHELL:
                    this->AppendIn(pFilter,this->setFloatEditor(new wxFloatProperty(gmeWXT("B系数"),"b",setting.getPFilterB()),0.0f,1.0f,10000,4) );
                    this->AppendIn(pFilter,this->setFloatEditor(new wxFloatProperty(gmeWXT("C系数"),"c",setting.getPFilterC()),0.0f,1.0f,10000,4) );
                    break;
                }
            }
        }
    }
}


void
SysPage::onDocumentClosed(void)
{
    clearPage();
}

SysPage::SysPage()
{
    DocIO   dio;
    dio.onSceneLoaded(boost::bind(&SysPage::onDocumentLoaded,this));
    dio.onSceneClosed(boost::bind(&SysPage::onDocumentClosed,this));
}

SysPage::~SysPage()
{
}

#ifdef PROPERTY_HAS_DELETE_PROPERTY
#else
void
SysPage::OnRefreshPage(wxCommandEvent &event)
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


void SysPage::OnPropertyChanging( wxPropertyGridEvent& event )
{
    if(m_bInClearStage)
        return;
    wxBusyCursor wait;

    wxPGProperty* p = event.GetProperty();


    std::string     id(p->GetName().c_str());

    if(boost::equals(id,"sampler"))
    {
        wxAny any_value = event.GetValue();
        BOOST_ASSERT(any_value.CheckType<int>());
        int type = any_value.As<int>();

        DocSetting  setting;
        if(!setting.setSamplerType(type) && event.CanVeto())
        {
            event.Veto();
        }else{
#ifdef PROPERTY_HAS_DELETE_PROPERTY
        //refresh childs.
#error  "NOT IMPLEMENT."
#else
            wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED, cmd::GID_REFRESH_MATPROP);
            wxPostEvent(this,evt);
#endif
        }
    }else if(boost::equals(id,"engine"))
    {
        wxAny any_value = event.GetValue();
        BOOST_ASSERT(any_value.CheckType<int>());
        int type = any_value.As<int>();

        DocSetting  setting;
        if(!setting.setEngineType(type) && event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,"pathdepth"))
    {
        wxAny any_value = event.GetValue();
        float value = any_value.As<float>();
        DocSetting setting;
        if(!setting.setPathDepth((int)value) && event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,"roulettecap"))
    {
        wxAny any_value = event.GetValue();
        float value = any_value.As<float>();
        DocSetting setting;
        if(!setting.setRouletteCap(value) && event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,"roulettedepth"))
    {
        wxAny any_value = event.GetValue();
        float value = any_value.As<float>();
        DocSetting setting;
        if(!setting.setRouletteDepth((int)value) && event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,"sampler.largesteprate"))
    {
        wxAny any_value = event.GetValue();
        float value = any_value.As<float>();
        DocSetting setting;
        setting.setLargesteprate(value);
    }else if(boost::equals(id,"sampler.imagemutationrate"))
    {
        wxAny any_value = event.GetValue();
        float value = any_value.As<float>();
        DocSetting setting;
        setting.setImagemutationrate(value);
    }else if(boost::equals(id,"sampler.maxconsecutivereject"))
    {
        wxAny any_value = event.GetValue();
        float value = any_value.As<float>();
        DocSetting setting;
        setting.setMaxconsecutivereject(value);
    }else if(boost::equals(id,"filter"))
    {
        wxAny any_value = event.GetValue();
        BOOST_ASSERT(any_value.CheckType<int>());
        int type = any_value.As<int>();

        DocSetting  setting;
        if(!setting.setPathFilterType(type) && event.CanVeto())
        {
            event.Veto();
        }else{
#ifdef PROPERTY_HAS_DELETE_PROPERTY
        //refresh childs.
#error  "NOT IMPLEMENT."
#else
            wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED, cmd::GID_REFRESH_MATPROP);
            wxPostEvent(this,evt);
#endif
        }
    }else if(boost::equals(id,"filter.widthx"))
    {
        wxAny any_value = event.GetValue();
        float value = any_value.As<float>();
        DocSetting setting;

        if(!setting.setPFWidthX(value) && event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,"filter.widthy"))
    {
        wxAny any_value = event.GetValue();
        float value = any_value.As<float>();
        DocSetting setting;

        if(!setting.setPFWidthY(value) && event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,"filter.alpha"))
    {
        wxAny any_value = event.GetValue();
        float value = any_value.As<float>();
        DocSetting setting;

        if(!setting.setPFAlpha(value) && event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,"filter.b"))
    {
        wxAny any_value = event.GetValue();
        float value = any_value.As<float>();
        DocSetting setting;

        if(!setting.setPFilterB(value) && event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,"filter.c"))
    {
        wxAny any_value = event.GetValue();
        float value = any_value.As<float>();
        DocSetting setting;

        if(!setting.setPFilterC(value) && event.CanVeto())
        {
            event.Veto();
        }
    }
}


}

