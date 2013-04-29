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
#include "wx/wxprec.h"
#include "wx/wx.h"
#include <wx/numdlg.h>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include "objectview.h"
#include "propgrid.h"
#include "property/hdrpage.h"
#include "property/materialpage.h"

// -----------------------------------------------------------------------
// Main propertygrid header.
#include <wx/propgrid/propgrid.h>
// Extra property classes.
#include <wx/propgrid/advprops.h>
// This defines wxPropertyGridManager.
#include <wx/propgrid/manager.h>

#include <wx/artprov.h>
#include "stringutil.h"
#include "property.h"
#include "mainframe.h"

namespace gme{

enum
{
    PGID = 1
};

// -----------------------------------------------------------------------
// Event table
// -----------------------------------------------------------------------

BEGIN_EVENT_TABLE(PropFrame, wxScrolledWindow)
    // This occurs when propgridmanager's page changes.
    EVT_PG_PAGE_CHANGED( PGID, PropFrame::OnPropertyGridPageChange )
END_EVENT_TABLE()

void PropFrame::OnPropertyGridPageChange( wxPropertyGridEvent& (event) )
{
    int idx = this->m_pPropGridManager->GetSelectedPage();
    GmePropPage *pGmePage = dynamic_cast<GmePropPage*>(m_pPropGridManager->GetPage(idx));
    if(pGmePage)
    {
        if(m_pLastShownPage)
        {
            m_pLastShownPage->onPageHiden();
        }
        pGmePage->onPageShown();
        m_pLastShownPage = pGmePage;
        std::cerr << "PropFrame::OnPropertyGridPageChange = " << idx <<std::endl;
    }
}

void PropFrame::initPages()
{
	DECLARE_WXCONVERT;

	wxPropertyGridPage *pPage = m_pPropGridManager->AddPage(gmeWXT("高动态图映射"),wxPG_NULL_BITMAP,new HDRPage());

	m_pPropGridManager->AddPage(gmeWXT("材质属性"),wxPG_NULL_BITMAP,new MaterialPage());


    GmePropPage *pGmePage = dynamic_cast<GmePropPage*>(pPage);
    BOOST_ASSERT(pGmePage != NULL);
	m_pPropGridManager->SelectPage(pPage);
    pGmePage->onPageShown();
    m_pLastShownPage = pGmePage;

//    m_pPropGridManager->Refresh();
//    m_pPropGridManager->RefreshGrid();
}

// -----------------------------------------------------------------------

PropFrame::PropFrame(wxFrame *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, const long& style) :
           wxScrolledWindow(parent, id, pos, size, style)
{
    m_pLastShownPage = NULL;
    //SetIcon(wxICON(sample));
//    m_panel = NULL;
/*
#if wxUSE_IMAGE
    // This is here to really test the wxImageFileProperty.
    wxInitAllImageHandlers();
#endif
*/
    // Register all editors (SpinCtrl etc.)
    wxPropertyGridManager::RegisterAdditionalEditors();

    // Register our sample custom editors
//    m_pSampleMultiButtonEditor =
//        wxPropertyGrid::RegisterEditorClass(new wxSampleMultiButtonEditor());


    long    propstyle = wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_AUTO_SORT |
                //wxPG_HIDE_MARGIN|wxPG_STATIC_SPLITTER | wxPG_TOOLTIPS | wxPG_HIDE_CATEGORIES | wxPG_LIMITED_EDITING |
                wxPG_TOOLBAR | wxPG_DESCRIPTION;
    long    propexstyle = wxPG_EX_MODE_BUTTONS | wxPG_EX_MULTIPLE_SELECTION
                // | wxPG_EX_AUTO_UNSPECIFIED_VALUES | wxPG_EX_GREY_LABEL_WHEN_DISABLED | wxPG_EX_NATIVE_DOUBLE_BUFFERING | wxPG_EX_HELP_AS_TOOLTIPS
                ;

    m_pPropGridManager = new wxPropertyGridManager(this,PGID,wxDefaultPosition,wxSize(100, 100),propstyle);

    m_pPropGridManager->SetExtraStyle(propexstyle);

    // This is the default validation failure behaviour
    m_pPropGridManager->SetValidationFailureBehavior( wxPG_VFB_MARK_CELL |
                                                      wxPG_VFB_SHOW_MESSAGEBOX );

    m_pPropGridManager->GetGrid()->SetVerticalSpacing( 2 );

    m_pPropGridManager->EnableCategories(false);


    initPages();

    // Column
    wxBoxSizer* sizer = new wxBoxSizer ( wxVERTICAL );
    sizer->Add( m_pPropGridManager, 1, wxEXPAND );
    sizer->SetSizeHints( this );
    SetSizer( sizer );

    setDefaultFramePosition();

}

void
PropFrame::establishConnect(ObjectView *pov)
{
    ObjectView::type_callback f = boost::bind(&PropFrame::OnSelectedObjectChanged,this,_1,_2);
    pov->addEventListen(ObjectView::EVT_SELECTION_CHANGED,f);
}


void PropFrame::setDefaultFramePosition()
{
    wxSize frameSize((wxSystemSettings::GetMetric(wxSYS_SCREEN_X)/10)*4,
                     (wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)/10)*8);

    if ( frameSize.x > 500 )
        frameSize.x = 500;

    SetSize(frameSize);
    Centre();
}

PropFrame::~PropFrame()
{
}

} //
