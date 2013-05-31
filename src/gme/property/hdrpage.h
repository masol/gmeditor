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

#ifndef  GME_PROPERTY_HDRPAGE_H
#define  GME_PROPERTY_HDRPAGE_H

#include <wx/wx.h>
#include <wx/propgrid/manager.h>
#include "gmeproppage.h"
#include "slg/film/tonemapping.h"

namespace gme{

class HDRPage : public GmePropPage
{
    typedef GmePropPage inherit;
public:
    HDRPage();
    virtual ~HDRPage();
private:
    void clearPage(void);
    void buildPage(void);
    void refreshToneMapping(int typeValue,const slg::ToneMapParams *param,wxPGProperty *pToneMap);
    void buildCamera(wxPGProperty *pCamCate);
    void updateCamera(void);
    void updateCameraField(const wxString &propid,float value);
protected:
#ifdef PROPERTY_HAS_DELETE_PROPERTY
#else
    void OnRefreshPage(wxCommandEvent &event);
#endif
    void onDocumentLoaded(void);
    void onDocumentClosed(void);
    void onCameraSwitched(void);
    void OnPropertySelect( wxPropertyGridEvent& event );
    void OnPropertyChanging( wxPropertyGridEvent& event );
    void OnPropertyChange( wxPropertyGridEvent& event );
private:
    DECLARE_EVENT_TABLE()
};

}

#endif //GME_PROPERTY_HDRPAGE_H
