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
#include <boost/format.hpp>
#include "utils/i18n.h"
#include "utils/option.h"
#include "dm/docimg.h"
#include "dm/docctl.h"
#include "dm/doccamera.h"
#include "dm/docio.h"
#include "gmestatus.h"
#include "cmdids.h"
#include "stringutil.h"
#include "mainframe.h"

namespace gme{

BEGIN_EVENT_TABLE(GMEStatusBar, GMEStatusBar::inherit)
    EVT_SIZE(GMEStatusBar::OnSize)
    EVT_IDLE(GMEStatusBar::OnIdle)
	EVT_MENU(cmd::GID_SB_SE_PASS,GMEStatusBar::OnSetPass)
	EVT_MENU(cmd::GID_SB_SE_TIME,GMEStatusBar::OnSetTime)
	EVT_MENU(cmd::GID_SB_SE_CONVERGENCE,GMEStatusBar::OnSetConvergence)
	EVT_MENU(cmd::GID_SB_SE_CLEARALL,GMEStatusBar::OnClearCondition)
    EVT_UPDATE_UI_RANGE(cmd::GID_SB_SE_PASS,cmd::GID_SB_SE_CLEARALL,GMEStatusBar::onUpdateRenderTerminate)

    EVT_MENU_RANGE(cmd::GID_SB_RI_BEGIN,cmd::GID_SB_RI_END,GMEStatusBar::onRenderInfoType)
    EVT_UPDATE_UI_RANGE(cmd::GID_SB_RI_BEGIN,cmd::GID_SB_RI_END,GMEStatusBar::onUpdateRenderInfoType)
END_EVENT_TABLE()

GMEStatusBar::GMEStatusBar(wxWindow *parent, long style)
    : wxStatusBar(parent, wxID_ANY, style, "gmeStatusBar")
{
    //计算提示窗口尺寸。
    wxClientDC dc(this);

    int timeWidth = dc.GetTextExtent(" 00:00:00 ").x;

    int widths[Field_TOTOAL];
    widths[Field_TEXT] = -1; // growable
    widths[Field_END] = timeWidth;
    widths[Field_PROGRESS] = 100;
    widths[Field_CONVERGENCE] = dc.GetTextExtent("100.00%").x;
    widths[Field_PASS] = dc.GetTextExtent("100000").x;
    widths[Field_TIME] = timeWidth;
    widths[Field_RPS] = dc.GetTextExtent("RPS:999.999M").x;
    widths[Field_SPS] = widths[Field_RPS];

    SetFieldsCount(Field_TOTOAL);
    SetStatusWidths(Field_TOTOAL, widths);

    m_pGauge = new wxGauge(this, wxID_ANY, 100);
    m_pGauge->SetValue(0);

    m_targetPass = 0;
    m_targetTime = 0;
    m_targetConv = 0;

    m_showType = gme::DocImg::RI_TOTAL;

    m_pGauge->Connect(wxEVT_LEFT_UP,wxMouseEventHandler(GMEStatusBar::OnGaugeClick),NULL,this);
    m_micro_tick = boost::posix_time::microsec_clock::local_time();
    opt_refresh_tick = 500;

    m_currentSource = -1;
    m_currentCamera = -1;
    m_autoRender = false;
    //SetMinHeight(m_pGauge->GetBestSize().GetHeight());
}

void
GMEStatusBar::OnClearCondition(wxCommandEvent &event)
{
    m_targetPass = 0;
    m_targetTime = 0;
    m_targetConv = 0;
}

void
GMEStatusBar::OnSetPass(wxCommandEvent &event)
{
    m_targetPass = 16000;
}

bool
GMEStatusBar::switchToNextSrc(void)
{
    std::string source("document.source");
    if(gme::Option::instance().is_existed(source))
    {
        gme::DocIO docio;
        std::vector<std::string> srcset = gme::Option::instance().get<std::vector<std::string> >(source);
        size_t  start = (m_currentSource >= 0 ? m_currentSource + 1 : 0);
        for(size_t idx = start; idx < srcset.size(); idx++)
        {
            if(docio.loadScene(srcset[idx]))
            {
                m_currentSource = idx;
                return true;
            }
        }
    }
    return false;
}

void
GMEStatusBar::quitProgram(void)
{
    gme::MainFrame* mainfrm = dynamic_cast<gme::MainFrame*>(wxTheApp->GetTopWindow());
    if(mainfrm)
    {
        mainfrm->quitProgram();
    }
}


bool
GMEStatusBar::switchToNextCamera(void)
{
    std::string  camera("document.camera");
    if(gme::Option::instance().is_existed(camera))
    {//存在camera.以此校正camera。
        std::vector<std::string> camset = gme::Option::instance().get<std::vector<std::string> >(camera);
        DocCamera doccam;
        size_t start = (m_currentCamera >= 0) ? m_currentCamera+1 : 0;
        if(start < camset.size())
        {
            for(size_t idx = start ; idx < camset.size() ; idx++)
            {
                int camId = doccam.find(camset[idx]);
                if(camId >= 0 && doccam.setSelected(camId))
                {
                    //cameraview会自动切换，所以这里不需要切换camera.
                    m_currentCamera = idx;
                    return true;
                }
            }
        }
    }
    return false;
}

void
GMEStatusBar::setOpenFromCommandLine(int srcIdx)
{
    m_currentSource = srcIdx;
    std::string pass("pass");
    if(gme::Option::instance().is_existed(pass))
    {
        m_targetPass = gme::Option::instance().get<long>(pass);
        if(m_targetPass >  0)
        {
            m_autoRender = true;
        }
    }
    switchToNextCamera();
}

void
GMEStatusBar::OnSetTime(wxCommandEvent &event)
{
}

void
GMEStatusBar::OnSetConvergence(wxCommandEvent &event)
{
}

void
GMEStatusBar::OnGaugeClick(wxMouseEvent& event)
{
    wxPoint point = event.GetPosition();
    // If from keyboard
    if (point.x == -1 && point.y == -1) {
        wxSize size = GetSize();
        point.x = size.x / 2;
        point.y = size.y / 2;
    } else {
        //point = m_pGauge->ScreenToClient(point);
    }

    DECLARE_WXCONVERT;

    wxMenu menu;
    menu.AppendCheckItem(cmd::GID_SB_SE_PASS, gmeWXT("设置次数终止"));
    menu.AppendCheckItem(cmd::GID_SB_SE_TIME, gmeWXT("设置时间终止"));
    menu.AppendCheckItem(cmd::GID_SB_SE_CONVERGENCE, gmeWXT("设置覆盖率终止"));
    menu.AppendCheckItem(cmd::GID_SB_SE_CLEARALL, gmeWXT("永久渲染"));
    menu.AppendSeparator();
    menu.AppendCheckItem(cmd::GID_SB_RI_NATIVE, gmeWXT("本地效率"));
    menu.AppendCheckItem(cmd::GID_SB_RI_CONTRIBUTE, gmeWXT("贡献效率"));
    menu.AppendCheckItem(cmd::GID_SB_RI_TOTAL, gmeWXT("总效率"));

    m_pGauge->PopupMenu(&menu, point);
}

void
GMEStatusBar::OnSize(wxSizeEvent& event)
{
    wxRect rect;
    if (!GetFieldRect(Field_PROGRESS, rect))
    {
        event.Skip();
        return;
    }
    wxRect rectCheck = rect;
    rectCheck.Deflate(2);
    m_pGauge->SetSize(rectCheck);
    event.Skip();
}

void
GMEStatusBar::SetTimeField(unsigned int time,int field_id)
{
    unsigned int  h,m,s;
    s = time % 60;
    time /= 60;
    m = time % 60;
    time /= 60;
    h = time;
    std::string info = boost::str(boost::format("%02d:%02d:%02d")% h % m % s);
    SetStatusText(info, field_id);
}

void
GMEStatusBar::terminateCurrent(void)
{
    //如果请求保存。
    std::string output("document.output");
    if(gme::Option::instance().is_existed(output))
    {
        DocImg  img;
        std::vector<std::string> outset = gme::Option::instance().get<std::vector<std::string> >(output);
        BOOST_FOREACH(const std::string &filename,outset)
        {
            img.saveImage(boost::filesystem::absolute(filename,boost::filesystem::current_path()).string());
        }
    }

    if(m_autoRender)
    {
        if(!switchToNextCamera())
        {
            if(!switchToNextSrc())
            {//Quit Program.
                quitProgram();
            }
        }
    }else{
        DocCtl  ctl;
        ctl.pause();
    }
}


void
GMEStatusBar::OnIdle(wxIdleEvent& event)
{
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration diff = now - m_micro_tick;
    if(diff.total_milliseconds() > opt_refresh_tick)
    {
        DocImg  img;
        gme::RenderInfo  ri;
        if(img.getRenderInfo(m_showType,ri))
        {
            std::string    info = boost::str(boost::format("%2.2f%%")% (ri.convergence * 100.0f));
            SetStatusText(info, Field_CONVERGENCE);
            info = boost::str(boost::format("%d")% (ri.pass));
            SetStatusText(info, Field_PASS);
            const char *unit = " KMGTPEZY";
            const char *c = unit;
            while(ri.totalRaysSec >= 1000.0f)
            {
                ri.totalRaysSec /= 1000.0f;
                c++;
            }
            info = boost::str(boost::format("RPS:%.3f%c")% (ri.totalRaysSec) % (*c));
            SetStatusText(info, Field_RPS);

            c = unit;
            while(ri.totalSamplesSec >= 1000.0f)
            {
                ri.totalSamplesSec /= 1000.0f;
                c++;
            }

            info = boost::str(boost::format("SPS:%.3f%c")% (ri.totalSamplesSec) % (*c));
            SetStatusText(info, Field_SPS);

            SetTimeField(ri.elapsedTime,Field_TIME);

            float value = 0;
            bool bNoStopCondition = false;
            if(m_targetPass)
            {
                value = ((float)ri.pass / (float)m_targetPass);
            }else if(m_targetTime){
            }else if(m_targetConv){
            }else{
                bNoStopCondition = true;
                SetStatusText("--:--:--", Field_END);
            }

            if(!bNoStopCondition)
            {
                if(value >= 1.0f)
                {
                    terminateCurrent();
                    SetStatusText("00:00:00", Field_END);
                }else{
                    int restTime = 0;
                    if(value > 0)
                    {
                        restTime = (unsigned int) ( (ri.elapsedTime / value)  - ri.elapsedTime) ;
                    }
                    SetTimeField(restTime,Field_END);
                }
                m_pGauge->SetValue( (value >= 1.0f) ? 100: (int)(value * 100.0f));
            }

        }else{
            SetStatusText("--", Field_CONVERGENCE);
            SetStatusText("--", Field_PASS);
            SetStatusText("--", Field_RPS);
            SetStatusText("--", Field_SPS);
            SetStatusText("--:--:--", Field_TIME);
        }
        m_micro_tick = now;
    }
    event.Skip();
}

GMEStatusBar::~GMEStatusBar()
{
}

void
GMEStatusBar::onRenderInfoType(wxCommandEvent &event)
{
    switch(event.GetId())
    {
    case cmd::GID_SB_RI_NATIVE:
        this->m_showType = gme::DocImg::RI_NATIVE;
        break;
    case cmd::GID_SB_RI_CONTRIBUTE:
        this->m_showType = gme::DocImg::RI_CONTRIBUTE;
        break;
    case cmd::GID_SB_RI_TOTAL:
        this->m_showType = gme::DocImg::RI_TOTAL;
        break;
    }
}

int
GMEStatusBar::getCmdIdFromShowType(void)
{
    switch(this->m_showType)
    {
    case gme::DocImg::RI_TOTAL:
        return cmd::GID_SB_RI_TOTAL;
    case gme::DocImg::RI_CONTRIBUTE:
        return cmd::GID_SB_RI_CONTRIBUTE;
    case gme::DocImg::RI_NATIVE:
        return cmd::GID_SB_RI_NATIVE;
    }
    throw std::runtime_error("invalid type");
}

void
GMEStatusBar::onUpdateRenderInfoType(wxUpdateUIEvent &event)
{
    event.Check(event.GetId() == getCmdIdFromShowType());
}

void
GMEStatusBar::onUpdateRenderTerminate(wxUpdateUIEvent &event)
{
    switch(event.GetId())
    {
    case cmd::GID_SB_SE_PASS:
        event.Check(m_targetPass != 0);
        break;
    case cmd::GID_SB_SE_TIME:
        event.Check(m_targetPass == 0 && m_targetTime != 0);
        break;
    case cmd::GID_SB_SE_CONVERGENCE:
        event.Check(m_targetPass == 0 && m_targetTime == 0 && m_targetConv != 0);
        break;
    case cmd::GID_SB_SE_CLEARALL:
        event.Check(m_targetPass == 0 && m_targetTime == 0 && m_targetConv == 0);
        break;
    }
}


}
