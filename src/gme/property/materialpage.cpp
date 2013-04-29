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
#include "materialpage.h"
#include "../stringutil.h"

BEGIN_EVENT_TABLE(gme::MaterialPage, gme::MaterialPage::inherit)
    EVT_PG_SELECTED( wxID_ANY, gme::MaterialPage::OnPropertySelect )
    EVT_PG_CHANGING( wxID_ANY, gme::MaterialPage::OnPropertyChanging )
    EVT_PG_CHANGED( wxID_ANY, gme::MaterialPage::OnPropertyChange )
END_EVENT_TABLE()


namespace gme{

MaterialPage::MaterialPage()
{
	// append film tonemap
//	wxPGProperty* pf = this->Append(new wxPropertyCategory(gmeWXT("材质定义"),gmeWXT("material")));
}


MaterialPage::~MaterialPage()
{
}


void MaterialPage::OnPropertySelect( wxPropertyGridEvent& WXUNUSED(event) )
{
    std::cerr << "MaterialPage::OnPropertySelect()" << std::endl;
}

void MaterialPage::OnPropertyChange( wxPropertyGridEvent& event )
{
    wxPGProperty* p = event.GetProperty();
    std::cerr << "MaterialPage::OnPropertyChange('" << p->GetName().c_str() << "', to value '" << p->GetDisplayedString().c_str() << "')" << std::endl;
}

void MaterialPage::OnPropertyChanging( wxPropertyGridEvent& event )
{
    wxPGProperty* p = event.GetProperty();
    std::cerr << "MaterialPage::OnPropertyChanging('" << p->GetName().c_str() << "', to value '" << event.GetValue().GetString().c_str() << "')" << std::endl;
}


}

