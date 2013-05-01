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
#include "dm/docio.h"
#include "dm/docobj.h"
#include <boost/bind.hpp>
#include "../stringutil.h"

BEGIN_EVENT_TABLE(gme::MaterialPage, gme::MaterialPage::inherit)
    EVT_PG_SELECTED( wxID_ANY, gme::MaterialPage::OnPropertySelect )
    EVT_PG_CHANGING( wxID_ANY, gme::MaterialPage::OnPropertyChanging )
    EVT_PG_CHANGED( wxID_ANY, gme::MaterialPage::OnPropertyChange )
END_EVENT_TABLE()


namespace gme{

void
MaterialPage::onDocumentItemSelected(const std::string &id)
{
    if(id != m_currentObject)
    {//刷新材质页面。
        buildPage(id);
    }
}

void
MaterialPage::onDocumentItemDeselected(const std::string &id)
{
    if(m_currentObject == id)
    {
        clearPage();
    }
}

void
MaterialPage::clearPage(void)
{
    //清空当前页面。
    m_currentObject.clear();
}

void
MaterialPage::buildPage(const std::string &objid)
{
    m_currentObject = objid;
}


void
MaterialPage::onDocumentClosed(void)
{
    clearPage();
}

MaterialPage::MaterialPage()
{
    DocIO   dio;
    dio.onSceneClosed(boost::bind(&MaterialPage::onDocumentClosed,this));

    DocObj  dobj;
    dobj.onSelected(boost::bind(&MaterialPage::onDocumentItemSelected,this,_1));
    dobj.onDeselected(boost::bind(&MaterialPage::onDocumentItemDeselected,this,_1));

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

