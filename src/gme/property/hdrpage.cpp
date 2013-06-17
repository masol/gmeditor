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
#include "slg/film/tonemapping.h"
#include "utils/i18n.h"
#include "utils/option.h"
#include "hdrpage.h"
#include "../stringutil.h"
#include "dm/docio.h"
#include "dm/docimg.h"
#include "dm/docsetting.h"
#include "dm/doccamera.h"
#include "../cmdids.h"
#include "../mainframe.h"
#include "imgfileeditor.h"


BEGIN_EVENT_TABLE(gme::HDRPage, gme::HDRPage::inherit)
    EVT_PG_SELECTED( wxID_ANY, gme::HDRPage::OnPropertySelect )
    EVT_PG_CHANGING( wxID_ANY, gme::HDRPage::OnPropertyChanging )
    EVT_PG_CHANGED( wxID_ANY, gme::HDRPage::OnPropertyChange )
#ifdef PROPERTY_HAS_DELETE_PROPERTY
#else
    EVT_BUTTON (cmd::GID_REFRESH_MATPROP, gme::HDRPage::OnRefreshPage)
#endif
END_EVENT_TABLE()


namespace gme{


class NoneToneMapParams : public slg::ToneMapParams
{
public:
    NoneToneMapParams()
    {
    }
    slg::ToneMapType GetType() const { return slg::TONEMAP_NONE; }
    slg::ToneMapParams *Copy() const {
        return new NoneToneMapParams();
    }
};


void
HDRPage::onDocumentLoaded(void)
{
    clearPage();
    buildPage();
}

void
HDRPage::clearPage(void)
{
    //清空当前页面。
    this->Clear();
}

void
HDRPage::refreshToneMapping(int typevlaue,const slg::ToneMapParams *param,wxPGProperty *pToneMap)
{
    DECLARE_WXCONVERT;

    switch(typevlaue)
    {
    case slg::TONEMAP_LINEAR:
        {
            const slg::LinearToneMapParams *pRealParam = dynamic_cast<const slg::LinearToneMapParams*>(param);
            this->AppendIn(pToneMap,this->setFloatEditor(new wxFloatProperty(gmeWXT("缩放系数"),wxString("scale"),pRealParam->scale), 0.0f,8.0f) );
        }
        break;
    case slg::TONEMAP_REINHARD02:
        {
            const slg::Reinhard02ToneMapParams *pRealParam = dynamic_cast<const slg::Reinhard02ToneMapParams*>(param);
            this->AppendIn(pToneMap,this->setFloatEditor(new wxFloatProperty(gmeWXT("prescale"),wxString("prescale"),pRealParam->preScale),0.0f,8.0f ) );
            this->AppendIn(pToneMap,this->setFloatEditor(new wxFloatProperty(gmeWXT("postscale"),wxString("postscale"),pRealParam->postScale) , 0.0f, 8.0f) );
            this->AppendIn(pToneMap,this->setFloatEditor(new wxFloatProperty(gmeWXT("burn"),wxString("burn"),pRealParam->burn),0.0f,8.0f) );
        }
        break;
    }
}

void
HDRPage::buildCamera(wxPGProperty *pCamCate)
{
    //设置camera信息。
    Camera      cam;
    DocCamera   doccamera;
    if(doccamera.saveTo(cam))
    {
        DECLARE_WXCONVERT;

        this->AppendIn(pCamCate,this->setFloatEditor(new wxFloatProperty(gmeWXT("视野"),wxString("fov"),cam.fieldOfView) , 15.0f, 360.0f) );
        this->AppendIn(pCamCate,this->setFloatEditor(new wxFloatProperty(gmeWXT("焦距"),wxString("lensradius"),cam.lensRadius) , 0.0f, 1.0f) );
        this->AppendIn(pCamCate,new wxFloatProperty(gmeWXT("聚焦距离"),wxString("focaldistance"),cam.focalDistance) );
        this->AppendIn(pCamCate,new wxFloatProperty(gmeWXT("近平面"),wxString("hither"),cam.clipHither) );
        this->AppendIn(pCamCate,new wxFloatProperty(gmeWXT("远平面"),wxString("yon"),cam.clipYon) );
        this->AppendIn(pCamCate,new wxIntProperty(gmeWXT("终止条件"),wxString("pass"),cam.pass) );
    }
}

void
HDRPage::buildPage(void)
{
    DECLARE_WXCONVERT;
	// append film tonemap
	wxPGProperty* pCate = this->Append(new wxPropertyCategory(gmeWXT("图像"),wxString("film")));

    {
        DocImg  img;
        int w = 0,h = 0;
        img.getSize(w,h);
        std::string value = boost::str(boost::format("%d %d") % w % h);
        wxPGProperty* pprop = new wxStringProperty(gmeWXT("尺寸"),wxString("size"), value);
        this->AppendIn(pCate,pprop);
    }
    {
        DocSetting  setting;

        {
            const slg::ToneMapParams *param = setting.getToneMapParams();

            int typevlaue = slg::TONEMAP_NONE;
            if(param)
                typevlaue = param->GetType();
            // Append is ideal way to add items to wxPropertyGrid.
            wxPGChoices soc;
            soc.Add( gmeWXT("线性映射"), slg::TONEMAP_LINEAR);
            soc.Add( gmeWXT("压缩映射"), slg::TONEMAP_REINHARD02);
            soc.Add( gmeWXT("无映射"), slg::TONEMAP_NONE);
            wxPGProperty *pToneMap = this->AppendIn(pCate,new wxEnumProperty(gmeWXT("图像映射"),wxString("tonemap"), soc,typevlaue));
            refreshToneMapping(typevlaue,param,pToneMap);
        }

        //设置gamma
        this->AppendIn(pCate,new wxFloatProperty(gmeWXT(constDef::gamma),wxString(constDef::gamma),setting.getGamma() ) );

        //设置filter.
        {
            wxPGChoices soc;
            soc.Add( gmeWXT("无"), slg::FILTER_NONE);
            soc.Add( gmeWXT("矩形过滤"), slg::FILTER_BOX);
            soc.Add( gmeWXT("高斯过滤"), slg::FILTER_GAUSSIAN);
            soc.Add( gmeWXT("米切尔过滤"), slg::FILTER_MITCHELL);
            soc.Add( gmeWXT("改进型米切尔过滤"), slg::FILTER_MITCHELL_SS);
            this->AppendIn(pCate,new wxEnumProperty(gmeWXT("反走样"),wxString(constDef::filter), soc,setting.getFilmFilter()));
        }

        //设置文件保存
        {
            std::string output("document.output");
            std::string     filename1,filename2;
            if(gme::Option::instance().is_existed(output))
            {
                std::vector<std::string>    outset = gme::Option::instance().get<std::vector<std::string> >(output);
                if(outset.size() >= 1)
                    filename1 = outset[0];
                if(outset.size() >= 2)
                    filename2 = outset[1];
            }

            wxPGProperty *pProp = new wxFileProperty(gmeWXT("保存照片1"),wxString("filename1"), filename1);
            this->AppendIn(pCate,pProp);
            pProp = new wxFileProperty(gmeWXT("保存照片2"),wxString("filename2"), filename2);
            this->AppendIn(pCate,pProp);
        }
    }

	wxPGProperty* pCamCate = this->Append(new wxPropertyCategory(gmeWXT("相机"),wxString(constDef::camera)));
	buildCamera(pCamCate);
}


void
HDRPage::onDocumentClosed(void)
{
    clearPage();
}

HDRPage::HDRPage()
{
    DocIO   dio;
    dio.onSceneLoaded(boost::bind(&HDRPage::onDocumentLoaded,this));
    dio.onSceneClosed(boost::bind(&HDRPage::onDocumentClosed,this));

    DocCamera   doccam;
    doccam.onCameraSwitched(boost::bind(&HDRPage::onCameraSwitched,this));
}

void
HDRPage::updateCameraField(const wxString &propid,float value)
{
    wxPGProperty  *pprop = this->GetProperty(propid);
    if(pprop)
    {
        pprop->SetValue(value);
    }
}

void
HDRPage::updateCamera(void)
{
    DocCamera   doccamera;
    int sel = doccamera.getSelected();
    if(sel >= 0 && sel < (int)doccamera.size())
    {//收到消息时，当前camera可能尚未更新。
        Camera      &cam = doccamera.get(sel);
        updateCameraField("fov",cam.fieldOfView);
        updateCameraField("lensradius",cam.lensRadius);
        updateCameraField("focaldistance",cam.focalDistance);
        updateCameraField("hither",cam.clipHither);
        updateCameraField("yon",cam.clipYon);

        wxPGProperty  *pprop = this->GetProperty(constDef::pass);
        if(pprop)
        {
            pprop->SetValue(cam.pass);
        }
    }
}

void
HDRPage::onCameraSwitched(void)
{
    updateCamera();
}

HDRPage::~HDRPage()
{
}


void HDRPage::OnPropertySelect( wxPropertyGridEvent& WXUNUSED(event) )
{
}

void HDRPage::OnPropertyChange( wxPropertyGridEvent& event )
{
}

#ifdef PROPERTY_HAS_DELETE_PROPERTY
#else
void
HDRPage::OnRefreshPage(wxCommandEvent &event)
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


void HDRPage::OnPropertyChanging( wxPropertyGridEvent& event )
{
    wxBusyCursor wait;

    wxPGProperty* p = event.GetProperty();


    std::string     id(p->GetName().c_str());

    if(boost::equals(id,"size"))
    {
        std::vector< std::string >    valArray;
        std::string     value(event.GetValue().GetString().c_str());
        boost::split(valArray,value,boost::is_any_of(" "),boost::token_compress_on);
        bool bSet = false;
        if(valArray.size() == 2)
        {
            int w = boost::lexical_cast<int>(valArray[0]);
            int h = boost::lexical_cast<int>(valArray[1]);
            if(w > 0 && w < 8092 && h > 0 && h < 8092)
            {
                DocImg  img;
                bSet = img.setSize(w,h);
            }
        }
        if(!bSet && event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,constDef::gamma))
    {
        wxAny any_value = event.GetValue();
        BOOST_ASSERT(any_value.CheckType<float>());
        float gamma = any_value.As<float>();
        DocSetting  setting;
        if(!setting.setGamma(gamma) && event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,constDef::filter))
    {
        wxAny any_value = event.GetValue();
        BOOST_ASSERT(any_value.CheckType<int>());
        int filter = any_value.As<int>();
        DocSetting  setting;
        if(!setting.setFilmFilter(filter) && event.CanVeto())
        {
            event.Veto();
        }
    }else if(boost::equals(id,"tonemap"))
    {
        wxAny any_value = event.GetValue();
        BOOST_ASSERT(any_value.CheckType<int>());
        int type = any_value.As<int>();

        DocSetting  setting;
        switch(type)
        {
        case slg::TONEMAP_LINEAR:
            {
                slg::LinearToneMapParams    toneMap;
                setting.setToneMapParams(toneMap);
            }
            break;
        case slg::TONEMAP_NONE:
            {
                NoneToneMapParams   toneMap;
                setting.setToneMapParams(toneMap);
            }
            break;
        case slg::TONEMAP_REINHARD02:
            {
                slg::Reinhard02ToneMapParams    toneMap;
                setting.setToneMapParams(toneMap);
            }
            break;
        default:
            BOOST_ASSERT(false);
        }
#ifdef PROPERTY_HAS_DELETE_PROPERTY
        this->removeChild(p);
        refreshToneMapping(type,setting.getToneMapParams(),p);
        this->RefreshProperty(p);
#else
        wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED, cmd::GID_REFRESH_MATPROP);
        wxPostEvent(this,evt);
#endif
    }else if(boost::equals(id,"tonemap.scale"))
    {
        wxAny any_value = event.GetValue();
        BOOST_ASSERT(any_value.CheckType<float>());
        float value = any_value.As<float>();

//        BOOST_ASSERT(setting.getToneMapParams()->GetType() == slg::TONEMAP_LINEAR);
        slg::LinearToneMapParams  param(value);
        {
            DocSetting  setting;
            setting.setToneMapParams(param);
        }
    }else if(boost::equals(id,"tonemap.prescale"))
    {
        wxAny any_value = event.GetValue();
        BOOST_ASSERT(any_value.CheckType<float>());
        float value = any_value.As<float>();

        DocSetting  setting;
        BOOST_ASSERT(setting.getToneMapParams()->GetType() == slg::TONEMAP_REINHARD02);
        const slg::Reinhard02ToneMapParams  *pOrig = dynamic_cast<const slg::Reinhard02ToneMapParams*>(setting.getToneMapParams());

        slg::Reinhard02ToneMapParams  param(value,pOrig->postScale,pOrig->burn);
        setting.setToneMapParams(param);
    }else if(boost::equals(id,"tonemap.postscale"))
    {
        wxAny any_value = event.GetValue();
        BOOST_ASSERT(any_value.CheckType<float>());
        float value = any_value.As<float>();

        DocSetting  setting;
        BOOST_ASSERT(setting.getToneMapParams()->GetType() == slg::TONEMAP_REINHARD02);
        const slg::Reinhard02ToneMapParams  *pOrig = dynamic_cast<const slg::Reinhard02ToneMapParams*>(setting.getToneMapParams());

        slg::Reinhard02ToneMapParams  param(pOrig->preScale,value,pOrig->burn);
        setting.setToneMapParams(param);
    }else if(boost::equals(id,"tonemap.burn"))
    {
        wxAny any_value = event.GetValue();
        BOOST_ASSERT(any_value.CheckType<float>());
        float value = any_value.As<float>();

        DocSetting  setting;
        BOOST_ASSERT(setting.getToneMapParams()->GetType() == slg::TONEMAP_REINHARD02);
        const slg::Reinhard02ToneMapParams  *pOrig = dynamic_cast<const slg::Reinhard02ToneMapParams*>(setting.getToneMapParams());

        slg::Reinhard02ToneMapParams  param(pOrig->preScale,pOrig->postScale,value);
        setting.setToneMapParams(param);
    }else if(boost::equals(id,"pass"))
    {
        wxAny any_value = event.GetValue();
        DocCamera   doccam;
        int sel = doccam.getSelected();
        if(sel >= 0 && sel < (int)doccam.size())
        {
            int  pass = any_value.As<int>();
            doccam.get(sel).pass = pass;
            gme::MainFrame* mainfrm = dynamic_cast<gme::MainFrame*>(wxTheApp->GetTopWindow());
            if(mainfrm)
            {
                mainfrm->setTerminatePass(pass);
            }
        }
    }else if(boost::starts_with(id,"filename"))
    {
        std::string     output("document.output");
        std::vector<std::string>    outset;
        if(gme::Option::instance().is_existed(output))
        {
            outset = gme::Option::instance().get<std::vector<std::string> >(output);
        }

        if(outset.size() < 2)
            outset.resize(2);

        int idx = boost::lexical_cast<int>(id.substr(strlen("filename"))) - 1;

        wxAny any_value = event.GetValue();
        std::string value;
        if(any_value.CheckType<wxString>()){
            wxString  wxvalue = any_value.As<wxString>();
            value = boost::locale::conv::utf_to_utf<char>(wxvalue.ToStdWstring());
        }else{
            value =  event.GetValue().GetString().c_str();
        }
        outset[idx] = value;
        gme::Option::instance().put(output,outset);
    }else
    {//设置camera.
        Camera      cam;
        DocCamera   doccam;
        if(doccam.saveTo(cam))
        {
            //获取当前camera的名字。
            int sel = doccam.getSelected();
            if(sel >= 0 && sel < (int)doccam.size())
            {
                cam.name = doccam.get(sel).name;
            }

            wxAny any_value = event.GetValue();
            BOOST_ASSERT(any_value.CheckType<float>());
            float value = any_value.As<float>();

            if(boost::equals(id,"fov"))
            {
                cam.fieldOfView = value;
            }else if(boost::equals(id,"lensradius"))
            {
                cam.lensRadius = value;
            }else if(boost::equals(id,"focaldistance"))
            {
                cam.focalDistance = value;
            }else if(boost::equals(id,"hither"))
            {
                cam.clipHither = value;
            }else if(boost::equals(id,"yon"))
            {
                cam.clipYon = value;
            }else{
                BOOST_ASSERT_MSG(false,"unknow id value");
            }

            if(doccam.restoreFrom(cam))
            {
                int sel = doccam.getSelected();
                if(sel >= 0 && sel < (int)doccam.size())
                {
                    doccam.get(sel) = cam;
                }
            }else if(event.CanVeto())
            {
                event.Veto();
            }
        }
    }
}


}

