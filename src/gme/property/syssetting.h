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

#ifndef  GME_PROPERTY_SYSSETTING_H
#define  GME_PROPERTY_SYSSETTING_H

#include <wx/wx.h>
#include <wx/propgrid/manager.h>
#include "gmeproppage.h"

namespace gme{

class SysPage : public GmePropPage
{
    typedef GmePropPage inherit;
public:
    SysPage();
    virtual ~SysPage();
private:
    void clearPage(void);
    void buildPage(void);
protected:
    void onDocumentLoaded(void);
    void onDocumentClosed(void);
    void OnPropertyChanging( wxPropertyGridEvent& event );
    void OnRefreshPage(wxCommandEvent &event);
private:
    //指示是否处于清理页面状态。
    bool        m_bInClearStage;
    DECLARE_EVENT_TABLE()
};

}

#endif //GME_PROPERTY_SYSSETTING_H
