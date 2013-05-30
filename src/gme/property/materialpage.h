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

#ifndef  GME_PROPERTY_MATERIALPAGE_H
#define  GME_PROPERTY_MATERIALPAGE_H

#include <wx/wx.h>
#include <boost/lexical_cast.hpp>
#include "gmeproppage.h"
#include "dm/xmlutil.h"

namespace gme{

class MaterialPage : public GmePropPage
{
private:
    enum{
        TEX_HAS_DISABLE = 0x1,
        TEX_HAS_IES = 0X2,
        TEX_MAX_EXTEND
    };
    typedef GmePropPage  inherit;
    std::string         m_currentObject;
    void    clearPage(void);
    void    buildPage(const std::string &objid);
    void    addMaterial(wxPGProperty &parent,type_xml_node *pSelf,const std::string &origID);
    void    addMaterialContent(wxPGProperty &matType,type_xml_node *pSelf,int type,const std::string &name);
    void    buildMaterialChoice(wxPGChoices &soc);

    template<class T>
    T   GetAttributeValue(type_xml_node *pSelf,const char* tagName,T defaultVal)
    {
        type_xml_attr *pAttr = pSelf->first_attribute(tagName);
        if(pAttr)
        {
            return boost::lexical_cast<T>(pAttr->value());
        }
        return defaultVal;
    }

    void    addMapping2D(wxPGProperty *pTexType,type_xml_node *pSelf);
    void    addTextureContent(wxPGProperty *pTexType,type_xml_node *pSelf,int type);
    void    addTexture(wxPGProperty &parent,type_xml_node *pParent,const std::string &childTag,int flag = 0,const char* name = NULL);
    void    buildTextureChoice(wxPGChoices &soc);
    ///@brief 从tag返回一个友好的名称。
    std::string     getNameFromTagName(const std::string &tag);
public:
    MaterialPage();
    virtual ~MaterialPage();
protected:
    void OnPropertySelect( wxPropertyGridEvent& event );
    void OnPropertyChanging( wxPropertyGridEvent& event );
    void OnPropertyChange( wxPropertyGridEvent& event );
#ifdef PROPERTY_HAS_DELETE_PROPERTY
#else
    void OnRefreshMaterialProp(wxCommandEvent &event);
#endif

	void  onDocumentItemSelected(const std::string &id);
	void  onDocumentItemDeselected(const std::string &id);
	void  onDocumentClosed(void);
private:
    DECLARE_EVENT_TABLE()
};

}

#endif //GME_PROPERTY_MATERIALPAGE_H
