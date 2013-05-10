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
#include "gmestatus.h"
#include "dm/docimg.h"
#include "dm/docctl.h"
#include "cmdids.h"
#include "stringutil.h"
#include <boost/format.hpp>

namespace gme{

BEGIN_EVENT_TABLE(GMEStatusBar, GMEStatusBar::inherit)
    EVT_SIZE(GMEStatusBar::OnSize)
    EVT_IDLE(GMEStatusBar::OnIdle)
	EVT_MENU(cmd::GID_SB_SE_PASS,GMEStatusBar::OnSetPass)
	EVT_MENU(cmd::GID_SB_SE_TIME,GMEStatusBar::OnSetTime)
	EVT_MENU(cmd::GID_SB_SE_CONVERGENCE,GMEStatusBar::OnSetConvergence)
	EVT_MENU(cmd::GID_SB_SE_CLEARALL,GMEStatusBar::OnClearCondition)
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

    m_pGauge->Connect(wxEVT_LEFT_UP,wxMouseEventHandler(GMEStatusBar::OnGaugeClick),NULL,this);
    m_micro_tick = boost::posix_time::microsec_clock::local_time();
    opt_refresh_tick = 500;

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
    m_targetPass = 10000;
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
    menu.Append(cmd::GID_SB_SE_PASS, gmeWXT("设置次数终止"));
    menu.Append(cmd::GID_SB_SE_TIME, gmeWXT("设置时间终止"));
    menu.Append(cmd::GID_SB_SE_CONVERGENCE, gmeWXT("设置覆盖率终止"));
    menu.Append(cmd::GID_SB_SE_CLEARALL, gmeWXT("永久渲染"));

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
GMEStatusBar::OnIdle(wxIdleEvent& event)
{
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration diff = now - m_micro_tick;
    if(diff.total_milliseconds() > opt_refresh_tick)
    {
        DocImg  img;
        DocImg::RenderInfo  ri;
        if(img.getRenderInfo(ri))
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
                    DocCtl  ctl;
                    ctl.stop();
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
            SetStatusText("", Field_CONVERGENCE);
        }
        m_micro_tick = now;
    }
    event.Skip();
}

GMEStatusBar::~GMEStatusBar()
{
}



}
