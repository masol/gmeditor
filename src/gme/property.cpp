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

#include "property.h"
#include "config.h"
#include "stringutil.h"
#include <boost/lexical_cast.hpp>
// Extra property classes.
#include <wx/propgrid/advprops.h>

namespace gme{

MaterialProperty::MaterialProperty()
{
	DECLARE_WXCONVERT;
	m_soc.Add( gmeWXT("matte"));
	m_soc.Add( gmeWXT("mirror"));
	m_soc.Add( gmeWXT("metal"));
	m_soc.Add( gmeWXT("glass"));
	m_soc.Add( gmeWXT("archglass"));
	m_soc.Add( gmeWXT("mix"));
	m_soc.Add( gmeWXT("null"));
	m_soc.Add( gmeWXT("mattetranslucent"));
	m_soc.Add( gmeWXT("glossy2"));
	m_soc.Add( gmeWXT("metal2"));
}

void
MaterialProperty::create(wxPGProperty *parent, type_xml_node *pNode)
{
	DECLARE_WXCONVERT;
	std::string typeName = pNode->name();
	if(typeName != "material")
		BOOST_ASSERT_MSG(typeName != "material","node is not material");
	// 如果材质没有名字，则将id作为名字
	type_xml_attr *pNameAttr = pNode->first_attribute("name");
	std::string matName;
	if(pNameAttr == NULL)
	{
		type_xml_attr *pIdAttr = pNode->first_attribute("id");
		matName = pIdAttr->value();
	}
	else
		matName = pNameAttr->value();
	type_xml_attr *pTypeAttr  = pNode->first_attribute("type");
	std::string type = pTypeAttr->value();
	std::string propId = "scene.materials." + matName;
	wxPGProperty* pSelf = new wxEnumProperty(gmeWXT(matName.c_str()),gmeWXT(propId.c_str()), m_soc);
	pSelf->SetClientObject(new wxStringClientData(gmeWXT("type")));
	pSelf->SetValue(gmeWXT(type.c_str()));
	// @todo need set common property
	// set material property
	if(type == "matte")
	{
		type_xml_node *pKd = pNode->first_node("kd");
		TexProperty texProp;
		texProp.create(pSelf, pKd);
	}
	else if(type == "mirror")
	{
		type_xml_node *pKr = pNode->first_node("kr");
		TexProperty texProp;
		texProp.create(pSelf, pKr);
	}
	else if(type == "metal")
	{
	}
	else if(type == "glass")
	{
	}
	else if(type == "archglass")
	{
	}
	else if(type == "mix")
	{
		// child node is material
		type_xml_node *pChild = pNode->first_node();
		for(; pChild != NULL; pChild = pChild->next_sibling())
		{
			std::string nodeName = pChild->name();
			if(nodeName == "material")
			{
				MaterialProperty childMat;
				childMat.create(pSelf, pChild);
			}
			if(nodeName == "amount")
			{
				TexProperty texProp;
				texProp.create(pSelf, pChild);
			}
		}
	}
	else if(type == "null")
	{
	}
	else if(type == "mattetranslucent")
	{
	}
	else if(type == "glossy2")
	{
	}
	else if(type == "metal2")
	{
	}
	// put material to parent
	parent->AppendChild(pSelf);
}

// ===================================================================
// TexProperty
TexProperty::TexProperty()
{
	DECLARE_WXCONVERT;
	m_soc.Add( gmeWXT("constfloat1"));
	m_soc.Add( gmeWXT("constfloat3"));
	m_soc.Add( gmeWXT("imagemap"));
	m_soc.Add( gmeWXT("scale"));
	m_soc.Add( gmeWXT("fresnelapproxn"));
	m_soc.Add( gmeWXT("fresnelapproxk"));
	m_soc.Add( gmeWXT("mix"));
	m_soc.Add( gmeWXT("add"));
	m_soc.Add( gmeWXT("checkerboard2d"));
	m_soc.Add( gmeWXT("checkerboard3d"));
	m_soc.Add( gmeWXT("fbm"));
	m_soc.Add( gmeWXT("marble"));
	m_soc.Add( gmeWXT("dots"));
	m_soc.Add( gmeWXT("brick"));
	m_soc.Add( gmeWXT("windy"));
	m_soc.Add( gmeWXT("wrinkled"));
	m_soc.Add( gmeWXT("uv"));
	m_soc.Add( gmeWXT("band"));
}

void
TexProperty::create(wxPGProperty *parent, type_xml_node *pNode)
{
	DECLARE_WXCONVERT;
	// @todo:根据数据类型来显示,暂时都以字符串形式显示
	std::string name = pNode->name();
	type_xml_attr *pTypeAttr  = pNode->first_attribute("type");
	std::string type = pTypeAttr->value();

	wxPGProperty* pSelf = new wxEnumProperty(name.c_str(),name.c_str(), m_soc);
	pSelf->SetClientObject(new wxStringClientData(gmeWXT("type")));
	pSelf->SetValue(gmeWXT(type.c_str()));

	wxPGProperty* pParam;
	std::string value;
	if(type == "constfloat3")
	{
		type_xml_attr *pRAttr = pNode->first_attribute("r");
		type_xml_attr *pGAttr = pNode->first_attribute("g");
		type_xml_attr *pBAttr = pNode->first_attribute("b");

		std::string rValue = pRAttr->value();
		std::string gValue = pGAttr->value();
		std::string bValue = pBAttr->value();
		unsigned int r = boost::lexical_cast<float>(rValue) * 255;
		unsigned int g = boost::lexical_cast<float>(gValue) * 255;
		unsigned int b = boost::lexical_cast<float>(bValue) * 255;

		pParam = new wxColourProperty(gmeWXT("value"),gmeWXT("value"), wxColour(r, g, b));
 	}
	else if(type == "constfloat1")
	{
		type_xml_attr *pValueAttr = pNode->first_attribute("value");
		value = pValueAttr->value();
		pParam = new wxStringProperty(gmeWXT("value"),gmeWXT("value"), value);
	}

	pSelf->AppendChild(pParam);
	// put tex to parent
	parent->AppendChild(pSelf);

}

// ===================================================================


}
