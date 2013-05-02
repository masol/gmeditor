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
#include "materialpage.h"
#include "dm/docio.h"
#include "dm/docobj.h"
#include "dm/docmat.h"
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <wx/propgrid/advprops.h>
#include "../stringutil.h"


BEGIN_EVENT_TABLE(gme::MaterialPage, gme::MaterialPage::inherit)
    EVT_PG_SELECTED( wxID_ANY, gme::MaterialPage::OnPropertySelect )
    EVT_PG_CHANGING( wxID_ANY, gme::MaterialPage::OnPropertyChanging )
    EVT_PG_CHANGED( wxID_ANY, gme::MaterialPage::OnPropertyChange )
END_EVENT_TABLE()


namespace gme{

enum{
    IDT_TEXTURE,
    IDT_MATERIAL,
    IDT_NORMAL,
    IDT_MAX
};

static  inline int
getIdTypeFromName(const std::string &name)
{
    if(boost::starts_with(name,"t_"))
    {
        return IDT_TEXTURE;
    }else if(boost::starts_with(name,"m_"))
    {
        return IDT_MATERIAL;
    }
    return IDT_NORMAL;
}


void
MaterialPage::onDocumentItemSelected(const std::string &id)
{
    if(id != m_currentObject)
    {//刷新材质页面。
        clearPage();
        buildPage(id);
    }
}

void
MaterialPage::onDocumentItemDeselected(const std::string &id)
{
    if(m_currentObject == id)
    {
        clearPage();
    }
}

void
MaterialPage::clearPage(void)
{
    //清空当前页面。
    this->Clear();
    m_currentObject.clear();
}

void
MaterialPage::buildTextureChoice(wxPGChoices &soc)
{
    DECLARE_WXCONVERT;

    soc.Add( gmeWXT("constfloat1"),DocMat::CONST_FLOAT);
    soc.Add( gmeWXT("constfloat3"),DocMat::CONST_FLOAT3);
    soc.Add( gmeWXT("imagemap"),DocMat::IMAGEMAP);
    soc.Add( gmeWXT("scale"),DocMat::SCALE_TEX);
    soc.Add( gmeWXT("fresnelapproxn"),DocMat::FRESNEL_APPROX_N);
    soc.Add( gmeWXT("fresnelapproxk"),DocMat::FRESNEL_APPROX_K);
    soc.Add( gmeWXT("mix"),DocMat::MIX_TEX);
    soc.Add( gmeWXT("add"),DocMat::ADD_TEX);
    soc.Add( gmeWXT("checkerboard2d"),DocMat::CHECKERBOARD2D);
    soc.Add( gmeWXT("checkerboard3d"),DocMat::CHECKERBOARD3D);
    soc.Add( gmeWXT("fbm"),DocMat::FBM_TEX);
    soc.Add( gmeWXT("marble"),DocMat::MARBLE);
    soc.Add( gmeWXT("dots"),DocMat::DOTS);
    soc.Add( gmeWXT("brick"),DocMat::BRICK);
    soc.Add( gmeWXT("windy"),DocMat::WINDY);
    soc.Add( gmeWXT("wrinkled"),DocMat::WRINKLED);
    soc.Add( gmeWXT("uv"),DocMat::UV_TEX);
    soc.Add( gmeWXT("band"),DocMat::BAND_TEX);
}

std::string
MaterialPage::getNameFromTagName(const std::string &tag)
{
    if(boost::iequals(tag,constDef::normaltex))
    {
        return __("法线贴图");
    }else if(boost::iequals(tag,constDef::bumptex))
    {
        return __("凹凸贴图");
    }else if(boost::iequals(tag,constDef::emission))
    {
        return __("光照贴图");
    }else if(boost::iequals(tag,constDef::kd))
    {
        return __("固有色");
    }else if(boost::iequals(tag,constDef::kr))
    {
        return __("反射系数");
    }
    return tag;
}


void
MaterialPage::addTexture(wxPGProperty &parent,type_xml_node *pParent,const std::string &childTag,int flag)
{
    DECLARE_WXCONVERT;
//build texture.
    type_xml_node   *pSelf = pParent->first_node(childTag.c_str());

    wxPGChoices soc;
    buildTextureChoice(soc);
    if(flag & TEX_HAS_DISABLE)
        soc.Add( gmeWXT("disable"),TEX_DISABLE);
    if(flag & TEX_HAS_IES)
        soc.Add( gmeWXT("ies"),TEX_IES);

    //如果自身id不存在,则使用p_+randomid作为id.指示引用父id.
    std::string  id;
    int type = TEX_DISABLE;
    if(pSelf)
    {
        type_xml_attr   *pIdAttr = pSelf->first_attribute(constDef::id);
        if(pIdAttr)
        {
            id = pIdAttr->value();
        }

        type_xml_attr   *pTypeAttr = pSelf->first_attribute(constDef::type);
        if(pTypeAttr)
        {
            type = DocMat::texGetTypeFromTypeName(pTypeAttr->value());
        }
    }

    id = childTag;

    wxPGProperty* pTexType = new wxEnumProperty(gmeWXT(getNameFromTagName(childTag).c_str()),id, soc);
    this->SetPropertyValue(pTexType,type);
    this->AppendIn(&parent,pTexType);

    if(pSelf)
    {//开始向pTextype添加可选项。
        if(type == DocMat::CONST_FLOAT3)
        {
            wxColour    color(128,128,128);
            type_xml_attr *rAttr = pSelf->first_attribute("r");
            type_xml_attr *gAttr = pSelf->first_attribute("g");
            type_xml_attr *bAttr = pSelf->first_attribute("b");
            if(rAttr && gAttr && bAttr)
            {
                int r = (int)(boost::lexical_cast<float>(rAttr->value()) * 255.0f);
                int g = (int)(boost::lexical_cast<float>(gAttr->value()) * 255.0f);
                int b = (int)(boost::lexical_cast<float>(bAttr->value()) * 255.0f);
                color.Set(r,g,b);
            }
            wxPGProperty* pTexValue = new wxColourProperty(gmeWXT("颜色"),"value",color);
            this->AppendIn(pTexType,pTexValue);
        }else if(type == DocMat::CONST_FLOAT)
        {
            float   value = 0.5;
            type_xml_attr   *pValueAttr = pSelf->first_attribute("value");
            if(pValueAttr)
            {
                value = boost::lexical_cast<float>(pValueAttr->value());
            }
            wxFloatProperty *pTexValue = new wxFloatProperty(gmeWXT("值"),"value",value);
            this->AppendIn(pTexType,pTexValue);
        }else if(type == DocMat::IMAGEMAP)
        {
            type_xml_attr  *fileAttr = pSelf->first_attribute("file");
            if(fileAttr)
            {
            }else
            {//@TODO: add default image here.
                BOOST_ASSERT_MSG(false,"not implement");
            }
            wxImageFileProperty *pTexFile = new wxImageFileProperty(gmeWXT("文件"),"file",fileAttr->value());
            this->AppendIn(pTexType,pTexFile);
            ///@todo : add gain and gamma.
        }else{
            BOOST_ASSERT_MSG(false,"not implement!");
        }
    }
}


void
MaterialPage::buildMaterialChoice(wxPGChoices &soc)
{
    DECLARE_WXCONVERT;
    soc.Add( gmeWXT("matte"),DocMat::MATTE);
    soc.Add( gmeWXT("mirror"),DocMat::MIRROR);
    soc.Add( gmeWXT("glass"),DocMat::GLASS);
    soc.Add( gmeWXT("metal"),DocMat::METAL);
    soc.Add( gmeWXT("archglass"),DocMat::ARCHGLASS);
    soc.Add( gmeWXT("mix"),DocMat::MIX);
    soc.Add( gmeWXT("null"),DocMat::NULLMAT);
    soc.Add( gmeWXT("mattetranslucent"),DocMat::MATTETRANSLUCENT);
    soc.Add( gmeWXT("glossy2"),DocMat::GLOSSY2);
    soc.Add( gmeWXT("metal2"),DocMat::METAL2);
}

void
MaterialPage::addMaterialContent(wxPGProperty &matType,type_xml_node *pSelf,int type,const std::string &name)
{
    DECLARE_WXCONVERT;
    wxPGProperty* pMatName = new wxStringProperty(gmeWXT("名称"),"name", gmeWXT(name.c_str()));
    this->AppendIn(&matType,pMatName);

    if(type != DocMat::NULLMAT)
    {
        addTexture(matType,pSelf,constDef::emission,TEX_HAS_DISABLE | TEX_HAS_IES);
        addTexture(matType,pSelf,constDef::bumptex,TEX_HAS_DISABLE);
        addTexture(matType,pSelf,constDef::normaltex,TEX_HAS_DISABLE);
    }

    if(type == DocMat::MATTE)
    {
        addTexture(matType,pSelf,constDef::kd);
    }else if(type == DocMat::MIRROR)
    {
        addTexture(matType,pSelf,constDef::kr);
    }else if(type == DocMat::GLASS)
    {
        addTexture(matType,pSelf,constDef::kr);
        addTexture(matType,pSelf,constDef::kt);
        addTexture(matType,pSelf,constDef::ioroutside);
        addTexture(matType,pSelf,constDef::iorinside);
    }else if(type == DocMat::METAL)
    {
        addTexture(matType,pSelf,constDef::kr);
        addTexture(matType,pSelf,constDef::exp);
    }else if(type == DocMat::ARCHGLASS)
    {
        addTexture(matType,pSelf,constDef::kr);
        addTexture(matType,pSelf,constDef::kt);
        addTexture(matType,pSelf,constDef::ioroutside);
        addTexture(matType,pSelf,constDef::iorinside);
    }else if(type == DocMat::MIX)
    {
        bool    bAdded = false;
        type_xml_node   *pChildA = pSelf->first_node(constDef::material);
        if(pChildA)
        {
            type_xml_node *pChildB = pChildA->next_sibling(constDef::material);
            if(pChildB)
            {
                bool    bSwap = false;
                type_xml_attr *pAttr = pChildA->first_attribute(constDef::position);
                if(pAttr && boost::iequals(pAttr->value(),"B"))
                {
                    bSwap = true;
                }
                pAttr = pChildB->first_attribute(constDef::position);
                if(pAttr && boost::iequals(pAttr->value(),"A"))
                {
                    bSwap = true;
                }
                if(bSwap)
                {
                    type_xml_node   *pTemp = pChildA;
                    pChildA = pChildB;
                    pChildB = pTemp;
                }

                addMaterial(matType,pChildA,"materialA");
                addMaterial(matType,pChildB,"materialB");

                addTexture(matType,pSelf,constDef::amount);
                bAdded = true;
            }
        }
        BOOST_ASSERT_MSG(bAdded,"invalid xml format!");
    }else if(type == DocMat::NULLMAT)
    {
    }else if(type == DocMat::MATTETRANSLUCENT)
    {
        addTexture(matType,pSelf,constDef::kr);
        addTexture(matType,pSelf,constDef::kt);
    }else if(type == DocMat::GLOSSY2)
    {
        addTexture(matType,pSelf,constDef::kd);
        addTexture(matType,pSelf,constDef::ks);
        addTexture(matType,pSelf,constDef::uroughness);
        addTexture(matType,pSelf,constDef::vroughness);
        addTexture(matType,pSelf,constDef::ka);
        addTexture(matType,pSelf,constDef::d);
        addTexture(matType,pSelf,constDef::index);
        bool multibounce = false;
        type_xml_attr   *pAttr = pSelf->first_attribute("multibounce");
        if(pAttr && (boost::iequals(pAttr->value(),"true")))
        {
            multibounce = true;
        }
        wxPGProperty* pMultibouncePG = new wxBoolProperty(gmeWXT("多重反射"),"multibounce", multibounce);
        this->AppendIn(&matType,pMultibouncePG);
    }else if(type == DocMat::METAL2)
    {
        addTexture(matType,pSelf,constDef::uroughness);
        addTexture(matType,pSelf,constDef::vroughness);
        addTexture(matType,pSelf,constDef::n);
        addTexture(matType,pSelf,constDef::k);
    }else
    {
        BOOST_ASSERT_MSG(false,"unknow material type!");
    }
}

void
MaterialPage::addMaterial(wxPGProperty &parent,type_xml_node *pSelf,const std::string &origID)
{
    DECLARE_WXCONVERT;
    if(boost::iequals(pSelf->name(),constDef::material))
    {
        type_xml_attr   *pType = pSelf->first_attribute(constDef::type);
        type_xml_attr   *pId = pSelf->first_attribute(constDef::id);
        BOOST_ASSERT_MSG(pId,"found a material without id??");
        if(pType)
        {
            wxPGChoices soc;
            buildMaterialChoice(soc);

            std::string     id;

            if(origID.empty())
                id = pId->value();
            else
                id = origID;

            std::string     name;
            type_xml_attr   *pName = pSelf->first_attribute(constDef::name);
            if(pName)
            {
                name = pName->value();
            }
            if(name.empty())
            {
                name = "未命名";
            }

            int type = DocMat::getTypeFromTypeName(pType->value());

            wxPGProperty* pMatType = new wxEnumProperty(gmeWXT(name.c_str()),id, soc,type);
            //this->SetPropertyValue(pMatType,type);
            this->AppendIn(&parent,pMatType);

            this->addMaterialContent(*pMatType,pSelf,type,name);
        }
    }
}


void
MaterialPage::buildPage(const std::string &objid)
{
    DECLARE_WXCONVERT;
    DocObj   dobj;
    ObjectNode *pNode = dobj.getRootObject().findObject(objid,NULL);
    if(pNode)
    {
        DocMat  mat;
        type_xml_doc    doc;
        type_xml_node   *pXmlMat = mat.getMaterial(pNode->matid(),doc);
        if(pXmlMat)
        {
            std::string content("对象'"+pNode->name() + "'的材质定义:");
            wxPGProperty* pCate = this->Append(new wxPropertyCategory(gmeWXT(content.c_str()),"material.catogory"));
            addMaterial(*pCate,pXmlMat,"");
        }
    }
    m_currentObject = objid;
}


void
MaterialPage::onDocumentClosed(void)
{
    clearPage();
}

MaterialPage::MaterialPage()
{
    DocIO   dio;
    dio.onSceneClosed(boost::bind(&MaterialPage::onDocumentClosed,this));

    DocObj  dobj;
    dobj.onSelected(boost::bind(&MaterialPage::onDocumentItemSelected,this,_1));
    dobj.onDeselected(boost::bind(&MaterialPage::onDocumentItemDeselected,this,_1));

	// append film tonemap
//	wxPGProperty* pf = this->Append(new wxPropertyCategory(gmeWXT("材质定义"),gmeWXT("material")));
}


MaterialPage::~MaterialPage()
{
}


void MaterialPage::OnPropertySelect( wxPropertyGridEvent& WXUNUSED(event) )
{
    std::cerr << "MaterialPage::OnPropertySelect()" << std::endl;
}

void MaterialPage::OnPropertyChange( wxPropertyGridEvent& event )
{
    wxPGProperty* p = event.GetProperty();
    std::vector< std::string >    idArray;

    std::string name(p->GetName().c_str());
    boost::split(idArray,name,boost::is_any_of("."),boost::token_compress_on);

    DocMat  mat;
    type_xml_doc    doc;
    std::string     value;
//    if(p->IsKindOf(&wxEnumProperty::ms_classInfo))
//    {
//        //value = boost::lexical_cast<std::string>(dynamic_cast<wxEnumProperty*>(p)->GetChoiceSelection());
//    }else
    if(p->IsKindOf(&wxColourProperty::ms_classInfo))
    {
        const wxColor &c = dynamic_cast<wxColourProperty*>(p)->GetVal().m_colour;
        value = boost::str(boost::format("%f %f %f") % ((float)c.Red()/255.0f)  % ((float)c.Green()/255.0f)  % ((float)c.Blue()/255.0f) );
    }else{
        value =  p->GetValue().GetString().c_str();
    }

    std::cerr << "value=" << value << std::endl;

    int result = mat.updateProperty(idArray,value,doc);
    switch(result)
    {
    case DocMat::UPDATE_DENY:
        std::cerr << "veto property changed." << std::endl;
        if(event.CanVeto())
            event.Veto();
        break;
    case DocMat::UPDATE_ACCEPT:
        std::cerr << "property changed accepted." << std::endl;
        break;
    case DocMat::UPDATE_REFRESH_MAT:
        std::cerr << "property changed need refresh." << std::endl;
        break;
    }


    std::cerr << "MaterialPage::OnPropertyChange('" << p->GetName().c_str() << "', to value '" << p->GetDisplayedString().c_str() << "')" << std::endl;
}

void MaterialPage::OnPropertyChanging( wxPropertyGridEvent& event )
{
    wxPGProperty* p = event.GetProperty();
    std::cerr << "MaterialPage::OnPropertyChanging('" << p->GetName().c_str() << "', to value '" << event.GetValue().GetString().c_str() << "')" << std::endl;
}


}

