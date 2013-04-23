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

// -----------------------------------------------------------------------

// Main propertygrid header.
#include <wx/propgrid/propgrid.h>
// Extra property classes.
#include <wx/propgrid/advprops.h>
// This defines wxPropertyGridManager.
#include <wx/propgrid/manager.h>

#include "propgrid.h"

#include <wx/artprov.h>
#include "stringutil.h"
#include "dm/docsetting.h"

namespace gme{

// -----------------------------------------------------------------------
// Menu IDs
// -----------------------------------------------------------------------

enum
{
    PGID = 1,
};

// -----------------------------------------------------------------------
// Event table
// -----------------------------------------------------------------------

BEGIN_EVENT_TABLE(PropFrame, wxFrame)
    EVT_IDLE(PropFrame::OnIdle)
    EVT_MOVE(PropFrame::OnMove)
    EVT_SIZE(PropFrame::OnResize)

    // This occurs when a property is selected
    EVT_PG_SELECTED( PGID, PropFrame::OnPropertyGridSelect )
    // This occurs when a property value changes
    EVT_PG_CHANGED( PGID, PropFrame::OnPropertyGridChange )
    // This occurs just prior a property value is changed
    EVT_PG_CHANGING( PGID, PropFrame::OnPropertyGridChanging )
    // This occurs when a mouse moves over another property
    EVT_PG_HIGHLIGHTED( PGID, PropFrame::OnPropertyGridHighlight )
    // This occurs when mouse is right-clicked.
    EVT_PG_RIGHT_CLICK( PGID, PropFrame::OnPropertyGridItemRightClick )
    // This occurs when mouse is double-clicked.
    EVT_PG_DOUBLE_CLICK( PGID, PropFrame::OnPropertyGridItemDoubleClick )
    // This occurs when propgridmanager's page changes.
    EVT_PG_PAGE_CHANGED( PGID, PropFrame::OnPropertyGridPageChange )

