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

#ifndef  GME_PROPERTY_GMEPROPPAGE_H
#define  GME_PROPERTY_GMEPROPPAGE_H

#include <wx/wx.h>
#include <wx/propgrid/manager.h>

namespace gme{

class GmePropPage : public wxPropertyGridPage
{
protected:
    typedef wxPropertyGridPage  inherit;
    bool    m_pageShown;
    inline  bool    isPagenShown(void)const
    {
        return m_pageShown;
    }
public:
    GmePropPage()
    {
        m_pageShown = false;
    }
    virtual ~GmePropPage(){}
    virtual void onPageHiden(void)
    {
        m_pageShown = false;
    }
    virtual void onPageShown(void)
    {
        m_pageShown = true;
    }
protected:
    virtual bool IsHandlingAllEvents() const { return false; }
};

}

#endif //GME_PROPERTY_GMEPROPPAGE_H
