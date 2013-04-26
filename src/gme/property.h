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

#ifndef GME_PROPERTY_H
#define GME_PROPERTY_H

#include <wx/propgrid/propgrid.h>
#include "dm/xmlutil.h"

namespace gme{

class PropertyBase
{
public:
	PropertyBase(){};
	~PropertyBase(){};
	
	virtual void create(wxPGProperty *parent, type_xml_node *pNode) = 0;
private:

};

class MaterialProperty : public PropertyBase
{
public:
	MaterialProperty();
	~MaterialProperty(){};
	
	void	 create(wxPGProperty *parent, type_xml_node *pNode);

private:
	wxPGChoices m_soc;
};

class TexProperty : public PropertyBase
{
public:
	TexProperty();
	~TexProperty(){};
	
	void create(wxPGProperty *parent, type_xml_node *pNode);
private:
	wxPGChoices m_soc;
};


}
#endif // GME_PROPERTY_H