    EVT_TEXT( PGID, PropFrame::OnPropertyGridTextUpdate )
END_EVENT_TABLE()

// -----------------------------------------------------------------------

void PropFrame::OnMove( wxMoveEvent& event )
{
    if ( !m_pPropGridManager )
    {
        // this check is here so the frame layout can be tested
        // without creating propertygrid
        event.Skip();
        return;
    }

    // Update position properties
    int x, y;
    GetPosition(&x,&y);

    wxPGProperty* id;

    // Must check if properties exist (as they may be deleted).
	DECLARE_WXCONVERT;
    // Using m_pPropGridManager, we can scan all pages automatically.
    id = m_pPropGridManager->GetPropertyByName( gmeWXT("X") );
    if ( id )
        m_pPropGridManager->SetPropertyValue( id, x );

    id = m_pPropGridManager->GetPropertyByName( gmeWXT("Y") );
    if ( id )
        m_pPropGridManager->SetPropertyValue( id, y );

    id = m_pPropGridManager->GetPropertyByName( gmeWXT("Position") );
    if ( id )
        m_pPropGridManager->SetPropertyValue( id, WXVARIANT(wxPoint(x,y)) );

    // Should always call event.Skip() in frame's MoveEvent handler
    event.Skip();
}

// -----------------------------------------------------------------------

void PropFrame::OnResize( wxSizeEvent& event )
{
    if ( !m_pPropGridManager )
    {
        // this check is here so the frame layout can be tested
        // without creating propertygrid
        event.Skip();
        return;
    }

    // Update size properties
    int w, h;
    GetSize(&w,&h);

    wxPGProperty* id;
    wxPGProperty* p;

    // Must check if properties exist (as they may be deleted).
	DECLARE_WXCONVERT;
    // Using m_pPropGridManager, we can scan all pages automatically.
    p = m_pPropGridManager->GetPropertyByName( gmeWXT("Width") );
    if ( p && !p->IsValueUnspecified() )
        m_pPropGridManager->SetPropertyValue( p, w );

    p = m_pPropGridManager->GetPropertyByName( gmeWXT("Height") );
    if ( p && !p->IsValueUnspecified() )
        m_pPropGridManager->SetPropertyValue( p, h );

    id = m_pPropGridManager->GetPropertyByName ( gmeWXT("Size") );
    if ( id )
        m_pPropGridManager->SetPropertyValue( id, WXVARIANT(wxSize(w,h)) );

    // Should always call event.Skip() in frame's SizeEvent handler
    event.Skip();
}

// -----------------------------------------------------------------------

void PropFrame::OnPropertyGridChanging( wxPropertyGridEvent& event )
{
	DECLARE_WXCONVERT;
    wxPGProperty* p = event.GetProperty();

    if ( p->GetName() == gmeWXT("Font") )
    {
        int res =
        wxMessageBox(wxString::Format(gmeWXT("'%s' is about to change (to variant of type '%s')\n\nAllow or deny?"),
                                      p->GetName().c_str(),event.GetValue().GetType().c_str()),
                     gmeWXT("Testing wxEVT_PG_CHANGING"), wxYES_NO, m_pPropGridManager);

        if ( res == wxNO )
        {
            wxASSERT(event.CanVeto());

            event.Veto();

            // Since we ask a question, it is better if we omit any validation
            // failure behaviour.
            event.SetValidationFailureBehavior(0);
        }
    }
}

//
// Note how we use three types of value getting in this method:
//   A) event.GetPropertyValueAsXXX
//   B) event.GetPropertValue, and then variant's GetXXX
//   C) grid's GetPropertyValueAsXXX(id)
//
void PropFrame::OnPropertyGridChange( wxPropertyGridEvent& event )
{
    wxPGProperty* property = event.GetProperty();
	wxStringClientData *clientData  = (wxStringClientData *)property->GetClientObject();
	const wxString& clientName = clientData->GetData();
    // Properties store values internally as wxVariants, but it is preferred
    // to use the more modern wxAny at the interface level
    wxAny value = property->GetValue();

    // Don't handle 'unspecified' values
    if ( value.IsNull() )
        return;
	DECLARE_WXCONVERT;
	float floatValue;
	gme::DocSetting docSetting;
    if ( clientName == gmeWXT("linear scale") )
	{
		floatValue = wxANY_AS(value, float);
		docSetting.setLinearScale(floatValue);
		// @todo:set linear scale by doc
	}
    else if ( clientName == gmeWXT("burn") )
		//@todo: set burn by doc
		floatValue = wxANY_AS(value, float);
}

// -----------------------------------------------------------------------

void PropFrame::OnPropertyGridSelect( wxPropertyGridEvent& event )
{
    wxPGProperty* property = event.GetProperty();
    if ( property )
    {
        //m_itemEnable->Enable( TRUE );
        //if ( property->IsEnabled() )
        //    m_itemEnable->SetItemLabel( gmeWXT("Disable") );
        //else
        //    m_itemEnable->SetItemLabel( gmeWXT("Enable") );
    }
    else
    {
        //m_itemEnable->Enable( FALSE );
    }

#if wxUSE_STATUSBAR
	DECLARE_WXCONVERT;
    wxPGProperty* prop = event.GetProperty();
    wxStatusBar* sb = GetStatusBar();
    if ( prop )
    {
        wxString text(gmeWXT("Selected: "));
        text += m_pPropGridManager->GetPropertyLabel( prop );
        sb->SetStatusText ( text );
    }
#endif
}

// -----------------------------------------------------------------------

void PropFrame::OnPropertyGridPageChange( wxPropertyGridEvent& WXUNUSED(event) )
{
#if wxUSE_STATUSBAR
	DECLARE_WXCONVERT;
    wxStatusBar* sb = GetStatusBar();
    wxString text(gmeWXT("Page Changed: "));
    text += m_pPropGridManager->GetPageName(m_pPropGridManager->GetSelectedPage());
    sb->SetStatusText( text );
#endif
}

void PropFrame::OnPropertyGridHighlight( wxPropertyGridEvent& WXUNUSED(event) )
{
}

// -----------------------------------------------------------------------

void PropFrame::OnPropertyGridItemRightClick( wxPropertyGridEvent& event )
{
#if wxUSE_STATUSBAR
	DECLARE_WXCONVERT;
    wxPGProperty* prop = event.GetProperty();
    wxStatusBar* sb = GetStatusBar();
    if ( prop )
    {
        wxString text(gmeWXT("Right-clicked: "));
        text += prop->GetLabel();
        text += gmeWXT(", name=");
        text += m_pPropGridManager->GetPropertyName(prop);
        sb->SetStatusText( text );
    }
    else
    {
        sb->SetStatusText( wxEmptyString );
    }
#endif
}

// -----------------------------------------------------------------------

void PropFrame::OnPropertyGridItemDoubleClick( wxPropertyGridEvent& event )
{
#if wxUSE_STATUSBAR
	DECLARE_WXCONVERT;
    wxPGProperty* prop = event.GetProperty();
    wxStatusBar* sb = GetStatusBar();
    if ( prop )
    {
        wxString text(gmeWXT("Double-clicked: "));
        text += prop->GetLabel();
        text += gmeWXT(", name=");
        text += m_pPropGridManager->GetPropertyName(prop);
        sb->SetStatusText ( text );
    }
    else
    {
        sb->SetStatusText ( wxEmptyString );
    }
#endif
}

// EVT_TEXT handling
void PropFrame::OnPropertyGridTextUpdate( wxCommandEvent& event )
{
    event.Skip();
}

// -----------------------------------------------------------------------

void PropFrame::OnPropertyGridKeyEvent( wxKeyEvent& WXUNUSED(event) )
{
    // Occurs on wxGTK mostly, but not wxMSW.
}

// -----------------------------------------------------------------------

void PropFrame::PopulateWithScene ()
{
	DECLARE_WXCONVERT;
    wxPropertyGridManager* pgman = m_pPropGridManager;
    wxPropertyGridPage* pg = pgman->GetPage(gmeWXT("Scene Setting"));

	// camera setting
	wxPGProperty* pCamera = pg->Append( new wxStringProperty(gmeWXT("camera"),wxPG_LABEL, gmeWXT("<composed>")) );
	pg->AppendIn( pCamera, new wxStringProperty( gmeWXT("fieldofview"), wxPG_LABEL ) );
	pg->AppendIn( pCamera, new wxStringProperty( gmeWXT("focaldistance"), wxPG_LABEL ) );
	pg->AppendIn( pCamera, new wxStringProperty( gmeWXT("lensradius"), wxPG_LABEL ) );
	pg->AppendIn( pCamera, new wxStringProperty( gmeWXT("lookat"), wxPG_LABEL ) );
	pg->AppendIn( pCamera, new wxStringProperty( gmeWXT("up"), wxPG_LABEL ) );

	// infinitelight setting
	wxPGProperty* pInfLight = pg->Append( new wxStringProperty(gmeWXT("infinitelight"),wxPG_LABEL, gmeWXT("<composed>")) );
	pg->AppendIn( pInfLight, new wxStringProperty( gmeWXT("gain"), wxPG_LABEL ) );
    pg->AppendIn( pInfLight, new wxStringProperty( gmeWXT("shift"), wxPG_LABEL ) );


	// skylight setting
	wxPGProperty* pSkyLight = pg->Append( new wxStringProperty(gmeWXT("skylight"),wxPG_LABEL, gmeWXT("<composed>")) );
	pg->AppendIn( pSkyLight, new wxStringProperty( gmeWXT("dir"), wxPG_LABEL ) );
    pg->AppendIn( pSkyLight, new wxStringProperty( gmeWXT("gain"), wxPG_LABEL ) );
	pg->AppendIn( pSkyLight, new wxStringProperty( gmeWXT("turbidity"), wxPG_LABEL ) );

	// sunlight setting
	wxPGProperty* pSunLight = pg->Append( new wxStringProperty(gmeWXT("sunlight"),wxPG_LABEL, gmeWXT("<composed>")) );
	pg->AppendIn( pSunLight, new wxStringProperty( gmeWXT("dir"), wxPG_LABEL ) );
    pg->AppendIn( pSunLight, new wxStringProperty( gmeWXT("gain"), wxPG_LABEL ) );
	pg->AppendIn( pSunLight, new wxStringProperty( gmeWXT("relsize"), wxPG_LABEL ) );
	pg->AppendIn( pSunLight, new wxStringProperty( gmeWXT("turbidity"), wxPG_LABEL ) );


}

// -----------------------------------------------------------------------

void PropFrame::PopulateWithFilm ()
{
	DECLARE_WXCONVERT;
    wxPropertyGridManager* pgman = m_pPropGridManager;
    wxPropertyGridPage* pg = pgman->GetPage(gmeWXT("Film Setting"));

    // Append is ideal way to add items to wxPropertyGrid.
	wxPGProperty* pid = pg->Append( new wxPropertyCategory(gmeWXT("色彩分布"),wxPG_LABEL));
	pid->SetClientObject( new wxStringClientData(gmeWXT("tonemap")));
	
	pg->SetPropertyCell( pid, 0, wxPG_LABEL );

	wxPGProperty* linearScale = pg->Append( new wxFloatProperty(gmeWXT("线性标尺"),wxPG_LABEL) );
	linearScale->SetClientObject( new wxStringClientData(gmeWXT("linear scale")));

	{
		wxPGProperty* reinhard02 = pg->AppendIn( pid, new wxPropertyCategory(gmeWXT("莱因哈德"), wxPG_LABEL)) ;
		reinhard02->SetClientObject( new wxStringClientData(gmeWXT("reinhard02")));
		
		pg->AppendIn( reinhard02, new wxFloatProperty(gmeWXT("曝光"), wxPG_LABEL))->SetClientObject( new wxStringClientData(gmeWXT("burn"))) ;
		pg->AppendIn( reinhard02, new wxFloatProperty(gmeWXT("postscale"), wxPG_LABEL))->SetClientObject( new wxStringClientData(gmeWXT("postscale"))) ;
		pg->AppendIn( reinhard02, new wxFloatProperty(gmeWXT("prescale"), wxPG_LABEL))->SetClientObject( new wxStringClientData(gmeWXT("prescale"))) ;
		/*
		{
			wxPGProperty* cat2 = pg->AppendIn( cat, new wxPropertyCategory(gmeWXT("reinhard032", wxPG_LABEL)) );
			pg->Append( new wxFloatProperty(gmeWXT("burn"), wxPG_LABEL) );
			pg->Append( new wxFloatProperty(gmeWXT("postscale"), wxPG_LABEL) );
			pg->Append( new wxFloatProperty(gmeWXT("prescale"), wxPG_LABEL) );
		}
		*/
	}


}


//
// Handle events of the third page here.
class wxMyPropertyGridPage : public wxPropertyGridPage
{
public:

