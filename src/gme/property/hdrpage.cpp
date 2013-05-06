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
#include "hdrpage.h"
#include "../stringutil.h"

BEGIN_EVENT_TABLE(gme::HDRPage, gme::HDRPage::inherit)
    EVT_PG_SELECTED( wxID_ANY, gme::HDRPage::OnPropertySelect )
    EVT_PG_CHANGING( wxID_ANY, gme::HDRPage::OnPropertyChanging )
    EVT_PG_CHANGED( wxID_ANY, gme::HDRPage::OnPropertyChange )
END_EVENT_TABLE()


namespace gme{

HDRPage::HDRPage()
{
    DECLARE_WXCONVERT;
	// append film tonemap
	//wxPGProperty* pf = this->Append(new wxPropertyCategory(gmeWXT("图像"),gmeWXT("film")));
    // Append is ideal way to add items to wxPropertyGrid.
	wxPGChoices soc;
	soc.Add( gmeWXT("linear"));
    soc.Add( gmeWXT("reinhard02"));
	wxPGProperty* pid = new wxEnumProperty(gmeWXT("图像映射"),gmeWXT("film.tonemap"), soc);
	//pid->SetClientObject(new wxStringClientData(gmeWXT("type")));
	this->Append(pid);

	RefreshProperty(pid);
}


HDRPage::~HDRPage()
{
}


void HDRPage::OnPropertySelect( wxPropertyGridEvent& WXUNUSED(event) )
{
    std::cerr << "HDRPage::OnPropertySelect()" << std::endl;
}

void HDRPage::OnPropertyChange( wxPropertyGridEvent& event )
{
    wxPGProperty* p = event.GetProperty();
    std::cerr << "HDRPage::OnPropertyChange('" << p->GetName().c_str() << "', to value '" << p->GetDisplayedString().c_str() << "')" << std::endl;
}

void HDRPage::OnPropertyChanging( wxPropertyGridEvent& event )
{
    wxPGProperty* p = event.GetProperty();
    std::cerr << "HDRPage::OnPropertyChanging('" << p->GetName().c_str() << "', to value '" << event.GetValue().GetString().c_str() << "')" << std::endl;
}


}

