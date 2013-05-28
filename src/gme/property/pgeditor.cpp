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

#include "wx/wx.h"

// Main propertygrid header.
#include <wx/propgrid/propgrid.h>

#include "pgeditor.h"

namespace gme
{

wxPGEditor*	wxPGSliderEditor::m_instance = NULL;
//----------------- wxPGSliderEditor ---------------------

IMPLEMENT_DYNAMIC_CLASS(wxPGSliderEditor, wxPGEditor)

wxPGWindowList wxPGSliderEditor::CreateControls( wxPropertyGrid*  propgrid,
                                                 wxPGProperty*    property,
                                                 const wxPoint&   pos,
                                                 const wxSize&    size ) const
{
	double value = property->GetValue().GetDouble();
	// 设置maxValue和tick为固定值
	double maxValue;
	double tick;
	
	getSliderInfo(propgrid, maxValue, tick);
	//SliderWindow* slider = new SliderWindow(propgrid, 0, 0, 100);
	m_slider->setSliderInfo(propgrid, maxValue, tick, value, pos, size);
	//wxPGWindowList wndList = wxPGWindowList(slider);
	//wxPGWindowList wndList = wxPGTextCtrlEditor::CreateControls
    //                        ( propgrid, property, pos,
    //                           size );
	wxPGWindowList wndList;
	wndList.SetSecondary(m_slider);
	return wndList;
}

void wxPGSliderEditor::UpdateControl ( wxPGProperty* property, wxWindow* wnd ) const
{
  //SliderWindow* ctrl = wxDynamicCast ( wnd, SliderWindow );
  if ( m_slider )
  {
		wxAny value = property->GetValue();
		if (!value.IsNull())
		{
	 		 double val = wxANY_AS(value, double);
			 m_slider->setRealValue ( val );

		}
  }
}

bool wxPGSliderEditor::OnEvent ( wxPropertyGrid*  propgrid, 
                                 wxPGProperty*    property,
                                 wxWindow*        wnd,
                                 wxEvent&         event ) const
{
	if(event.GetEventType() == wxEVT_SCROLL_CHANGED)
	{
		// Update the value    
		event.Skip();
		propgrid->EditorsValueWasModified();

		return true;
	}
	return false;
}

bool wxPGSliderEditor::GetValueFromControl ( wxVariant&     variant,
                                             wxPGProperty*  property,
                                             wxWindow*      wnd ) const
{
	//SliderWindow* ctrl = wxDynamicCast ( wnd, SliderWindow );
	if ( m_slider )
	{
		variant = wxVariant ( m_slider->getRealValue() );
	
		property->SetValue ( variant );
	}

	return true;
}

void
wxPGSliderEditor::SetValueToUnspecified ( wxPGProperty* property, wxWindow* wnd) const
{
	//SliderWindow* ctrl = wxDynamicCast ( wnd, SliderWindow );
	if ( m_slider )
	{
		m_slider->SetDefalutValue();
	}
}

wxPGEditor* 
wxPGSliderEditor::getInstance(wxPropertyGrid*  propgrid)
{
	if(m_instance == NULL)
	{
		wxPGSliderEditor *slider = new wxPGSliderEditor(propgrid);
		m_instance = wxPropertyGrid::DoRegisterEditorClass(slider, slider->getEditorName());
	}
	return m_instance;
}

}