    // Return false here to indicate unhandled events should be
    // propagated to manager's parent, as normal.
    virtual bool IsHandlingAllEvents() const { return false; }

protected:

    virtual wxPGProperty* DoInsert( wxPGProperty* parent,
                                    int index,
                                    wxPGProperty* property )
    {
        return wxPropertyGridPage::DoInsert(parent,index,property);
    }

    void OnPropertySelect( wxPropertyGridEvent& event );
    void OnPropertyChanging( wxPropertyGridEvent& event );
    void OnPropertyChange( wxPropertyGridEvent& event );
    void OnPageChange( wxPropertyGridEvent& event );

private:
    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(wxMyPropertyGridPage, wxPropertyGridPage)
    EVT_PG_SELECTED( wxID_ANY, wxMyPropertyGridPage::OnPropertySelect )
    EVT_PG_CHANGING( wxID_ANY, wxMyPropertyGridPage::OnPropertyChanging )
    EVT_PG_CHANGED( wxID_ANY, wxMyPropertyGridPage::OnPropertyChange )
    EVT_PG_PAGE_CHANGED( wxID_ANY, wxMyPropertyGridPage::OnPageChange )
END_EVENT_TABLE()


void wxMyPropertyGridPage::OnPropertySelect( wxPropertyGridEvent& WXUNUSED(event) )
{
	DECLARE_WXCONVERT;
    wxLogDebug(gmeWXT("wxMyPropertyGridPage::OnPropertySelect()"));
}

void wxMyPropertyGridPage::OnPropertyChange( wxPropertyGridEvent& event )
{
	DECLARE_WXCONVERT;
    wxPGProperty* p = event.GetProperty();
    wxLogVerbose(gmeWXT("wxMyPropertyGridPage::OnPropertyChange('%s', to value '%s')"),
               p->GetName().c_str(),
               p->GetDisplayedString().c_str());
}

void wxMyPropertyGridPage::OnPropertyChanging( wxPropertyGridEvent& event )
{
	DECLARE_WXCONVERT;
    wxPGProperty* p = event.GetProperty();
    wxLogVerbose(gmeWXT("wxMyPropertyGridPage::OnPropertyChanging('%s', to value '%s')"),
               p->GetName().c_str(),
               event.GetValue().GetString().c_str());
}

void wxMyPropertyGridPage::OnPageChange( wxPropertyGridEvent& WXUNUSED(event) )
{
	DECLARE_WXCONVERT;
    wxLogDebug(gmeWXT("wxMyPropertyGridPage::OnPageChange()"));
}


class wxPGKeyHandler : public wxEvtHandler
{
public:

