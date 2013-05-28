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
		this->Hide();
	}
	~SliderWindow(){};
	
	void setSliderInfo(wxPropertyGrid*  propgrid, double maxValue, double tick, double value, const wxPoint& pos, const wxSize&    size )
	{
		m_maxValue = maxValue;
		m_tick = tick;
		if(value > maxValue)
			value = maxValue;
		this->SetMax((int)(maxValue / tick));
		this->SetValue((int)(value / tick));
		// set positon and size
		this->SetSize(size.x - 50, size.y);
		this->SetPosition(wxPoint(pos.x + 50, pos.y));

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
private:
	double			m_maxValue;
	double			m_tick;
};

// 滑块editor
class wxPGSliderEditor : public wxPGEditor 
{
  DECLARE_DYNAMIC_CLASS(wxPGSliderEditor)
private:
	wxPGSliderEditor(){};
	wxPGSliderEditor(wxPropertyGrid*  propgrid){
		m_slider = new SliderWindow(propgrid, 0, 0, 100);
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
private:
	static wxPGEditor *m_instance;
	SliderWindow	  *m_slider;
};

}

#endif //GME_PROPERTY_PGEDITOR_H
