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
#include <boost/locale.hpp>
#include "dm/docimg.h"
#include "imgfileeditor.h"
#include "../filedialog.h"


namespace gme
{

IMPLEMENT_DYNAMIC_CLASS(gmeImageFileProperty, wxFileProperty)

gmeImageFileProperty::gmeImageFileProperty( const wxString& label, const wxString& name,
    const wxString& value )
    : wxFileProperty(label,name,value)
{
    SetAttribute( wxPG_FILE_WILDCARD, OpenImageDialog::getDefaultImageWildcard() );

    m_pBitmap = NULL;
    if(value.IsEmpty())
    {
        m_filepath.clear();
    }else{
        m_filepath = boost::locale::conv::utf_to_utf<char>(value.ToStdWstring());
    }
}

gmeImageFileProperty::~gmeImageFileProperty()
{
    if ( m_pBitmap )
        delete m_pBitmap;
    m_filepath.clear();
}

void gmeImageFileProperty::OnSetValue()
{
    wxFileProperty::OnSetValue();

    // Delete old image
    m_filepath.clear();
    wxDELETE(m_pBitmap);

    wxFileName filename = GetFileName();

    m_filepath = boost::locale::conv::utf_to_utf<char>(filename.GetFullPath().ToStdWstring());
//    std::cerr << "m_filepath=" << m_filepath <<std::endl;
}

wxSize gmeImageFileProperty::OnMeasureImage( int ) const
{
    return wxPG_DEFAULT_IMAGE_SIZE;
}

void gmeImageFileProperty::OnCustomPaint( wxDC& dc,
                                         const wxRect& rect,
                                         wxPGPaintData& )
{
    // Draw the thumbnail

    // Create the bitmap here because required size is not known in OnSetValue().
    if ( !m_pBitmap && !m_filepath.empty() )
    {
        DocImg  img;
        int w,h;
        const float *pixels;
        if(img.getImage(m_filepath,w,h,pixels))
        {
            wxImage image(w,h,false);
            unsigned char* data = image.GetData();
            int pitch = w * 3;

            for(int y = 0; y < h; y++)
            {
                unsigned char* line = &data[(h - y - 1) * pitch];
                const float* srcline;
                ///TODO:do nearest match resize here to improve effective.
                srcline = &pixels[y * w * 3];
                for(int x = 0; x < w; x++)
                {
                    unsigned char r,g,b;
                    int index = x * 3;
                    r = (unsigned char)(srcline[index] * 255 );
                    b = (unsigned char)(srcline[index + 2]* 255 );
                    g = (unsigned char)(srcline[index + 1]* 255 );

                    int c = x * 3;
                    line[c] = r;
                    line[c+1] = g;
                    line[c+2] = b;
                }
            }

            image.Rescale( rect.width, rect.height );
            m_pBitmap = new wxBitmap( image );
        }
//      wxImage    img; // intermediate thumbnail area
        // Create the image thumbnail
////            m_pImage->Rescale( rect.width, rect.height );
//            m_pBitmap = new wxBitmap( *m_pImage );
    }

    if(m_pBitmap)
    {
        dc.DrawBitmap( *m_pBitmap, rect.x, rect.y, false );
    }
    else
    {
        // No file - just draw a white box
        dc.SetBrush( *wxWHITE_BRUSH );
        dc.DrawRectangle ( rect );
    }
}


}