    void OnKeyEvent( wxKeyEvent& event )
    {
		DECLARE_WXCONVERT;
        wxMessageBox(wxString::Format(gmeWXT("%i"),event.GetKeyCode()));
        event.Skip();
    }
private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxPGKeyHandler,wxEvtHandler)
    EVT_KEY_DOWN( wxPGKeyHandler::OnKeyEvent )
END_EVENT_TABLE()


// -----------------------------------------------------------------------

void PropFrame::InitPanel()
{
    if ( m_panel )
        m_panel->Destroy();

    wxWindow* panel = new wxPanel(this, wxID_ANY,
                                  wxDefaultPosition, wxSize(400, 400),
                                  wxTAB_TRAVERSAL);
    m_panel = panel;

    // Column
    wxBoxSizer* topSizer = new wxBoxSizer ( wxVERTICAL );

    m_topSizer = topSizer;

}

void PropFrame::FinalizePanel( bool wasCreated )
{

    m_panel->SetSizer( m_topSizer );
    m_topSizer->SetSizeHints( m_panel );

    wxBoxSizer* panelSizer = new wxBoxSizer( wxHORIZONTAL );
    panelSizer->Add( m_panel, 1, wxEXPAND|wxFIXED_MINSIZE );

    SetSizer( panelSizer );
    panelSizer->SetSizeHints( this );

    if ( wasCreated )
        FinalizeFramePosition();
}

void PropFrame::PopulateGrid()
{
	DECLARE_WXCONVERT;
    wxPropertyGridManager* pgman = m_pPropGridManager;

    pgman->AddPage(gmeWXT("Film Setting"));

    PopulateWithFilm();

    // Use wxMyPropertyGridPage (see above) to test the
    // custom wxPropertyGridPage feature.
    pgman->AddPage(gmeWXT("Scene Setting"));

    PopulateWithScene();

}

void PropFrame::CreateGrid( int style, int extraStyle )
{
    //
    // This function (re)creates the property grid in our sample
    //

    if ( style == -1 )
        style = // default style
                wxPG_BOLD_MODIFIED |
                wxPG_SPLITTER_AUTO_CENTER |
                wxPG_AUTO_SORT |
                //wxPG_HIDE_MARGIN|wxPG_STATIC_SPLITTER |
                //wxPG_TOOLTIPS |
                //wxPG_HIDE_CATEGORIES |
                //wxPG_LIMITED_EDITING |
                wxPG_TOOLBAR |
                wxPG_DESCRIPTION;

    if ( extraStyle == -1 )
        // default extra style
        extraStyle = wxPG_EX_MODE_BUTTONS |
                     wxPG_EX_MULTIPLE_SELECTION;
                //| wxPG_EX_AUTO_UNSPECIFIED_VALUES
                //| wxPG_EX_GREY_LABEL_WHEN_DISABLED
                //| wxPG_EX_NATIVE_DOUBLE_BUFFERING
                //| wxPG_EX_HELP_AS_TOOLTIPS

    bool wasCreated = m_panel ? false : true;

    InitPanel();

    //
    // This shows how to combine two static choice descriptors
    //m_combinedFlags.Add( _fs_windowstyle_labels, _fs_windowstyle_values );
    //m_combinedFlags.Add( _fs_framestyle_labels, _fs_framestyle_values );

    wxPropertyGridManager* pgman = m_pPropGridManager =
        new wxPropertyGridManager(m_panel,
                                  // Don't change this into wxID_ANY in the sample, or the
                                  // event handling will obviously be broken.
                                  PGID, /*wxID_ANY*/
                                  wxDefaultPosition,
                                  wxSize(100, 100), // FIXME: wxDefaultSize gives assertion in propgrid.
                                                    // But calling SetInitialSize in manager changes the code
                                                    // order to the grid gets created immediately, before SetExtraStyle
                                                    // is called.
                                  style );

    m_propGrid = pgman->GetGrid();

    pgman->SetExtraStyle(extraStyle);

    // This is the default validation failure behaviour
    m_pPropGridManager->SetValidationFailureBehavior( wxPG_VFB_MARK_CELL |
                                                      wxPG_VFB_SHOW_MESSAGEBOX );

    m_pPropGridManager->GetGrid()->SetVerticalSpacing( 2 );

    //
    // Set somewhat different unspecified value appearance
    wxPGCell cell;
    cell.SetText("Unspecified");
    cell.SetFgCol(*wxLIGHT_GREY);
    m_propGrid->SetUnspecifiedValueAppearance(cell);

    PopulateGrid();

    // Change some attributes in all properties
    //pgman->SetPropertyAttributeAll(wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING,true);
    //pgman->SetPropertyAttributeAll(wxPG_BOOL_USE_CHECKBOX,true);

    //m_pPropGridManager->SetSplitterLeft(true);
    //m_pPropGridManager->SetSplitterPosition(137);

    /*
    // This would setup event handling without event table entries
    Connect(m_pPropGridManager->GetId(), wxEVT_PG_SELECTED,
            wxPropertyGridEventHandler(PropFrame::OnPropertyGridSelect) );
    Connect(m_pPropGridManager->GetId(), wxEVT_PG_CHANGED,
            wxPropertyGridEventHandler(PropFrame::OnPropertyGridChange) );
    */

    m_topSizer->Add( m_pPropGridManager, 1, wxEXPAND );

    FinalizePanel(wasCreated);
}

// -----------------------------------------------------------------------

PropFrame::PropFrame(wxFrame *parent, const wxString& title, const wxPoint& pos, const wxSize& size, const long& style) :
           wxFrame(parent, -1, title, pos, size, style)
{
    //SetIcon(wxICON(sample));

    m_propGrid = NULL;
    m_panel = NULL;

#if wxUSE_IMAGE
    // This is here to really test the wxImageFileProperty.
    wxInitAllImageHandlers();
#endif

    // Register all editors (SpinCtrl etc.)
    m_pPropGridManager->RegisterAdditionalEditors();

    // Register our sample custom editors
//    m_pSampleMultiButtonEditor =
//        wxPropertyGrid::RegisterEditorClass(new wxSampleMultiButtonEditor());

    CreateGrid( // style
                wxPG_BOLD_MODIFIED |
                wxPG_SPLITTER_AUTO_CENTER |
                wxPG_AUTO_SORT |
                //wxPG_HIDE_MARGIN|wxPG_STATIC_SPLITTER |
                //wxPG_TOOLTIPS |
                //wxPG_HIDE_CATEGORIES |
                //wxPG_LIMITED_EDITING |
                wxPG_TOOLBAR |
                wxPG_DESCRIPTION,
                // extra style
                wxPG_EX_MODE_BUTTONS |
                wxPG_EX_MULTIPLE_SELECTION
                //| wxPG_EX_AUTO_UNSPECIFIED_VALUES
                //| wxPG_EX_GREY_LABEL_WHEN_DISABLED
                //| wxPG_EX_NATIVE_DOUBLE_BUFFERING
                //| wxPG_EX_HELP_AS_TOOLTIPS
              );

#if wxUSE_STATUSBAR
    // create a status bar
    CreateStatusBar(1);
    SetStatusText(wxEmptyString);
#endif // wxUSE_STATUSBAR
	updateDate();

}

void PropFrame::updateDate()
{
	// update property date from doc
}

void PropFrame::FinalizeFramePosition()
{
    wxSize frameSize((wxSystemSettings::GetMetric(wxSYS_SCREEN_X)/10)*4,
                     (wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)/10)*8);

