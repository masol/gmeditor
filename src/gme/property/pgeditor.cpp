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
#include "wx/wx.h"
#include <wx/propgrid/propgrid.h>
#include <boost/format.hpp>
#include "pgeditor.h"

namespace gme
{

wxPGEditor*	wxPGSliderEditor::m_instance = NULL;
//----------------- wxPGSliderEditor ---------------------

IMPLEMENT_DYNAMIC_CLASS(wxPGSliderEditor, wxPGTextCtrlEditor)

wxPGWindowList wxPGSliderEditor::CreateControls( wxPropertyGrid*  propgrid,
                                                 wxPGProperty*    property,
                                                 const wxPoint&   pos,
                                                 const wxSize&    size ) const
{
    double value = property->GetValue().GetDouble();
	// 设置maxValue和tick为固定值
    float max = (float)propgrid->GetPropertyAttribute(property,gme_wxPG_ATTR_MAX).GetDouble();
    float min = (float)propgrid->GetPropertyAttribute(property,gme_wxPG_ATTR_MIN).GetDouble();
    int  precision = propgrid->GetPropertyAttribute(property,gme_wxPG_FLOAT_PRECISION).GetInteger();

    int textWidth = SliderWindow::getTextWidth(size.x);

    SliderWindow    *silder = new SliderWindow(propgrid->GetPanel(),(float)value,min,max,precision,
        wxPoint(pos.x + textWidth + 2 ,pos.y),wxSize(size.x - textWidth - 2,size.y) );
	silder->SetBackgroundColour(propgrid->GetCellBackgroundColour());

    //SliderWindow* slider = new SliderWindow(propgrid, 0, 0, 100);
	//wxPGWindowList wndList = wxPGWindowList(slider);
	wxPGWindowList wndList = wxPGTextCtrlEditor::CreateControls
                            ( propgrid, property, pos,
                            wxSize(textWidth, size.y) );
	//wxPGWindowList wndList;
	wndList.SetSecondary(silder);
	return wndList;
}

wxTextCtrl*
wxPGSliderEditor::getTextCtrlFromSlider(SliderWindow *ctrl)const
{
    wxWindow *pSibling = ctrl->GetNextSibling();
    while(pSibling)
    {
        wxTextCtrl* pText = wxDynamicCast ( pSibling, wxTextCtrl );
        if(pText)
        {
            return pText;
        }
        pSibling = pSibling->GetNextSibling();
    }
    pSibling = ctrl->GetPrevSibling();
    while(pSibling)
    {
        wxTextCtrl* pText = wxDynamicCast ( pSibling, wxTextCtrl );
        if(pText)
        {
            return pText;
        }
        pSibling = pSibling->GetPrevSibling();
    }
    return NULL;
}


SliderWindow*
wxPGSliderEditor::getSliderFromTextCtrl(wxTextCtrl *ctrl)const
{
    wxWindow *pSibling = ctrl->GetNextSibling();
    while(pSibling)
    {
        SliderWindow* pslider = wxDynamicCast ( pSibling, SliderWindow );
        if(pslider)
        {
            return pslider;
        }
        pSibling = pSibling->GetNextSibling();
    }
    pSibling = ctrl->GetPrevSibling();
    while(pSibling)
    {
        SliderWindow* pslider = wxDynamicCast ( pSibling, SliderWindow );
        if(pslider)
        {
            return pslider;
        }
        pSibling = pSibling->GetPrevSibling();
    }
    return NULL;
}


void wxPGSliderEditor::UpdateControl ( wxPGProperty* property, wxWindow* wnd ) const
{
    //SliderWindow* ctrl = wxDynamicCast ( wnd, SliderWindow );
	wxTextCtrl* ctrl = wxDynamicCast ( wnd, wxTextCtrl );
	if(ctrl)
	{
		wxAny value = property->GetValue();
		if (!value.IsNull())
		{
            float dval = value.As<float>();
            SliderWindow* pslider = getSliderFromTextCtrl(ctrl);
            if(pslider)
            {
                pslider->setRealValue(dval);
            }
            std::string strval = boost::str(boost::format("%.4f") % dval);
			ctrl->SetValue(strval.c_str());
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
        SliderWindow* ctrl = wxDynamicCast ( wnd, SliderWindow );
        if(ctrl)
        {
		    // Update the value
		    event.Skip();
			double ctrlValue = ctrl->getRealValue();
			double propValue = property->GetValue().GetDouble();
			if(propValue != ctrlValue)
				property->SetValue ( ctrlValue );
		    //propgrid->EditorsValueWasModified();
		    return true;
        }
    }else if(event.GetEventType() == wxEVT_SIZE)
    {
        ///@todo process frame size.
	    //wxTextCtrl* ctrl = wxDynamicCast ( wnd, wxTextCtrl );
     //   SliderWindow*  pSlider = NULL;
     //   if(!ctrl)
     //   {
     //       pSlider = wxDynamicCast ( wnd, SliderWindow );
     //       if(pSlider)
     //       {
     //           ctrl = getTextCtrlFromSlider(pSlider);
     //       }
     //   }
     //   if(ctrl)
     //   {
     //       if(!pSlider)
     //           pSlider = getSliderFromTextCtrl(ctrl);
     //       if(pSlider)
     //       {
     //           wxSizeEvent *pEvent = wxDynamicCast( &event , wxSizeEvent );
     //           wxSize size = pEvent->GetSize();
     //           wxRect r = ctrl->GetRect();
     //           int textWidth = SliderWindow::getTextWidth(size.x);
     //           ctrl->SetSize(textWidth,size.y);
     //           pSlider->SetSize(size.x - textWidth - 2,size.y);
     //           event.Skip();
     //           return true;
     //       }
     //   }
    }else if(event.GetEventType() == wxEVT_MOVE)
    {
	    wxTextCtrl* ctrl = wxDynamicCast ( wnd, wxTextCtrl );
        SliderWindow*  pSlider = NULL;
        if(ctrl)
        {
            if(!pSlider)
                pSlider = getSliderFromTextCtrl(ctrl);
            if(pSlider)
            {
                wxMoveEvent *pEvent = wxDynamicCast( &event , wxMoveEvent );
                wxPoint pt = pEvent->GetPosition();
                wxPoint pt_orig = ctrl->GetPosition();
                wxSize size = ctrl->GetSize();
                int totalWidth_orig = size.x + pSlider->GetSize().x;
                ctrl->Move(pt.x,pt.y);
                int totalWidth = pt_orig.x - pt.x + totalWidth_orig;
                int textWidth = SliderWindow::getTextWidth(totalWidth);
                ctrl->SetSize(pt.x,pt.y, textWidth, size.y);
                pSlider->SetSize(pt.x + textWidth + 2, pt.y, totalWidth - textWidth - 2, size.y);
                event.Skip();
                //return true;
            }
        }
    }
	return wxPGTextCtrlEditor::OnEvent(propgrid, property, wnd, event);
}

bool wxPGSliderEditor::GetValueFromControl ( wxVariant&     variant,
                                             wxPGProperty*  property,
                                             wxWindow*      wnd ) const
{
	wxTextCtrl* ctrl = wxDynamicCast ( wnd, wxTextCtrl );
	if(ctrl)
	{
        wxAny value = property->GetValue();
		double val;
		ctrl->GetValue().ToDouble(&val);
		variant = wxVariant(val);
		property->SetValue ( variant );
	}
	return true;
}

void
wxPGSliderEditor::SetValueToUnspecified ( wxPGProperty* property, wxWindow* wnd) const
{
	//SliderWindow* ctrl = wxDynamicCast ( wnd, SliderWindow );
	//if ( m_slider )
	//{
	//	m_slider->SetDefalutValue();
	//}
}

wxPGEditor*
wxPGSliderEditor::getInstance(void)
{
	if(m_instance == NULL)
	{
		wxPGSliderEditor* sliderEditor = new wxPGSliderEditor();
		m_instance = wxPropertyGrid::DoRegisterEditorClass(sliderEditor, sliderEditor->getEditorName());
	}
	return m_instance;
}

}
