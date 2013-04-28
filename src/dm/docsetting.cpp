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
#include "dm/setting.h"
#include "dm/docsetting.h"
#include "slg/slg.h"
#include "docprivate.h"
#include "slgmaterial.h"
#include <boost/assert.hpp>


namespace gme
{

// get toneMap properties
type_xml_node*
DocSetting::getToneMap(type_xml_node &parent)
{
	type_xml_doc *pDoc = parent.document();
    BOOST_ASSERT_MSG(pDoc != NULL,"invalid node,must from doc");

	slg::RenderSession* session = pDocData->getSession();
    if(session && session->film)
    {
		slg::Film *film = session->film;
        type_xml_node * pSelf = pDoc->allocate_node(NS_RAPIDXML::node_element,"tonemap");
		parent.append_node(pSelf);

		int type = film->GetToneMapParams()->GetType();
		std::string typeName = getToneMapTypeNameByName(type);
		pSelf->append_attribute(pDoc->allocate_attribute(constDef::type,allocate_string(pDoc,typeName)));
		type_xml_node *pParams = pDoc->allocate_node(NS_RAPIDXML::node_element,allocate_string(pSelf->document(),typeName));
		pSelf->append_node(pParams);
		if(typeName == "linear")
		{
			slg::LinearToneMapParams *params = (slg::LinearToneMapParams *)film->GetToneMapParams()->Copy();
			pParams->append_attribute(allocate_attribute_withkey(pDoc,"scale",boost::lexical_cast<std::string>( params->scale )));
			delete params;
		}
		else if(typeName == "reinhard02")
		{
			slg::Reinhard02ToneMapParams *params = (slg::Reinhard02ToneMapParams *)film->GetToneMapParams()->Copy();
			pParams->append_attribute(allocate_attribute_withkey(pDoc,"burn",boost::lexical_cast<std::string>( params->burn )));
			pParams->append_attribute(allocate_attribute_withkey(pDoc,"postScale",boost::lexical_cast<std::string>( params->postScale )));
			pParams->append_attribute(allocate_attribute_withkey(pDoc,"preScale",boost::lexical_cast<std::string>( params->preScale )));
			delete params;
		}
		return pSelf;
	}
	return NULL;
}

std::string
DocSetting::getToneMapTypeNameByName(int type)
{
	std::string typeName;
	switch(type)
	{
	case slg::TONEMAP_NONE:
		typeName = "none";
		break;
	case slg::TONEMAP_LINEAR:
		typeName = "linear";
		break;
	case slg::TONEMAP_REINHARD02:
		typeName = "reinhard02";
		break;
	default:
		break;
	}
	return typeName;
}

bool
DocSetting::getLinearScale(float &ls)
{
	bool ret = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->film)
    {
		slg::Film *film = session->film;
		if(film->GetToneMapParams()->GetType() == slg::TONEMAP_LINEAR)
		{
			slg::LinearToneMapParams *params = (slg::LinearToneMapParams *)film->GetToneMapParams()->Copy();
			ls = params->scale;
			delete params;
			ret = true;
		}
	}
    return ret;
}

bool
DocSetting::setLinearScale(float ls)
{
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->film)
    {
		slg::Film *film = session->film;
		slg::LinearToneMapParams *params = (slg::LinearToneMapParams *)film->GetToneMapParams()->Copy();
		params->scale = ls;
		film->SetToneMapParams(*params);
		delete params;
		return true;
	}
    return false;
}

//==========================================================

bool
DocSettingHelper::getToneMap(boost::unordered_map< std::string, boost::unordered_map<std::string, std::string> > &propMap)
{
	type_xml_doc    xmldoc;
	type_xml_node * pFilm = xmldoc.allocate_node(NS_RAPIDXML::node_element,"film");
	xmldoc.append_node(pFilm);
	type_xml_node *pNode = m_docSetting.getToneMap(*pFilm);
	if(pNode)
	{
		m_nodeToProps.ToToneMapProps(pNode, propMap);
		return true;
	}
	return false;
}

bool
DocSettingHelper::setToneMapProperty(const std::string &propName,
									 const std::string &propValue)
{
	bool ret = false;
	if(propName == "film.tonemap.linear.scale")
	{
		float floatValue = boost::lexical_cast<float>(propValue);
		ret = m_docSetting.setLinearScale(floatValue);
	}
	return ret;
}

type_xml_node*
DocSettingHelper::getMaterial(const std::string &id, type_xml_node &parent)
{
	return	m_docMat.getMaterial(id, parent);
}

//==========================================================

void
XmlNodeToProps::ToToneMapProps(type_xml_node* pNode,
							   boost::unordered_map< std::string, boost::unordered_map<std::string, std::string> > &propMap)
{
	boost::unordered_map<std::string, std::string> valueMap;
	type_xml_attr* pType = pNode->first_attribute("type");

	std::string typeName = pType->value();

	type_xml_node *pParams = pNode->first_node(typeName.c_str());
	type_xml_attr *pParam;
	for(pParam = pParams->first_attribute(); pParam != NULL; pParam = pParam->next_attribute())
	{
		valueMap.insert( std::pair<std::string, std::string>(pParam->name(), pParam->value()) );
	}
	propMap.insert( std::pair<std::string, boost::unordered_map<std::string, std::string> >(typeName, valueMap) );

}

}
