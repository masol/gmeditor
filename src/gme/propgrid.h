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

#ifndef GME_PROPGRID_H
#define GME_PROPGRID_H

#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <wx/propgrid/propgrid.h>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

namespace gme{

class   GmePropPage;
class PropFrame : public wxScrolledWindow
{
public:
    PropFrame(wxFrame *parent, wxWindowID id, const wxPoint& pos, const wxSize& size , const long& style);
    ~PropFrame();

    wxPropertyGridManager   *m_pPropGridManager;
    GmePropPage             *m_pLastShownPage;
private:
    /// @brief set the default propertyview size.
    void setDefaultFramePosition();

//    enum{
//        PGID_POSTPROCESS = 0,
//        PGID_MATERIAL,
//        PGID_ENVIRONMENT,
//        PGID_MAX
//    };
    /// @brief all all pages here.
    void initPages(void);
protected:
    void OnPropertyGridPageChange( wxPropertyGridEvent& event );
    void OnSelectedObjectChanged(const std::string &oid,const std::string &matid)
    {
        ///@todo check visible here.
        //bugy... need fix it!
        //showMatProps(matid);
    }
private:
    DECLARE_EVENT_TABLE()
};

} //end namespace gme

#endif // GME_PROPGRID_H
