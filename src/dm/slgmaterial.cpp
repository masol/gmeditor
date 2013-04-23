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

//SlgMaterial2Name::SlgMaterial2Name()
//{
//    slg::Scene  *scene = Doc::instance().pDocData->getSession()->renderConfig->scene;
//    m_materialNameArray = scene->matDefs.GetMaterialNames ();
//    m_matIdx2NameIdx.resize(m_materialNameArray.size());
//    u_int   nameIdx = 0;
//    for(std::vector< std::string >::const_iterator it = m_materialNameArray.begin(); it < m_materialNameArray.end(); ++it,++nameIdx)
//    {
//        m_matIdx2NameIdx[scene->matDefs.GetMaterialIndex(*it)] = nameIdx;
//    }
//}
//
//const std::string&
//SlgMaterial2Name::getMaterialName(const slg::Material* pmat)const
//{
//    slg::Scene  *scene = Doc::instance().pDocData->getSession()->renderConfig->scene;
//    u_int matNameIdx = m_matIdx2NameIdx[scene->matDefs.GetMaterialIndex(pmat)];
//    return m_materialNameArray[matNameIdx];
//}

static  inline
void removeIfExist(ExtraTextureManager &texManager,const slg::Texture *pTex)
{
    if(pTex)
        texManager.onTextureRemoved(pTex);
}

void
ExtraMaterialManager::onMaterialRemoved(const slg::Material* pMat)
{
    ExtraTextureManager &texManager = Doc::instance().pDocData->texManager;
    switch(pMat->GetType())
    {
    case slg::MATTE:
        {
            const slg::Texture *pTex = dynamic_cast<const slg::MatteMaterial*>(pMat)->GetKd ();
            removeIfExist(texManager,pTex);
        }
        break;
    case slg::MIRROR:
        {
            const slg::Texture *pTex = dynamic_cast<const slg::MirrorMaterial*>(pMat)->GetKr();
            removeIfExist(texManager,pTex);
        }
        break;
    case slg::GLASS:
        {
            const slg::GlassMaterial *pMaterial = dynamic_cast<const slg::GlassMaterial*>(pMat);
            removeIfExist(texManager,pMaterial->GetKr());
            removeIfExist(texManager,pMaterial->GetKt());
            removeIfExist(texManager,pMaterial->GetOutsideIOR());
            removeIfExist(texManager,pMaterial->GetIOR());
        }
        break;
    case slg::METAL:
        {
            const slg::MetalMaterial *pMaterial = dynamic_cast<const slg::MetalMaterial*>(pMat);
            removeIfExist(texManager,pMaterial->GetKr());
            removeIfExist(texManager,pMaterial->GetExp());
        }
        break;
    case slg::ARCHGLASS:
        {
            const slg::ArchGlassMaterial *pMaterial = dynamic_cast<const slg::ArchGlassMaterial*>(pMat);
            removeIfExist(texManager,pMaterial->GetKr());
            removeIfExist(texManager,pMaterial->GetKt());
            removeIfExist(texManager,pMaterial->GetOutsideIOR());
            removeIfExist(texManager,pMaterial->GetIOR());
        }
        break;
    case slg::MIX:
        {
            const slg::MixMaterial *pMaterial = dynamic_cast<const slg::MixMaterial*>(pMat);
            BOOST_ASSERT_MSG(pMaterial->GetMaterialA() && pMaterial->GetMaterialB(), "invalid mix material");
            onMaterialRemoved(pMaterial->GetMaterialA());
            onMaterialRemoved(pMaterial->GetMaterialB());
            removeIfExist(texManager,pMaterial->GetMixFactor());
        }
        break;
    case slg::NULLMAT:
        {
        }
        break;
    case slg::MATTETRANSLUCENT:
        {
            const slg::MatteTranslucentMaterial  *pMaterial = dynamic_cast<const slg::MatteTranslucentMaterial*>(pMat);
            removeIfExist(texManager,pMaterial->GetKr());
            removeIfExist(texManager,pMaterial->GetKt());
        }
        break;
    case slg::GLOSSY2:
        {
            const slg::Glossy2Material  *pMaterial = dynamic_cast<const slg::Glossy2Material*>(pMat);
            removeIfExist(texManager,pMaterial->GetKd());
            removeIfExist(texManager,pMaterial->GetKs());
            removeIfExist(texManager,pMaterial->GetNu());
            removeIfExist(texManager,pMaterial->GetNv());
            removeIfExist(texManager,pMaterial->GetKa());
            removeIfExist(texManager,pMaterial->GetDepth());
            removeIfExist(texManager,pMaterial->GetIndex());
        }
        break;
    case slg::METAL2:
        {
            const slg::Metal2Material  *pMaterial = dynamic_cast<const slg::Metal2Material*>(pMat);
            removeIfExist(texManager,pMaterial->GetN());
            removeIfExist(texManager,pMaterial->GetK());
            removeIfExist(texManager,pMaterial->GetNu());
            removeIfExist(texManager,pMaterial->GetNv());
        }
        break;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code");
    }
    const std::string   &id = m_mat2id[pMat];
    if(id.length())
        m_id2name.erase(id);
    m_mat2id.erase(pMat);
}

void
ExtraMaterialManager::appendMat2IdFromSlg(void)
{
    slg::Scene  *scene = Doc::instance().pDocData->getSession()->renderConfig->scene;
    std::vector<std::string> materialNameArray = scene->matDefs.GetMaterialNames ();
    u_int   nameIdx = 0;
    for(std::vector< std::string >::const_iterator it = materialNameArray.begin(); it < materialNameArray.end(); ++it,++nameIdx)
    {
        m_mat2id[scene->matDefs.GetMaterial(scene->matDefs.GetMaterialIndex(*it))] = materialNameArray[nameIdx];
    }
}

type_xml_node*
ExtraMaterialManager::dump(type_xml_node &parent,const slg::Material* pMat,dumpContext &ctx)
{
    type_xml_doc *pDoc = parent.document();
    BOOST_ASSERT_MSG(pDoc != NULL,"invalid node,must from doc");

    type_xml_node *pSelf = pDoc->allocate_node(NS_RAPIDXML::node_element,constDef::material);
    parent.append_node(pSelf);

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

    conditional_md5 md5(ctx);
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

    return pSelf;
}

void
ExtraMaterialManager::createGrayMaterial(ImportContext &ctx,const std::string &id)
{
    slg::Scene  *scene = ctx.scene();//Doc::instance().pDocData->getSession()->renderConfig->scene;
    std::stringstream   ss;
    ss << "scene.materials." << id << ".type = matte" << std::endl;
    ss << "scene.materials." << id << ".kd = 0.75 0.75 0.75" << std::endl;
    scene->DefineMaterials(ss.str());
    m_mat2id[scene->matDefs.GetMaterial(id)] = id;
    ctx.addAction(slg::MATERIALS_EDIT);
    ctx.addAction(slg::MATERIAL_TYPES_EDIT);
}

void
ExtraMaterialManager::createMatteMaterial(ImportContext &ctx,const std::string& id,const std::string &kdpath,const char* emissionPath,const char* normalPath)
{
//    slg::Material   *pMat;
    createGrayMaterial(ctx,id);
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
