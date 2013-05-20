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

#ifndef  GME_GMESTATUS_H
#define  GME_GMESTATUS_H

#include <wx/wx.h>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace gme{

class GMEStatusBar : public wxStatusBar
{
public:
    typedef    wxStatusBar     inherit;
    GMEStatusBar(wxWindow *parent, long style = wxSTB_DEFAULT_STYLE);
    virtual ~GMEStatusBar();
protected:
	enum{
		Field_TEXT,
		Field_END,
		Field_PROGRESS,
		Field_CONVERGENCE,
		Field_PASS,
		Field_TIME,
		Field_RPS,
		Field_SPS,
		Field_TOTOAL
	};

    void UpdateInfo();

    // event handlers
    void OnSize(wxSizeEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnGaugeClick(wxMouseEvent& event);
    void OnSetPass(wxCommandEvent &event);
    void OnSetTime(wxCommandEvent &event);
    void OnSetConvergence(wxCommandEvent &event);
    void OnClearCondition(wxCommandEvent &event);
    void SetTimeField(unsigned int time,int field_id);

    void onRenderInfoType(wxCommandEvent &event);
    void onUpdateRenderInfoType(wxUpdateUIEvent &event);
private:
    int  getCmdIdFromShowType(void);
    wxStaticBitmap *m_statbmp;
    wxGauge        *m_pGauge;
    int             m_targetPass;   //目标pass.
    int             m_targetTime;   //目标时间。
    int             m_targetConv;   //目标覆盖率。
    boost::posix_time::ptime    m_micro_tick;   //控制刷新率.
    int             opt_refresh_tick;   //0.5s default
    ///@biref   度量信息的显示模式。缺省是native.
    int             m_showType;

    DECLARE_EVENT_TABLE()
};

} //end namespace gme

#endif //GME_GMESTATUS_H
