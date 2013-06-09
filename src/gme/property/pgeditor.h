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

#define gme_wxPG_ATTR_MIN              wxS("GME_Min")
#define gme_wxPG_ATTR_MAX              wxS("GME_Max")
#define gme_wxPG_FLOAT_PRECISION       wxS("GME_Precision")


namespace gme
{

class WXDLLIMPEXP_PROPGRID SliderWindow : public wxSlider
{
public:
	SliderWindow(wxWindow*  parent,float value,float minValue,
             float maxValue,int precision,const wxPoint &position = wxDefaultPosition,
             const wxSize  &size = wxDefaultSize)
			 : wxSlider(parent, wxID_ANY, value * precision,
                          minValue * precision, maxValue * precision,
                          position, size,wxSL_HORIZONTAL)
	{
        m_precision = precision;
		//SetBackgroundColour(propgrid->GetCellBackgroundColour());
	}

	~SliderWindow(){}

    inline float   getMaxValue(void)const
    {
        return ((float)this->GetMax() / (float)this->m_precision);
    }
    inline void   setMaxValue(float max)
    {
        this->SetMax(max * this->m_precision);
    }
    inline float   getMinValue(void)const
    {
        return ((float)this->GetMin() / (float)this->m_precision);
    }
    inline void   setMinValue(float min)
    {
        this->SetMin(min * this->m_precision);
    }
	inline float getRealValue()const
	{
		int value = this->GetValue();
		return ((float)value /(float) m_precision);
	}
	inline void setRealValue(float value)
	{
		int sliderValue = (int)(value * m_precision);
        //文本框能输入的范围要超过slider的范围。
        if(sliderValue > this->GetMax())
            sliderValue = this->GetMax();
        if(sliderValue < this->GetMin())
            sliderValue = this->GetMin();
		this->SetValue(sliderValue);
	}

    inline void resize(int w, int h)
	{
		this->SetSize(w - getTextWidth(w), h);
	}
	inline static int getTextWidth(int width)
	{
        int textWidth = 60;
        int maxTexWidth = ((float)width / 3.0f);
        if(textWidth > maxTexWidth)
        {
            textWidth = maxTexWidth;
        }
		return textWidth;
	}
private:
    int             m_precision;
};

// 滑块editor
///@todo 使用wxSizer改写PGSliderEditor.
class wxPGSliderEditor : public wxPGTextCtrlEditor
{
  DECLARE_DYNAMIC_CLASS(wxPGSliderEditor)
private:
	wxPGSliderEditor(){};

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

public:
	static wxPGEditor* getInstance(void);
	static wxPGSliderEditor* getSliderEditor()
	{
		if(m_instance == NULL)
			return NULL;
		return wxDynamicCast ( m_instance, wxPGSliderEditor );
	};
private:
    SliderWindow*   getSliderFromTextCtrl(wxTextCtrl *ctrl)const;
    wxTextCtrl*     getTextCtrlFromSlider(SliderWindow *ctrl)const;
	static wxPGEditor *m_instance;
};

}

#endif //GME_PROPERTY_PGEDITOR_H
