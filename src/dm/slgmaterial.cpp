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
#include "slgmaterial.h"
#include "slgtexture.h"
#include "utils/MD5.h"

namespace gme{

void
ExtraMaterialManager::appendMat2IdFromSlg(void)
{
    slg::Scene  *scene = Doc::instance().pDocData->getSession()->renderConfig->scene;
    std::vector<std::string> materialNameArray = scene->matDefs.GetMaterialNames ();
    u_int   nameIdx = 0;
    for(std::vector< std::string >::const_iterator it = materialNameArray.begin(); it < materialNameArray.end(); ++it,++nameIdx)
    {
        m_mat2id[scene->matDefs.GetMaterial(scene->matDefs.GetMaterialIndex(*it))] = nameIdx;
    }
}


SlgMaterial2Name::SlgMaterial2Name()
{
    slg::Scene  *scene = Doc::instance().pDocData->getSession()->renderConfig->scene;
    m_materialNameArray = scene->matDefs.GetMaterialNames ();
    m_matIdx2NameIdx.resize(m_materialNameArray.size());
    u_int   nameIdx = 0;
    for(std::vector< std::string >::const_iterator it = m_materialNameArray.begin(); it < m_materialNameArray.end(); ++it,++nameIdx)
    {
        m_matIdx2NameIdx[scene->matDefs.GetMaterialIndex(*it)] = nameIdx;
    }
}

const std::string&
SlgMaterial2Name::getMaterialName(const slg::Material* pmat)const
{
    slg::Scene  *scene = Doc::instance().pDocData->getSession()->renderConfig->scene;
    u_int matNameIdx = m_matIdx2NameIdx[scene->matDefs.GetMaterialIndex(pmat)];
    return m_materialNameArray[matNameIdx];
}

type_xml_node*
ExtraMaterialManager::dump(type_xml_node &parent,const slg::Material* pMat,dumpContext &ctx)
{
    type_xml_doc *pDoc = parent.document();
    BOOST_ASSERT_MSG(pDoc != NULL,"invalid node,must from doc");

    type_xml_node *pSelf = pDoc->allocate_node(NS_RAPIDXML::node_element,constDef::material);

    const std::string &id = this->getMaterialId(pMat);
    if(id.length())
    {
        const std::string  &name = this->get(id);
        if(name.length())
        {
            pSelf->append_attribute(pDoc->allocate_attribute(constDef::name,allocate_string(pDoc,name)));
        }
        //output id.
        pSelf->append_attribute(pDoc->allocate_attribute(constDef::id,allocate_string(pDoc,id)));
    }

    std::string type = DocMat::getTypeNameFromType(pMat->GetType());
    pSelf->append_attribute(pDoc->allocate_attribute(constDef::type,allocate_string(pDoc,type)));

    conditional_md5 md5(ctx.flags);
    md5.update(type.c_str(),type.length());
    ExtraTextureManager &texManager = Doc::instance().pDocData->texManager;

    if(pMat->IsLightSource())
    {
        texManager.dump(*pSelf,"emission",pMat->GetEmitTexture(),ctx,md5);
    }

    if(pMat->HasBumpTex())
    {
        texManager.dump(*pSelf,"bumptex",pMat->GetBumpTexture(),ctx,md5);
    }

    if(pMat->HasNormalTex())
    {
        texManager.dump(*pSelf,"normaltex",pMat->GetNormalTexture(),ctx,md5);
    }

    switch(pMat->GetType())
    {
    case slg::MATTE:
        {
            const slg::Texture *pTex = dynamic_cast<const slg::MatteMaterial*>(pMat)->GetKd ();
            if(pTex)
            {
                texManager.dump(*pSelf,"kd",pTex,ctx,md5);
            }
        }
        break;
    case slg::MIRROR:
        {
            const slg::Texture *pTex = dynamic_cast<const slg::MirrorMaterial*>(pMat)->GetKr();
            if(pTex)
            {
                texManager.dump(*pSelf,"kr",pTex,ctx,md5);
            }
        }
        break;
    case slg::GLASS:
        {
            const slg::GlassMaterial *pMaterial = dynamic_cast<const slg::GlassMaterial*>(pMat);
            if(pMaterial->GetKr())
            {
                texManager.dump(*pSelf,"kr",pMaterial->GetKr(),ctx,md5);
            }
            if(pMaterial->GetKt())
            {
                texManager.dump(*pSelf,"kt",pMaterial->GetKt(),ctx,md5);
            }
            if(pMaterial->GetOutsideIOR())
            {
                texManager.dump(*pSelf,"ioroutside",pMaterial->GetOutsideIOR(),ctx,md5);
            }
            if(pMaterial->GetIOR())
            {
                texManager.dump(*pSelf,"iorinside",pMaterial->GetIOR(),ctx,md5);
            }
        }
        break;
    case slg::METAL:
        {
            const slg::MetalMaterial *pMaterial = dynamic_cast<const slg::MetalMaterial*>(pMat);
            if(pMaterial->GetKr())
            {
                texManager.dump(*pSelf,"kr",pMaterial->GetKr(),ctx,md5);
            }
            if(pMaterial->GetExp())
            {
                texManager.dump(*pSelf,"exp",pMaterial->GetExp(),ctx,md5);
            }
        }
        break;
    case slg::ARCHGLASS:
        {
            const slg::ArchGlassMaterial *pMaterial = dynamic_cast<const slg::ArchGlassMaterial*>(pMat);
            if(pMaterial->GetKr())
            {
                texManager.dump(*pSelf,"kr",pMaterial->GetKr(),ctx,md5);
            }
            if(pMaterial->GetKt ())
            {
                texManager.dump(*pSelf,"kt",pMaterial->GetKt(),ctx,md5);
            }
            if(pMaterial->GetOutsideIOR())
            {
                texManager.dump(*pSelf,"ioroutside",pMaterial->GetOutsideIOR(),ctx,md5);
            }
            if(pMaterial->GetIOR())
            {
                texManager.dump(*pSelf,"iorinside",pMaterial->GetIOR(),ctx,md5);
            }
        }
        break;
    case slg::MIX:
        {
            const slg::MixMaterial *pMaterial = dynamic_cast<const slg::MixMaterial*>(pMat);
            BOOST_ASSERT_MSG(pMaterial->GetMaterialA() && pMaterial->GetMaterialB(), "invalid mix material");

			type_xml_node* matA = dump(*pSelf,pMaterial->GetMaterialA(),ctx);
			type_xml_node* matB = dump(*pSelf,pMaterial->GetMaterialB(),ctx);
			md5.updateChild(matA);
			md5.updateChild(matB);

			//为了防止子节点没有id而搞乱位置，我们通过position来识别位置。
			matA->append_attribute(pDoc->allocate_attribute(constDef::position,allocate_string(pDoc,"A")));
			matB->append_attribute(pDoc->allocate_attribute(constDef::position,allocate_string(pDoc,"B")));


            if(pMaterial->GetMixFactor())
            {
				texManager.dump(*pSelf,"amount",pMaterial->GetMixFactor(),ctx,md5);
            }
        }
        break;
    case slg::NULLMAT:
        {
        }
        break;
    case slg::MATTETRANSLUCENT:
        {
            const slg::MatteTranslucentMaterial  *pMaterial = dynamic_cast<const slg::MatteTranslucentMaterial*>(pMat);
            if(pMaterial->GetKr())
            {
				texManager.dump(*pSelf,"kr",pMaterial->GetKr(),ctx,md5);
            }
            if(pMaterial->GetKt ())
            {
				texManager.dump(*pSelf,"kt",pMaterial->GetKt(),ctx,md5);
            }
        }
        break;
    case slg::GLOSSY2:
        {
            const slg::Glossy2Material  *pMaterial = dynamic_cast<const slg::Glossy2Material*>(pMat);
            if(pMaterial->GetKd())
            {
				texManager.dump(*pSelf,"kd",pMaterial->GetKd(),ctx,md5);
            }
            if(pMaterial->GetKs ())
            {
				texManager.dump(*pSelf,"ks",pMaterial->GetKs(),ctx,md5);
            }
            if(pMaterial->GetNu ())
            {
				texManager.dump(*pSelf,"uroughness",pMaterial->GetNu(),ctx,md5);
            }
            if(pMaterial->GetNv ())
            {
				texManager.dump(*pSelf,"vroughness",pMaterial->GetNv(),ctx,md5);
            }
            if(pMaterial->GetKa ())
            {
				texManager.dump(*pSelf,"ka",pMaterial->GetKa(),ctx,md5);
            }
            if(pMaterial->GetDepth ())
            {
				texManager.dump(*pSelf,"d",pMaterial->GetDepth(),ctx,md5);
            }
            if(pMaterial->GetIndex ())
            {
				texManager.dump(*pSelf,"index",pMaterial->GetIndex(),ctx,md5);
            }
			pSelf->append_attribute(pDoc->allocate_attribute(constDef::position,allocate_string(pDoc,(pMaterial->IsMultibounce() ? "true" : "false" ))));
        }
        break;
    case slg::METAL2:
        {
            const slg::Metal2Material  *pMaterial = dynamic_cast<const slg::Metal2Material*>(pMat);
            if(pMaterial->GetN())
            {
				texManager.dump(*pSelf,"n",pMaterial->GetN(),ctx,md5);
            }
            if(pMaterial->GetK())
            {
				texManager.dump(*pSelf,"k",pMaterial->GetK(),ctx,md5);
            }
            if(pMaterial->GetNu ())
            {
				texManager.dump(*pSelf,"uroughness",pMaterial->GetNu(),ctx,md5);
            }
            if(pMaterial->GetNv ())
            {
				texManager.dump(*pSelf,"vroughness",pMaterial->GetNv(),ctx,md5);
            }
        }
        break;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code");
    }

    if(md5.isGenerateMD5())
    {
        std::string     md5_value = md5.hexdigest();
        pSelf->append_attribute(allocate_attribute(pDoc,constDef::ctxmd5,md5_value));
    }

    parent.append_node(pSelf);
    return pSelf;
}

/*
std::string
ExtraMaterialManager::WriteMaterialImpl(MaterialWriteContext &ctx,
										const slg::Material *pMat,
										const boost::uuids::uuid *pId,
										const std::string &name,
										MD5 *ppmd5)
{
    std::ostream &o = ctx.stream();
	ctx.outIndent();
    o << "<material";
    //没有给出pId参数。说明这是一个mix材质，没有id值，使用md5作为id值。
    std::string     idstring;
    if(pId)
    {
        idstring = ObjectNode::idto_string(*pId);
        o << " id='" << idstring << "'";
    }
    BOOST_ASSERT_MSG(pMat != NULL,"materialInfoArray Panic!");
    if(name.length())
    {
        o << " name='" << name << "'";
    }


    std::string type = DocMat::getTypeNameFromType(pMat->GetType());
    o << " type='" << type << "'";

    MD5     md5;
    md5.update(type.c_str(),type.length());

    std::stringstream   ss;

    TextureWriteContextSetter   textCtxAssign(ctx.m_texCtx,ss,ctx.m_indent+1);

    if(pMat->IsLightSource())
    {
        ExtraTextureManager::writeTexture(ctx.m_texCtx,"emission",pMat->GetEmitTexture(),&md5);
    }

    if(pMat->HasBumpTex())
    {
        ExtraTextureManager::writeTexture(ctx.m_texCtx,"bumptex",pMat->GetBumpTexture(),&md5);
    }

    if(pMat->HasNormalTex())
    {
        ExtraTextureManager::writeTexture(ctx.m_texCtx,"normaltex",pMat->GetNormalTexture(),&md5);
    }

    switch(pMat->GetType())
    {
    case slg::MATTE:
        {
            const slg::Texture *pTex = dynamic_cast<const slg::MatteMaterial*>(pMat)->GetKd ();
            if(pTex)
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"kd",pTex,&md5);
            }
        }
        break;
    case slg::MIRROR:
        {
            const slg::Texture *pTex = dynamic_cast<const slg::MirrorMaterial*>(pMat)->GetKr();
            if(pTex)
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"kr",pTex,&md5);
            }
        }
        break;
    case slg::GLASS:
        {
            const slg::GlassMaterial *pMaterial = dynamic_cast<const slg::GlassMaterial*>(pMat);
            if(pMaterial->GetKr())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"kr",pMaterial->GetKr(),&md5);
            }
            if(pMaterial->GetKt())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"kt",pMaterial->GetKt(),&md5);
            }
            if(pMaterial->GetOutsideIOR())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"ioroutside",pMaterial->GetOutsideIOR(),&md5);
            }
            if(pMaterial->GetIOR())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"iorinside",pMaterial->GetIOR(),&md5);
            }
        }
        break;
    case slg::METAL:
        {
            const slg::MetalMaterial *pMaterial = dynamic_cast<const slg::MetalMaterial*>(pMat);
            if(pMaterial->GetKr())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"kr",pMaterial->GetKr(),&md5);
            }
            if(pMaterial->GetExp())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"exp",pMaterial->GetExp(),&md5);
            }
        }
        break;
    case slg::ARCHGLASS:
        {
            const slg::ArchGlassMaterial *pMaterial = dynamic_cast<const slg::ArchGlassMaterial*>(pMat);
            if(pMaterial->GetKr())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"kr",pMaterial->GetKr(),&md5);
            }
            if(pMaterial->GetKt ())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"kt",pMaterial->GetKt (),&md5);
            }
            if(pMaterial->GetOutsideIOR())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"ioroutside",pMaterial->GetOutsideIOR(),&md5);
            }
            if(pMaterial->GetIOR())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"iorinside",pMaterial->GetIOR(),&md5);
            }
        }
        break;
    case slg::MIX:
        {
            const slg::MixMaterial *pMaterial = dynamic_cast<const slg::MixMaterial*>(pMat);
            BOOST_ASSERT_MSG(pMaterial->GetMaterialA() && pMaterial->GetMaterialB(), "invalid mix material");
            const std::string &matAname = ctx.m_mat2name.getMaterialName(pMaterial->GetMaterialA());
            const std::string &matBname = ctx.m_mat2name.getMaterialName(pMaterial->GetMaterialB());
			ExtraMaterialManager::type_id2matinfo_map::iterator itA = queryFromSlgname(matAname);
			ExtraMaterialManager::type_id2matinfo_map::iterator itB = queryFromSlgname(matBname);

			std::ostream *ptmp = ctx.m_pstream;
			ctx.m_pstream = &ss;
			ctx.m_indent++;

            std::string matIdStr;
			if(itA != m_id2matinfo_map.end())
            {
                matIdStr = WriteMaterialImpl(ctx,pMaterial->GetMaterialA(),&itA->first,itA->second.name(),&md5);
            }else{
				//@本材质没有被任何对象直接引用。
                matIdStr = WriteMaterialImpl(ctx,pMaterial->GetMaterialA(),NULL,matAname,&md5);
            }
            o << " material1='" << matIdStr << "'";

			if(itB != m_id2matinfo_map.end())
            {
                matIdStr = WriteMaterialImpl(ctx,pMaterial->GetMaterialB(),&itB->first,itB->second.name(),&md5);
            }else{
				//@本材质没有被任何对象直接引用。
                matIdStr = WriteMaterialImpl(ctx,pMaterial->GetMaterialB(),NULL,matBname,&md5);
            }
            o << " material2='" << matIdStr << "'";

			ctx.m_pstream = ptmp;
			ctx.m_indent--;

            if(pMaterial->GetMixFactor())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"amount",pMaterial->GetMixFactor(),&md5);
            }
        }
        break;
    case slg::NULLMAT:
        {
        }
        break;
    case slg::MATTETRANSLUCENT:
        {
            const slg::MatteTranslucentMaterial  *pMaterial = dynamic_cast<const slg::MatteTranslucentMaterial*>(pMat);
            if(pMaterial->GetKr())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"kr",pMaterial->GetKr(),&md5);
            }
            if(pMaterial->GetKt ())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"kt",pMaterial->GetKt (),&md5);
            }
        }
        break;
    case slg::GLOSSY2:
        {
            const slg::Glossy2Material  *pMaterial = dynamic_cast<const slg::Glossy2Material*>(pMat);
            if(pMaterial->GetKd())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"kd",pMaterial->GetKd(),&md5);
            }
            if(pMaterial->GetKs ())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"ks",pMaterial->GetKs (),&md5);
            }
            if(pMaterial->GetNu ())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"uroughness",pMaterial->GetNu (),&md5);
            }
            if(pMaterial->GetNv ())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"vroughness",pMaterial->GetNv (),&md5);
            }
            if(pMaterial->GetKa ())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"ka",pMaterial->GetKa (),&md5);
            }
            if(pMaterial->GetDepth ())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"d",pMaterial->GetDepth (),&md5);
            }
            if(pMaterial->GetIndex ())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"index",pMaterial->GetIndex (),&md5);
            }
			o << " multibounce='" << (pMaterial->IsMultibounce() ? "true" : "false" ) << "'";
        }
        break;
    case slg::METAL2:
        {
            const slg::Metal2Material  *pMaterial = dynamic_cast<const slg::Metal2Material*>(pMat);
            if(pMaterial->GetN())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"n",pMaterial->GetN(),&md5);
            }
            if(pMaterial->GetK())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"k",pMaterial->GetK(),&md5);
            }
            if(pMaterial->GetNu ())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"uroughness",pMaterial->GetNu (),&md5);
            }
            if(pMaterial->GetNv ())
            {
                ExtraTextureManager::writeTexture(ctx.m_texCtx,"vroughness",pMaterial->GetNv (),&md5);
            }
        }
        break;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code");
    }

    std::string     md5value = md5.finalize().hexdigest();
    MaterialWriteContext::type_ctx2id::iterator it = ctx.m_ctx2id.find(md5value);
    if(it != ctx.m_ctx2id.end())
    {
        o << " use='" << it->second << "'>" << std::endl;
    }else{
        //开始输出material属性及子节点。
        if(!idstring.length()) //pId)
        {//没有给出pId属性。使用md5码作为id并输出.
            idstring = md5value;
            o << " id='" << idstring << "'";
        }
        ctx.m_ctx2id[md5value] = idstring;
        o << " ctxmd5='" << md5value << "'>" << std::endl;
    }
    o << ss.str();
	ctx.outIndent();
    o << "</material>" << std::endl;

    if(ppmd5)
    {
        ppmd5->update(md5value.c_str(),md5value.length());
    }
    return idstring;
}
*/


/*
void
ExtraMaterialManager::write(MaterialWriteContext &ctx,const std::vector<boost::uuids::uuid> &outset)
{
    ///我们顺着对象列表来输出全部引用到的材质。这样，我们只输出使用到的材质。
	///m_id2matinfo_map不仅仅维护直接被对象引用的材质(无此假设)。
    std::vector<boost::uuids::uuid>::const_iterator   outit = outset.begin();
    while(outit != outset.end())
    {
        std::string     slgname;
        std::string     name;
        slg::Material *pMat = this->getSlgMaterial(*outit,&name);

        BOOST_ASSERT_MSG(pMat != NULL,"panic material?");
        WriteMaterialImpl(ctx,pMat,&(*outit),name,NULL);
        outit++;
    }
}

boost::uuids::uuid
ExtraMaterialManager::createAssimpMaterial(aiMaterial *paiMat,SlgUtil::Editor &editor)
{
    boost::uuids::uuid  matid = boost::uuids::random_generator()();
    return matid;
}

boost::uuids::uuid
ExtraMaterialManager::createGrayMaterial(const std::string &name)
{
    boost::uuids::uuid  matid = createGrayMaterial();
    if(name.length())
    {
        ExtraMaterial   em(ObjectNode::idto_string(matid));
        em.m_name = name;
        this->m_id2matinfo_map[matid] = em;
    }
    return matid;
}

*/

void
ExtraMaterialManager::createGrayMaterial(const std::string &id)
{
    slg::Scene  *scene = Doc::instance().pDocData->getSession()->renderConfig->scene;
    std::stringstream   ss;
    ss << "scene.materials." << id << ".type = matte" << std::endl;
    ss << "scene.materials." << id << ".kd = 0.75 0.75 0.75" << std::endl;
    scene->DefineMaterials(ss.str());
}

bool
ExtraMaterialManager::materialIsLight(const slg::Material *pmat)
{
    if(!pmat)
        return false;
    if(pmat->IsLightSource())
        return true;
    const slg::MixMaterial *pMix = dynamic_cast<const slg::MixMaterial*>(pmat);
    if(pMix)
    {
        if(materialIsLight(pMix->GetMaterialA()))
            return true;
        if(materialIsLight(pMix->GetMaterialB()))
            return true;
    }
    return false;
}


slg::Material*
ExtraMaterialManager::getSlgMaterial(const std::string &id)
{
    return Doc::instance().pDocData->m_session->renderConfig->scene->matDefs.GetMaterial(id);
}




}