    if ( frameSize.x > 500 )
        frameSize.x = 500;

    SetSize(frameSize);

    Centre();

}

//
// Normally, wxPropertyGrid does not check whether item with identical
// label already exists. However, since in this sample we use labels for
// identifying properties, we have to be sure not to generate identical
// labels.
//
void GenerateUniquePropertyLabel( wxPropertyGridManager* pg, wxString& baselabel )
{
	DECLARE_WXCONVERT;
    int count = -1;
    wxString newlabel;

    if ( pg->GetPropertyByLabel( baselabel ) )
    {
        for (;;)
        {
            count++;
            newlabel.Printf(gmeWXT("%s%i"),baselabel.c_str(),count);
            if ( !pg->GetPropertyByLabel( newlabel ) ) break;
        }
    }

    if ( count >= 0 )
    {
        baselabel = newlabel;
    }
}

PropFrame::~PropFrame()
{
}

void PropFrame::OnIdle( wxIdleEvent& event )
{
    /*
    // This code is useful for debugging focus problems
    static wxWindow* last_focus = (wxWindow*) NULL;

    wxWindow* cur_focus = ::wxWindow::FindFocus();

    if ( cur_focus != last_focus )
    {
        const wxChar* class_name = gmeWXT("<none>");
        if ( cur_focus )
            class_name = cur_focus->GetClassInfo()->GetClassName();
        last_focus = cur_focus;
        wxLogDebug( gmeWXT("FOCUSED: %s %X"),
            class_name,
            (unsigned int)cur_focus);
    }
    */

    event.Skip();
}


} //
