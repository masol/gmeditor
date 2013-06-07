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

#ifndef  GME_PROPERTY_PGEDITOR_H
#define  GME_PROPERTY_PGEDITOR_H

namespace gme
{

class WXDLLIMPEXP_PROPGRID SliderWindow : public wxSlider
{
public:
	SliderWindow(wxPropertyGrid*  propgrid,
             int value,
             int minValue,
             int maxValue)
			 : wxSlider(propgrid->GetPanel(), wxID_ANY, value, minValue, maxValue,
                             wxDefaultPosition, wxDefaultSize,
                             wxSL_HORIZONTAL)
	{
		SetBackgroundColour(propgrid->GetCellBackgroundColour());
		m_textWidth = 50;
		this->Hide();
	}
	~SliderWindow(){};
	
	void setSliderInfo(wxPropertyGrid*  propgrid, wxPGProperty*    property, double maxValue, double tick, double value, const wxPoint& pos, const wxSize&    size )
	{
		m_maxValue = maxValue;
		m_tick = tick;
		m_property = property;
		if(value > maxValue)
			value = maxValue;
		this->SetMax((int)(maxValue / tick));
		this->SetValue((int)(value / tick));
		// set positon and size
		this->resize(size.x, size.y);
		this->SetPosition(wxPoint(pos.x + m_textWidth, pos.y));

		this->Show();
	}

	double getRealValue()
	{
		int value = this->GetValue();
		double retValue = value * m_tick;
		return retValue;
	}
	void setRealValue(double value)
	{
		int sliderValue = (int)(value / m_tick);
		this->SetValue(sliderValue);
	}

	void SetDefalutValue()
	{
		this->SetValue(0);
	}

	void resize(int w, int h)
	{
		this->SetSize(w - m_textWidth, h);
	}

	void clearSelected()
	{
		wxPropertyGrid *grid = m_property->GetGrid();
		grid->ClearSelection();
	}

	int getTextWidth()
	{
		return m_textWidth;
	}
private:
	double			m_maxValue;
	double			m_tick;
	wxPGProperty*   m_property;
	int				m_textWidth;
};

// 滑块editor
class wxPGSliderEditor : public wxPGTextCtrlEditor 
{
  DECLARE_DYNAMIC_CLASS(wxPGSliderEditor)
private:
	wxPGSliderEditor(){};
	wxPGSliderEditor(wxPropertyGrid*  propgrid){
		m_slider = new SliderWindow(propgrid, 0, 0, 100);
		m_sliderEditor = this;
	};
	
	~wxPGSliderEditor ()
	{
		std::cout << "~wxPGSliderEditor called" <<std::endl;
	}
	std::string getEditorName()
	{
		return "sliderEditor";
	}
	virtual wxPGWindowList CreateControls( wxPropertyGrid* propGrid,
										   wxPGProperty* property,
										   const wxPoint& pos,
										   const wxSize& sz ) const;

	void UpdateControl ( wxPGProperty* property, wxWindow* wnd) const;
	bool OnEvent ( wxPropertyGrid* propgrid, wxPGProperty* property, wxWindow* wnd, wxEvent& event) const;
	bool GetValueFromControl ( wxVariant& variant, wxPGProperty* property, wxWindow* wnd) const;
	void SetValueToUnspecified ( wxPGProperty* property, wxWindow* wnd) const;
	//void DrawValue ( wxDC& dc, const wxRect& rect, wxPGProperty* property, const wxString& text) const;

	bool getSliderInfo ( wxPropertyGrid* propgrid, double &max, double &tick) const
	{
		max = 10000;
		tick = 0.1;
		return true;
	};

public:
	static wxPGEditor* getInstance(wxPropertyGrid*  propgrid);
	static wxPGSliderEditor* getSliderEditor()
	{
		if(m_instance == NULL)
			return NULL;
		return wxDynamicCast ( m_instance, wxPGSliderEditor );
	};
	// 矫正位置
	void Finalize();
private:
	static wxPGEditor *m_instance;
	wxPGSliderEditor  *m_sliderEditor;
	SliderWindow	  *m_slider;
};

}

#endif //GME_PROPERTY_PGEDITOR_H
