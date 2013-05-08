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

#ifndef  GME_PROPERTY_ENVLIGHTPAGE_H
#define  GME_PROPERTY_ENVLIGHTPAGE_H

#include <wx/wx.h>
#include <boost/lexical_cast.hpp>
#include "gmeproppage.h"
#include "dm/xmlutil.h"
#include "dm/docsetting.h"

namespace gme{

class EnvLightPage : public GmePropPage
{
private:
    typedef GmePropPage  inherit;
    void    clearPage(void);
    void    buildPage(void);
    void    appendEnvLight(wxPGProperty* pType,DocSetting &setting);
    void    appendSunLight(wxPGProperty* pEnable,DocSetting &setting);
    static  const unsigned int DISABLE_LIGHTER = 0x100;
public:
    EnvLightPage();
    virtual ~EnvLightPage();
private:
    bool OnEnvtypeChanged(wxPGProperty* p,unsigned int type);
protected:
    void OnPropertyChanging( wxPropertyGridEvent& event );
    void OnPropertyChange( wxPropertyGridEvent& event );

	void  onDocumentClosed(void);
	void  onDocumentOpend(void);
private:
    DECLARE_EVENT_TABLE()
};

}

#endif //GME_PROPERTY_ENVLIGHTPAGE_H
