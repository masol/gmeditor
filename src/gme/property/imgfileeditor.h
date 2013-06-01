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

#ifndef  GME_PROPERTY_IMGFILEEDITOR_H
#define  GME_PROPERTY_IMGFILEEDITOR_H

#include <wx/wx.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/props.h>

namespace gme
{

class gmeImageFileProperty : public wxFileProperty
{
    DECLARE_DYNAMIC_CLASS(gmeImageFileProperty)
public:

    gmeImageFileProperty( const wxString& label= wxPG_LABEL,
        const wxString& name = wxPG_LABEL,
        const wxString& value = wxEmptyString);
    virtual ~gmeImageFileProperty();

    virtual void OnSetValue();

    virtual wxSize OnMeasureImage( int item ) const;
    virtual void OnCustomPaint( wxDC& dc,
        const wxRect& rect, wxPGPaintData& paintdata );

protected:
    std::string m_filepath; //store the file path to query imageddata from slg.
    wxBitmap*   m_pBitmap; // final thumbnail area
};

}


#endif //GME_PROPERTY_IMGFILEEDITOR_H
