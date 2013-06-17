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
#include "dm/docio.h"
#include "dm/docsetting.h"
#include "syssetting.h"
#include "../stringutil.h"
#include "../cmdids.h"
#include "../mainframe.h"


BEGIN_EVENT_TABLE(gme::SysPage, gme::SysPage::inherit)
    EVT_PG_CHANGING( wxID_ANY, gme::SysPage::OnPropertyChanging )
END_EVENT_TABLE()


namespace gme{


void
SysPage::onDocumentLoaded(void)
{
    clearPage();
    buildPage();
}

void
SysPage::clearPage(void)
{
    //清空当前页面。
    this->Clear();
}

void
SysPage::buildPage(void)
{
}


void
SysPage::onDocumentClosed(void)
{
    clearPage();
}

SysPage::SysPage()
{
    DocIO   dio;
    dio.onSceneLoaded(boost::bind(&SysPage::onDocumentLoaded,this));
    dio.onSceneClosed(boost::bind(&SysPage::onDocumentClosed,this));
}

SysPage::~SysPage()
{
}


void SysPage::OnPropertyChanging( wxPropertyGridEvent& event )
{
    wxBusyCursor wait;

    wxPGProperty* p = event.GetProperty();


    std::string     id(p->GetName().c_str());
}


}

