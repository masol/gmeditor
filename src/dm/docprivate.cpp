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
#include "docprivate.h"
#include "dm/docmat.h"
#include "utils/MD5.h"


namespace gme{

static inline
std::string     getMaterialNameInSlg(const boost::uuids::uuid &id,ExtraMaterialInfo &matInfo)
{
    if(matInfo.m_slgname.length())
        return matInfo.m_slgname;
    return ObjectNode::idto_string(id);
}

static
void    writeTexture(std::ostream &o,const std::string &tag,const slg::Texture *pTex,MD5 &md5)
{
    o << "<" << tag;

    MD5 tex_md5;
    std::string texTypeName = DocMat::texGetTypeNameFromType(pTex->GetType());
    o << " type='" << texTypeName << "'";
    
    tex_md5.update(texTypeName.c_str(),texTypeName.length());
    switch(pTex->GetType())
    {
    case slg::CONST_FLOAT3:
        {
            slg::Spectrum color = dynamic_cast<const slg::ConstFloat3Texture*>(pTex)->GetColor();
            o << " r='" << boost::lexical_cast<std::string>( color.r ) << "'";
            o << " g='" << boost::lexical_cast<std::string>( color.g ) << "'";
            o << " b='" << boost::lexical_cast<std::string>( color.b ) << "'";
            tex_md5.update((unsigned char*)(void*)&color,sizeof(color));
        }
        break;
    case slg::CONST_FLOAT:
        break;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code");
    }
    
    std::string     tex_md5_value = tex_md5.finalize().hexdigest();
    md5.update(tex_md5_value.c_str(),tex_md5_value.length());
    o << " ctxmd5='" << tex_md5_value << "'>" << std::endl;
    o << "</" << tag << ">" << std::endl;
}

DocPrivate::DocPrivate(void)
{
    m_started = false;
}


DocPrivate::~DocPrivate(void)
{
    if(m_session)
    {
        m_session.reset();
    }
}

void
DocPrivate::WriteMaterialImpl(std::ofstream &o,const std::string &slgname,type_id2material_map::iterator &it)
{
    o << "<material";
    //没有给出it参数。说明这是一个mix材质。不输出id值。
    slg::Material *pMat;
    if(it != m_id2material_map.end())
    {
        o << " id='" << ObjectNode::idto_string(it->first) << "'";
        if(it->second.m_name.length())
        {
            o << " name='" << it->second.m_name << "'";
        }
        pMat = m_session->renderConfig->scene->matDefs.GetMaterial(getMaterialNameInSlg(it->first,it->second));
    }else{
        o << " name='" << slgname << "'";
        BOOST_ASSERT_MSG(slgname.length(),"materialInfoArray Panic!");
        pMat = m_session->renderConfig->scene->matDefs.GetMaterial(slgname);
    }
    BOOST_ASSERT_MSG(pMat != NULL,"materialInfoArray Panic!");
    
    std::string type = DocMat::getTypeNameFromType(pMat->GetType());
    o << " type='" << type << "'";
    
    MD5     md5;
    md5.update(type.c_str(),type.length());

    std::stringstream   ss;
    if(pMat->IsLightSource())
    {
        writeTexture(ss,"emission",pMat->GetEmitTexture(),md5);
    }
    
    if(pMat->HasBumpTex())
    {
        writeTexture(ss,"bumptex",pMat->GetBumpTexture(),md5);
    }

    if(pMat->HasNormalTex())
    {
        writeTexture(ss,"normaltex",pMat->GetNormalTexture(),md5);
    }
    
    switch(pMat->GetType())
    {
    case slg::MATTE:
        {
            const slg::Texture *pTex = dynamic_cast<slg::MatteMaterial*>(pMat)->GetKd ();
            if(pTex)
            {
                writeTexture(ss,"kd",pTex,md5);
            }
        }
        break;
    }

    o << " ctxmd5='" << md5.finalize().hexdigest() << "'>" << std::endl;
    o << ss.str();
    o << "</material>" << std::endl;
}


void
DocPrivate::WriteMaterial(std::ofstream &o)
{
    type_id2material_map::iterator  it = m_id2material_map.begin();
    while(it != m_id2material_map.end())
    {
        WriteMaterialImpl(o,it->second.m_slgname,it);
        it++;
    }
}


}


