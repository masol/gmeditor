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

static  inline
void removeIfExist(ExtraTextureManager &texManager,const slg::Texture *pTex)
{
    if(pTex)
        texManager.onTextureRemoved(pTex);
}

static  inline void appendTexture(ImportContext &ctx,const std::string &id,type_xml_node &self,std::ostream &o,const std::string &tag,ExtraTextureManager &texManager)
{
    type_xml_node   *pChild = self.first_node(tag.c_str());
    if(pChild)
    {
        o << "scene.materials." << id << "." << tag << " = "<< texManager.createTexture(ctx,*pChild) << std::endl;
    }
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
///@fixme: we can not erase id2name,because the material-refresh will exec to here.in this situation id2name is valid,only pMat changed.but,how to distinguish it from id destroy? the best solution may be callback from slg::matDefs.
//    const std::string   &id = m_mat2id[pMat];
//    if(id.length())
//        m_id2name.erase(id);
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

const std::string&
ExtraMaterialManager::dump(luxrays::Properties &prop,const slg::Material* pMat)
{
    const std::string &id = this->getMaterialId(pMat);
    BOOST_ASSERT_MSG(!id.empty(),"where are the pMat come from?");
    std::string typeStr = DocMat::getTypeNameFromType(pMat->GetType());
    const std::string matPrefix = "scene.materials." + id + '.';
    prop.SetString(matPrefix + "type",typeStr);

    if(pMat->IsLightSource())
    {
        prop.SetString(matPrefix + constDef::emission,ExtraTextureManager::dump(prop,pMat->GetEmitTexture()));
    }

    if(pMat->HasBumpTex())
    {
        prop.SetString(matPrefix + constDef::bumptex,ExtraTextureManager::dump(prop,pMat->GetBumpTexture()));
    }

    if(pMat->HasNormalTex())
    {
        prop.SetString(matPrefix + constDef::normaltex,ExtraTextureManager::dump(prop,pMat->GetNormalTexture()));
    }

    switch(pMat->GetType())
    {
    case slg::MATTE:
        {
            const slg::Texture *pTex = dynamic_cast<const slg::MatteMaterial*>(pMat)->GetKd ();
            if(pTex)
            {
                prop.SetString(matPrefix + constDef::kd,ExtraTextureManager::dump(prop,pTex));
            }
        }
        break;
    case slg::MIRROR:
        {
            const slg::Texture *pTex = dynamic_cast<const slg::MirrorMaterial*>(pMat)->GetKr();
            if(pTex)
            {
                prop.SetString(matPrefix + constDef::kr,ExtraTextureManager::dump(prop,pTex));
            }
        }
        break;
    case slg::GLASS:
        {
            const slg::GlassMaterial *pMaterial = dynamic_cast<const slg::GlassMaterial*>(pMat);
            if(pMaterial->GetKr())
            {
                prop.SetString(matPrefix + constDef::kr,ExtraTextureManager::dump(prop,pMaterial->GetKr()));
            }
            if(pMaterial->GetKt())
            {
                prop.SetString(matPrefix + constDef::kt,ExtraTextureManager::dump(prop,pMaterial->GetKt()));
            }
            if(pMaterial->GetOutsideIOR())
            {
                prop.SetString(matPrefix + constDef::ioroutside,ExtraTextureManager::dump(prop,pMaterial->GetOutsideIOR()));
            }
            if(pMaterial->GetIOR())
            {
                prop.SetString(matPrefix + constDef::iorinside,ExtraTextureManager::dump(prop,pMaterial->GetIOR()));
            }
        }
        break;
    case slg::METAL:
        {
            const slg::MetalMaterial *pMaterial = dynamic_cast<const slg::MetalMaterial*>(pMat);
            if(pMaterial->GetKr())
            {
                prop.SetString(matPrefix + constDef::kr,ExtraTextureManager::dump(prop,pMaterial->GetKr()));
            }
            if(pMaterial->GetExp())
            {
                prop.SetString(matPrefix + constDef::exp,ExtraTextureManager::dump(prop,pMaterial->GetExp()));
            }
        }
        break;
    case slg::ARCHGLASS:
        {
            const slg::ArchGlassMaterial *pMaterial = dynamic_cast<const slg::ArchGlassMaterial*>(pMat);
            if(pMaterial->GetKr())
            {
                prop.SetString(matPrefix + constDef::kr,ExtraTextureManager::dump(prop,pMaterial->GetKr()));
            }
            if(pMaterial->GetKt ())
            {
                prop.SetString(matPrefix + constDef::kt,ExtraTextureManager::dump(prop,pMaterial->GetKt()));
            }
            if(pMaterial->GetOutsideIOR())
            {
                prop.SetString(matPrefix + constDef::ioroutside,ExtraTextureManager::dump(prop,pMaterial->GetOutsideIOR()));
            }
            if(pMaterial->GetIOR())
            {
                prop.SetString(matPrefix + constDef::iorinside,ExtraTextureManager::dump(prop,pMaterial->GetIOR()));
            }
        }
        break;
    case slg::MIX:
        {
            const slg::MixMaterial *pMaterial = dynamic_cast<const slg::MixMaterial*>(pMat);
            BOOST_ASSERT_MSG(pMaterial->GetMaterialA() && pMaterial->GetMaterialB(), "invalid mix material");

            prop.SetString(matPrefix + "material1",dump(prop,pMaterial->GetMaterialA()));
            prop.SetString(matPrefix + "material2",dump(prop,pMaterial->GetMaterialB()));

            if(pMaterial->GetMixFactor())
            {
                prop.SetString(matPrefix + constDef::amount,ExtraTextureManager::dump(prop,pMaterial->GetMixFactor()));
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
				prop.SetString(matPrefix + constDef::kr,ExtraTextureManager::dump(prop,pMaterial->GetKr()));
            }
            if(pMaterial->GetKt ())
            {
				prop.SetString(matPrefix + constDef::kt,ExtraTextureManager::dump(prop,pMaterial->GetKt()));
            }
        }
        break;
    case slg::GLOSSY2:
        {
            const slg::Glossy2Material  *pMaterial = dynamic_cast<const slg::Glossy2Material*>(pMat);
            if(pMaterial->GetKd())
            {
				prop.SetString(matPrefix + constDef::kd,ExtraTextureManager::dump(prop,pMaterial->GetKd()));
            }
            if(pMaterial->GetKs ())
            {
				prop.SetString(matPrefix + constDef::ks,ExtraTextureManager::dump(prop,pMaterial->GetKs()));
            }
            if(pMaterial->GetNu ())
            {
				prop.SetString(matPrefix + constDef::uroughness,ExtraTextureManager::dump(prop,pMaterial->GetNu()));
            }
            if(pMaterial->GetNv ())
            {
				prop.SetString(matPrefix + constDef::vroughness,ExtraTextureManager::dump(prop,pMaterial->GetNv()));
            }
            if(pMaterial->GetKa ())
            {
				prop.SetString(matPrefix + constDef::ka,ExtraTextureManager::dump(prop,pMaterial->GetKa()));
            }
            if(pMaterial->GetDepth ())
            {
                prop.SetString(matPrefix + constDef::d,ExtraTextureManager::dump(prop,pMaterial->GetDepth()));
            }
            if(pMaterial->GetIndex ())
            {
                prop.SetString(matPrefix + constDef::index,ExtraTextureManager::dump(prop,pMaterial->GetIndex()));
            }
            prop.SetString(matPrefix + "multibounce",(pMaterial->IsMultibounce() ? "1" : "0" ));
        }
        break;
    case slg::METAL2:
        {
            const slg::Metal2Material  *pMaterial = dynamic_cast<const slg::Metal2Material*>(pMat);
            if(pMaterial->GetN())
            {
                prop.SetString(matPrefix + constDef::n,ExtraTextureManager::dump(prop,pMaterial->GetN()));
            }
            if(pMaterial->GetK())
            {
                prop.SetString(matPrefix + constDef::k,ExtraTextureManager::dump(prop,pMaterial->GetK()));
            }
            if(pMaterial->GetNu ())
            {
                prop.SetString(matPrefix + constDef::uroughness,ExtraTextureManager::dump(prop,pMaterial->GetNu()));
            }
            if(pMaterial->GetNv ())
            {
                prop.SetString(matPrefix + constDef::vroughness,ExtraTextureManager::dump(prop,pMaterial->GetNv()));
            }
        }
        break;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code");
    }
    return id;
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
    md5.update(type.c_str(),(MD5::size_type)type.length());
    ExtraTextureManager &texManager = Doc::instance().pDocData->texManager;

    if(pMat->IsLightSource())
    {
        texManager.dump(*pSelf,constDef::emission,pMat->GetEmitTexture(),ctx,md5);
    }

    if(pMat->HasBumpTex())
    {
        texManager.dump(*pSelf,constDef::bumptex,pMat->GetBumpTexture(),ctx,md5);
    }

    if(pMat->HasNormalTex())
    {
        texManager.dump(*pSelf,constDef::normaltex,pMat->GetNormalTexture(),ctx,md5);
    }

    switch(pMat->GetType())
    {
    case slg::MATTE:
        {
            const slg::Texture *pTex = dynamic_cast<const slg::MatteMaterial*>(pMat)->GetKd ();
            if(pTex)
            {
                texManager.dump(*pSelf,constDef::kd,pTex,ctx,md5);
            }
        }
        break;
    case slg::MIRROR:
        {
            const slg::Texture *pTex = dynamic_cast<const slg::MirrorMaterial*>(pMat)->GetKr();
            if(pTex)
            {
                texManager.dump(*pSelf,constDef::kr,pTex,ctx,md5);
            }
        }
        break;
    case slg::GLASS:
        {
            const slg::GlassMaterial *pMaterial = dynamic_cast<const slg::GlassMaterial*>(pMat);
            if(pMaterial->GetKr())
            {
                texManager.dump(*pSelf,constDef::kr,pMaterial->GetKr(),ctx,md5);
            }
            if(pMaterial->GetKt())
            {
                texManager.dump(*pSelf,constDef::kt,pMaterial->GetKt(),ctx,md5);
            }
            if(pMaterial->GetOutsideIOR())
            {
                texManager.dump(*pSelf,constDef::ioroutside,pMaterial->GetOutsideIOR(),ctx,md5);
            }
            if(pMaterial->GetIOR())
            {
                texManager.dump(*pSelf,constDef::iorinside,pMaterial->GetIOR(),ctx,md5);
            }
        }
        break;
    case slg::METAL:
        {
            const slg::MetalMaterial *pMaterial = dynamic_cast<const slg::MetalMaterial*>(pMat);
            if(pMaterial->GetKr())
            {
                texManager.dump(*pSelf,constDef::kr,pMaterial->GetKr(),ctx,md5);
            }
            if(pMaterial->GetExp())
            {
                texManager.dump(*pSelf,constDef::exp,pMaterial->GetExp(),ctx,md5);
            }
        }
        break;
    case slg::ARCHGLASS:
        {
            const slg::ArchGlassMaterial *pMaterial = dynamic_cast<const slg::ArchGlassMaterial*>(pMat);
            if(pMaterial->GetKr())
            {
                texManager.dump(*pSelf,constDef::kr,pMaterial->GetKr(),ctx,md5);
            }
            if(pMaterial->GetKt ())
            {
                texManager.dump(*pSelf,constDef::kt,pMaterial->GetKt(),ctx,md5);
            }
            if(pMaterial->GetOutsideIOR())
            {
                texManager.dump(*pSelf,constDef::ioroutside,pMaterial->GetOutsideIOR(),ctx,md5);
            }
            if(pMaterial->GetIOR())
            {
                texManager.dump(*pSelf,constDef::iorinside,pMaterial->GetIOR(),ctx,md5);
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
				texManager.dump(*pSelf,constDef::amount,pMaterial->GetMixFactor(),ctx,md5);
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
				texManager.dump(*pSelf,constDef::kr,pMaterial->GetKr(),ctx,md5);
            }
            if(pMaterial->GetKt ())
            {
				texManager.dump(*pSelf,constDef::kt,pMaterial->GetKt(),ctx,md5);
            }
        }
        break;
    case slg::GLOSSY2:
        {
            const slg::Glossy2Material  *pMaterial = dynamic_cast<const slg::Glossy2Material*>(pMat);
            if(pMaterial->GetKd())
            {
				texManager.dump(*pSelf,constDef::kd,pMaterial->GetKd(),ctx,md5);
            }
            if(pMaterial->GetKs ())
            {
				texManager.dump(*pSelf,constDef::ks,pMaterial->GetKs(),ctx,md5);
            }
            if(pMaterial->GetNu ())
            {
				texManager.dump(*pSelf,constDef::uroughness,pMaterial->GetNu(),ctx,md5);
            }
            if(pMaterial->GetNv ())
            {
				texManager.dump(*pSelf,constDef::vroughness,pMaterial->GetNv(),ctx,md5);
            }
            if(pMaterial->GetKa ())
            {
				texManager.dump(*pSelf,constDef::ka,pMaterial->GetKa(),ctx,md5);
            }
            if(pMaterial->GetDepth ())
            {
				texManager.dump(*pSelf,constDef::d,pMaterial->GetDepth(),ctx,md5);
            }
            if(pMaterial->GetIndex ())
            {
				texManager.dump(*pSelf,constDef::index,pMaterial->GetIndex(),ctx,md5);
            }
			pSelf->append_attribute(pDoc->allocate_attribute(constDef::position,allocate_string(pDoc,(pMaterial->IsMultibounce() ? "1" : "0" ))));
        }
        break;
    case slg::METAL2:
        {
            const slg::Metal2Material  *pMaterial = dynamic_cast<const slg::Metal2Material*>(pMat);
            if(pMaterial->GetN())
            {
				texManager.dump(*pSelf,constDef::n,pMaterial->GetN(),ctx,md5);
            }
            if(pMaterial->GetK())
            {
				texManager.dump(*pSelf,constDef::k,pMaterial->GetK(),ctx,md5);
            }
            if(pMaterial->GetNu ())
            {
				texManager.dump(*pSelf,constDef::uroughness,pMaterial->GetNu(),ctx,md5);
            }
            if(pMaterial->GetNv ())
            {
				texManager.dump(*pSelf,constDef::vroughness,pMaterial->GetNv(),ctx,md5);
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

const std::string&
ExtraMaterialManager::buildDefaultMaterial(SlgUtil::UpdateContext &ctx,const slg::Material *pMat,int type)
{
    //first,remove all material from ctx.
    const std::string &id = this->getMaterialId(pMat);
    std::string prefix = "scene.materials." + id;
    std::vector< std::string >  matKeys = ctx.props.GetAllKeys (prefix);
    BOOST_FOREACH(const std::string &key,matKeys)
    {
        ctx.props.Delete(key);
    }

    prefix += '.';

    switch(type)
    {
    case slg::MATTE:
        ctx.props.SetString(prefix + constDef::type,"matte");
        ctx.props.SetString(prefix + constDef::kd,"0.75 0.75 0.75");
        break;
    case slg::MIRROR:
        ctx.props.SetString(prefix + constDef::type,"mirror");
        ctx.props.SetString(prefix + constDef::kd,"1.0 1.0 1.0");
        break;
    case slg::GLASS:
        ctx.props.SetString(prefix + constDef::type,"glass");
        ctx.props.SetString(prefix + constDef::kr,"1.0 1.0 1.0");
        ctx.props.SetString(prefix + constDef::kt,"1.0 1.0 1.0");
        ctx.props.SetString(prefix + constDef::ioroutside,"1.0");
        ctx.props.SetString(prefix + constDef::iorinside,"1.5");
        break;
    case slg::METAL:
        ctx.props.SetString(prefix + constDef::type,"metal");
        ctx.props.SetString(prefix + constDef::kr,"1.0 1.0 1.0");
        ctx.props.SetString(prefix + constDef::exp,"10.0");
        break;
    case slg::ARCHGLASS:
        ctx.props.SetString(prefix + constDef::type,"archglass");
        ctx.props.SetString(prefix + constDef::kr,"1.0 1.0 1.0");
        ctx.props.SetString(prefix + constDef::kt,"1.0 1.0 1.0");
        ctx.props.SetString(prefix + constDef::ioroutside,"1.0");
        ctx.props.SetString(prefix + constDef::iorinside,"1.5");
        break;
    case slg::MIX:
        {
            boost::uuids::random_generator  gen;
            std::string mat1 = string::uuid_to_string(gen());
            std::string mat2 = string::uuid_to_string(gen());
            createGrayMaterial(ctx.editor.scene(),mat1);
            createGrayMaterial(ctx.editor.scene(),mat2);

            ctx.props.SetString(prefix + constDef::type,"mix");
            ctx.props.SetString(prefix + "material1",mat1);
            ctx.props.SetString(prefix + "material2",mat2);
            ctx.props.SetString(prefix + "amount","0.5");
         }
        break;
    case slg::NULLMAT:
        ctx.props.SetString(prefix + constDef::type,"null");
        break;
    case slg::MATTETRANSLUCENT:
        ctx.props.SetString(prefix + constDef::type,"mattetranslucent");
        ctx.props.SetString(prefix + constDef::kr,"0.5 0.5 0.5");
        ctx.props.SetString(prefix + constDef::kt,"0.5 0.5 0.5");
        break;
    case slg::GLOSSY2:
        ctx.props.SetString(prefix + constDef::type,"glossy2");
        //other use default.
        break;
    case slg::METAL2:
        ctx.props.SetString(prefix + constDef::type,"metal2");
        //other use default.
        break;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code");
    }

    if(!isMaterialTypeCompiled(type))
    {
        ctx.editor.addAction(slg::MATERIAL_TYPES_EDIT);
    }
    return id;
}

void
ExtraMaterialManager::updateMaterialInfo(const slg::Material *pMat,SlgMaterial2Name &mat2name,SlgTexture2Name &tex2name)
{
    const std::string &matId = mat2name.getMaterialName(pMat);
    updateMaterialId(pMat,matId);

    ExtraTextureManager &texManager = Doc::instance().pDocData->texManager;

    if(pMat->IsLightSource())
    {
        texManager.updateTextureInfo(pMat->GetEmitTexture(),tex2name);
    }

    if(pMat->HasBumpTex())
    {
        texManager.updateTextureInfo(pMat->GetBumpTexture(),tex2name);
    }

    if(pMat->HasNormalTex())
    {
        texManager.updateTextureInfo(pMat->GetNormalTexture(),tex2name);
    }

    switch(pMat->GetType())
    {
    case slg::MATTE:
        {
            const slg::Texture *pTex = dynamic_cast<const slg::MatteMaterial*>(pMat)->GetKd ();
            if(pTex)
            {
                texManager.updateTextureInfo(pTex,tex2name);
            }
        }
        break;
    case slg::MIRROR:
        {
            const slg::Texture *pTex = dynamic_cast<const slg::MirrorMaterial*>(pMat)->GetKr();
            if(pTex)
            {
                texManager.updateTextureInfo(pTex,tex2name);
            }
        }
        break;
    case slg::GLASS:
        {
            const slg::GlassMaterial *pMaterial = dynamic_cast<const slg::GlassMaterial*>(pMat);
            if(pMaterial->GetKr())
            {
                texManager.updateTextureInfo(pMaterial->GetKr(),tex2name);
            }
            if(pMaterial->GetKt())
            {
                texManager.updateTextureInfo(pMaterial->GetKt(),tex2name);
            }
            if(pMaterial->GetOutsideIOR())
            {
                texManager.updateTextureInfo(pMaterial->GetOutsideIOR(),tex2name);
            }
            if(pMaterial->GetIOR())
            {
                texManager.updateTextureInfo(pMaterial->GetIOR(),tex2name);
            }
        }
        break;
    case slg::METAL:
        {
            const slg::MetalMaterial *pMaterial = dynamic_cast<const slg::MetalMaterial*>(pMat);
            if(pMaterial->GetKr())
            {
                texManager.updateTextureInfo(pMaterial->GetKr(),tex2name);
            }
            if(pMaterial->GetExp())
            {
                texManager.updateTextureInfo(pMaterial->GetExp(),tex2name);
            }
        }
        break;
    case slg::ARCHGLASS:
        {
            const slg::ArchGlassMaterial *pMaterial = dynamic_cast<const slg::ArchGlassMaterial*>(pMat);
            if(pMaterial->GetKr())
            {
                texManager.updateTextureInfo(pMaterial->GetKr(),tex2name);
            }
            if(pMaterial->GetKt ())
            {
                texManager.updateTextureInfo(pMaterial->GetKt(),tex2name);
            }
            if(pMaterial->GetOutsideIOR())
            {
                texManager.updateTextureInfo(pMaterial->GetOutsideIOR(),tex2name);
            }
            if(pMaterial->GetIOR())
            {
                texManager.updateTextureInfo(pMaterial->GetIOR(),tex2name);
            }
        }
        break;
    case slg::MIX:
        {
            const slg::MixMaterial *pMaterial = dynamic_cast<const slg::MixMaterial*>(pMat);
            BOOST_ASSERT_MSG(pMaterial->GetMaterialA() && pMaterial->GetMaterialB(), "invalid mix material");

            updateMaterialInfo(pMaterial->GetMaterialA(),mat2name,tex2name);
            updateMaterialInfo(pMaterial->GetMaterialB(),mat2name,tex2name);
            if(pMaterial->GetMixFactor())
            {
				texManager.updateTextureInfo(pMaterial->GetMixFactor(),tex2name);
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
				texManager.updateTextureInfo(pMaterial->GetKr(),tex2name);
            }
            if(pMaterial->GetKt ())
            {
				texManager.updateTextureInfo(pMaterial->GetKt(),tex2name);
            }
        }
        break;
    case slg::GLOSSY2:
        {
            const slg::Glossy2Material  *pMaterial = dynamic_cast<const slg::Glossy2Material*>(pMat);
            if(pMaterial->GetKd())
            {
				texManager.updateTextureInfo(pMaterial->GetKd(),tex2name);
            }
            if(pMaterial->GetKs ())
            {
				texManager.updateTextureInfo(pMaterial->GetKs(),tex2name);
            }
            if(pMaterial->GetNu ())
            {
				texManager.updateTextureInfo(pMaterial->GetNu(),tex2name);
            }
            if(pMaterial->GetNv ())
            {
				texManager.updateTextureInfo(pMaterial->GetNv(),tex2name);
            }
            if(pMaterial->GetKa ())
            {
				texManager.updateTextureInfo(pMaterial->GetKa(),tex2name);
            }
            if(pMaterial->GetDepth ())
            {
				texManager.updateTextureInfo(pMaterial->GetDepth(),tex2name);
            }
            if(pMaterial->GetIndex ())
            {
				texManager.updateTextureInfo(pMaterial->GetIndex(),tex2name);
            }
        }
        break;
    case slg::METAL2:
        {
            const slg::Metal2Material  *pMaterial = dynamic_cast<const slg::Metal2Material*>(pMat);
            if(pMaterial->GetN())
            {
				texManager.updateTextureInfo(pMaterial->GetN(),tex2name);
            }
            if(pMaterial->GetK())
            {
				texManager.updateTextureInfo(pMaterial->GetK(),tex2name);
            }
            if(pMaterial->GetNu ())
            {
				texManager.updateTextureInfo(pMaterial->GetNu(),tex2name);
            }
            if(pMaterial->GetNv ())
            {
				texManager.updateTextureInfo(pMaterial->GetNv(),tex2name);
            }
        }
        break;
    default:
        BOOST_ASSERT_MSG(false,"unreachable code");
    }
}

const slg::Texture*
ExtraMaterialManager::getTextureFromKeypath(const slg::Material *pMat,const std::vector<std::string> &keyPath,size_t curIdx)
{
    if(curIdx == keyPath.size())
        return NULL;
    const std::string &curKey = keyPath[curIdx];
    if(curKey == constDef::emission)
    {//
        return ExtraTextureManager::getTextureFromKeypath(pMat->GetEmitTexture(),keyPath,curIdx+1);
    }else if(curKey == constDef::bumptex)
    {
        return ExtraTextureManager::getTextureFromKeypath(pMat->GetBumpTexture(),keyPath,curIdx+1);
    }else if(curKey == constDef::normaltex)
    {
        return ExtraTextureManager::getTextureFromKeypath(pMat->GetNormalTexture(),keyPath,curIdx+1);
    }else{
        switch(pMat->GetType())
        {
        case slg::MATTE:
            return ExtraTextureManager::getTextureFromKeypath(dynamic_cast<const slg::MatteMaterial*>(pMat)->GetKd(),keyPath,curIdx+1);
        case slg::MIRROR:
            return ExtraTextureManager::getTextureFromKeypath(dynamic_cast<const slg::MirrorMaterial*>(pMat)->GetKr(),keyPath,curIdx+1);
        case slg::GLASS:
            {
                const slg::GlassMaterial *pMaterial = dynamic_cast<const slg::GlassMaterial*>(pMat);
                if(curKey == constDef::kr)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetKr(),keyPath,curIdx+1);
                }else if(curKey == constDef::kt)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetKt(),keyPath,curIdx+1);
                }else if(curKey == constDef::ioroutside)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetOutsideIOR(),keyPath,curIdx+1);
                }else if(curKey == constDef::iorinside)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetIOR(),keyPath,curIdx+1);
                }
            }
        case slg::METAL:
            {
                const slg::MetalMaterial *pMaterial = dynamic_cast<const slg::MetalMaterial*>(pMat);
                if(curKey == constDef::kr)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetKr(),keyPath,curIdx+1);
                }else if(curKey == constDef::exp)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetExp(),keyPath,curIdx+1);
                }
            }
            break;
        case slg::ARCHGLASS:
            {
                const slg::ArchGlassMaterial *pMaterial = dynamic_cast<const slg::ArchGlassMaterial*>(pMat);
                if(curKey == constDef::kr)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetKr(),keyPath,curIdx+1);
                }else if(curKey == constDef::kt)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetKt(),keyPath,curIdx+1);
                }else if(curKey == constDef::ioroutside)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetOutsideIOR(),keyPath,curIdx+1);
                }else if(curKey == constDef::iorinside)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetIOR(),keyPath,curIdx+1);
                }
            }
            break;
        case slg::MIX:
            {
                const slg::MixMaterial *pMaterial = dynamic_cast<const slg::MixMaterial*>(pMat);
                BOOST_ASSERT_MSG(pMaterial->GetMaterialA() && pMaterial->GetMaterialB(), "invalid mix material");
                if(boost::iequals(curKey,"material1"))
                {
                    return getTextureFromKeypath(pMaterial->GetMaterialA(),keyPath,curIdx+1);
                }else if(boost::iequals(curKey,"material2"))
                {
                    return getTextureFromKeypath(pMaterial->GetMaterialB(),keyPath,curIdx+1);
                }else if(boost::iequals(curKey,constDef::amount))
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetMixFactor(),keyPath,curIdx+1);
                }
            }
            break;
        case slg::NULLMAT:
            break;
        case slg::MATTETRANSLUCENT:
            {
                const slg::MatteTranslucentMaterial  *pMaterial = dynamic_cast<const slg::MatteTranslucentMaterial*>(pMat);
                if(curKey == constDef::kr)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetKr(),keyPath,curIdx+1);
                }else if(curKey == constDef::kt)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetKt(),keyPath,curIdx+1);
                }
            }
            break;
        case slg::GLOSSY2:
            {
                const slg::Glossy2Material  *pMaterial = dynamic_cast<const slg::Glossy2Material*>(pMat);
                if(curKey == constDef::kd)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetKd(),keyPath,curIdx+1);
                }else if(curKey == constDef::ks)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetKs(),keyPath,curIdx+1);
                }else if(curKey == constDef::uroughness)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetNu(),keyPath,curIdx+1);
                }else if(curKey == constDef::vroughness)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetNv(),keyPath,curIdx+1);
                }else if(curKey == constDef::ka)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetKa(),keyPath,curIdx+1);
                }else if(curKey == constDef::d)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetDepth(),keyPath,curIdx+1);
                }else if(curKey == constDef::index)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetIndex(),keyPath,curIdx+1);
                }
            }
            break;
        case slg::METAL2:
            {
                const slg::Metal2Material  *pMaterial = dynamic_cast<const slg::Metal2Material*>(pMat);
                if(curKey == constDef::n)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetN(),keyPath,curIdx+1);
                }else if(curKey == constDef::k)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetK(),keyPath,curIdx+1);
                }else if(curKey == constDef::uroughness)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetNu(),keyPath,curIdx+1);
                }else if(curKey == constDef::vroughness)
                {
                    return ExtraTextureManager::getTextureFromKeypath(pMaterial->GetNv(),keyPath,curIdx+1);
                }
            }
        default:
            BOOST_ASSERT_MSG(false,"unreachable code");
        }
    }
    throw std::runtime_error("reach unreachable code");
}

static  inline bool
checkAndUpdateDisableTexture(ExtraTextureManager &texManager,SlgUtil::UpdateContext &ctx,const std::string &curKey,const std::string &prefix,const std::string &suffix,const slg::Texture *pTex,size_t curIdx)
{
    if(curKey == suffix)
    {

        std::string key = prefix + suffix;
        std::string texValue = texManager.updateTexture(ctx,pTex,curIdx+1);
        if(texValue.empty())
        {
            ctx.props.Delete(key);
        }else{
            ctx.props.SetString(key,texValue);
        }
        return true;
    }
    return false;
}


bool
ExtraMaterialManager::updateMaterial(SlgUtil::UpdateContext &ctx,const slg::Material *pMat,size_t curIdx)
{
    if(curIdx == ctx.keyPath.size())
    {//最后一级。这里就必然是material type.
        int type = boost::lexical_cast<int>(ctx.value);

        ctx.updatedId = buildDefaultMaterial(ctx,pMat,type);
        ctx.idIsMat = true;
        ctx.bGenNode = true;
    }else
    {
        ExtraTextureManager &texManager = Doc::instance().pDocData->texManager;
        const std::string &matId = this->getMaterialId(pMat);
        std::string prefix = "scene.materials." + matId + '.';
        const std::string &curKey = ctx.keyPath[curIdx];

        if(checkAndUpdateDisableTexture(texManager,ctx,curKey,prefix,constDef::emission,pMat->GetEmitTexture(),curIdx))
        {
        }else if(checkAndUpdateDisableTexture(texManager,ctx,curKey,prefix,constDef::bumptex,pMat->GetBumpTexture(),curIdx))
        {
        }else if(checkAndUpdateDisableTexture(texManager,ctx,curKey,prefix,constDef::normaltex,pMat->GetNormalTexture(),curIdx))
        {
        }else{
            switch(pMat->GetType())
            {
            case slg::MATTE:
                if(curKey == constDef::kd)
                {
                    ctx.props.SetString(prefix + constDef::kd,texManager.updateTexture(ctx,dynamic_cast<const slg::MatteMaterial*>(pMat)->GetKd(),curIdx+1));
                }else{
                    BOOST_ASSERT(false);
                }
                break;
            case slg::MIRROR:
                if(curKey == constDef::kr)
                {
                    ctx.props.SetString(prefix + constDef::kr,texManager.updateTexture(ctx,dynamic_cast<const slg::MirrorMaterial*>(pMat)->GetKr(),curIdx+1));
                }else{
                    BOOST_ASSERT(false);
                }
                break;
            case slg::GLASS:
                {
                    const slg::GlassMaterial *pMaterial = dynamic_cast<const slg::GlassMaterial*>(pMat);
                    if(curKey == constDef::kr)
                    {
                        ctx.props.SetString(prefix + constDef::kr,texManager.updateTexture(
                            ctx,pMaterial->GetKr(),curIdx+1));
                    }else if(curKey == constDef::kt)
                    {
                        ctx.props.SetString(prefix + constDef::kt,texManager.updateTexture(
                            ctx,pMaterial->GetKt(),curIdx+1));
                    }else if(curKey == constDef::ioroutside)
                    {
                        ctx.props.SetString(prefix + constDef::ioroutside,texManager.updateTexture(
                            ctx,pMaterial->GetOutsideIOR(),curIdx+1));
                    }else if(curKey == constDef::iorinside)
                    {
                        ctx.props.SetString(prefix + constDef::iorinside,texManager.updateTexture(
                            ctx,pMaterial->GetIOR(),curIdx+1));
                    }else{
                        BOOST_ASSERT(false);
                    }
                }
            case slg::METAL:
                {
                    const slg::MetalMaterial *pMaterial = dynamic_cast<const slg::MetalMaterial*>(pMat);
                    if(curKey == constDef::kr)
                    {
                        ctx.props.SetString(prefix + constDef::kr,texManager.updateTexture(
                            ctx,pMaterial->GetKr(),curIdx+1));
                    }else if(curKey == constDef::exp)
                    {
                        ctx.props.SetString(prefix + constDef::exp,texManager.updateTexture(
                            ctx,pMaterial->GetExp(),curIdx+1));
                    }else{
                        BOOST_ASSERT(false);
                    }
                }
                break;
            case slg::ARCHGLASS:
                {
                    const slg::ArchGlassMaterial *pMaterial = dynamic_cast<const slg::ArchGlassMaterial*>(pMat);
                    if(curKey == constDef::kr)
                    {
                        ctx.props.SetString(prefix + constDef::kr,texManager.updateTexture(
                            ctx,pMaterial->GetKr(),curIdx+1));
                    }else if(curKey == constDef::kt)
                    {
                        ctx.props.SetString(prefix + constDef::kt,texManager.updateTexture(
                            ctx,pMaterial->GetKt(),curIdx+1));
                    }else if(curKey == constDef::ioroutside)
                    {
                        ctx.props.SetString(prefix + constDef::ioroutside,texManager.updateTexture(
                            ctx,pMaterial->GetOutsideIOR(),curIdx+1));
                    }else if(curKey == constDef::iorinside)
                    {
                        ctx.props.SetString(prefix + constDef::iorinside,texManager.updateTexture(
                            ctx,pMaterial->GetIOR(),curIdx+1));
                    }else{
                        BOOST_ASSERT(false);
                    }
                }
                break;
            case slg::MIX:
                {
                    const slg::MixMaterial *pMaterial = dynamic_cast<const slg::MixMaterial*>(pMat);
                    BOOST_ASSERT_MSG(pMaterial->GetMaterialA() && pMaterial->GetMaterialB(), "invalid mix material");
                    if(boost::iequals(curKey,"material1"))
                    {
                        const std::string &id = this->getMaterialId(pMaterial->GetMaterialA());
                        updateMaterial(ctx,pMaterial->GetMaterialA(),curIdx+1);
                        //in any case,we need update material.
                        ctx.editor.scene()->UpdateMaterial(id,ctx.props);
                    }else if(boost::iequals(curKey,"material2"))
                    {
                        const std::string &id = this->getMaterialId(pMaterial->GetMaterialB());
                        updateMaterial(ctx,pMaterial->GetMaterialB(),curIdx+1);
                        //in any case,we need update material.
                        ctx.editor.scene()->UpdateMaterial(id,ctx.props);
                    }else if(boost::iequals(curKey,constDef::amount))
                    {
                        ctx.props.SetString(prefix + constDef::amount,texManager.updateTexture(
                            ctx,pMaterial->GetMixFactor(),curIdx+1));
                    }else{
                        BOOST_ASSERT(false);
                    }
                }
                break;
            case slg::NULLMAT:
                break;
            case slg::MATTETRANSLUCENT:
                {
                    const slg::MatteTranslucentMaterial  *pMaterial = dynamic_cast<const slg::MatteTranslucentMaterial*>(pMat);
                    if(curKey == constDef::kr)
                    {
                        ctx.props.SetString(prefix + constDef::kr,texManager.updateTexture(
                            ctx,pMaterial->GetKr(),curIdx+1));
                    }else if(curKey == constDef::kt)
                    {
                        ctx.props.SetString(prefix + constDef::kt,texManager.updateTexture(
                            ctx,pMaterial->GetKt(),curIdx+1));
                    }else{
                        BOOST_ASSERT(false);
                    }
                }
                break;
            case slg::GLOSSY2:
                {
                    const slg::Glossy2Material  *pMaterial = dynamic_cast<const slg::Glossy2Material*>(pMat);
                    if(curKey == constDef::kd)
                    {
                        ctx.props.SetString(prefix + constDef::kd,texManager.updateTexture(
                            ctx,pMaterial->GetKd(),curIdx+1));
                    }else if(curKey == constDef::ks)
                    {
                        ctx.props.SetString(prefix + constDef::ks,texManager.updateTexture(
                            ctx,pMaterial->GetKs(),curIdx+1));
                    }else if(curKey == constDef::uroughness)
                    {
                        ctx.props.SetString(prefix + constDef::uroughness,texManager.updateTexture(
                            ctx,pMaterial->GetNu(),curIdx+1));
                    }else if(curKey == constDef::vroughness)
                    {
                        ctx.props.SetString(prefix + constDef::vroughness,texManager.updateTexture(
                            ctx,pMaterial->GetNv(),curIdx+1));
                    }else if(curKey == constDef::ka)
                    {
                        ctx.props.SetString(prefix + constDef::ka,texManager.updateTexture(
                            ctx,pMaterial->GetKa(),curIdx+1));
                    }else if(curKey == constDef::d)
                    {
                        ctx.props.SetString(prefix + constDef::d,texManager.updateTexture(
                            ctx,pMaterial->GetDepth(),curIdx+1));
                    }else if(curKey == constDef::index)
                    {
                        ctx.props.SetString(prefix + constDef::index,texManager.updateTexture(
                            ctx,pMaterial->GetIndex(),curIdx+1));
                    }else if(curKey == "multibounce")
                    {
                        ctx.props.SetString(prefix + "multibounce",(boost::iequals(ctx.value,"true") ? "1" : "0"));
                    }else{
                        BOOST_ASSERT(false);
                    }
                }
                break;
            case slg::METAL2:
                {
                    const slg::Metal2Material  *pMaterial = dynamic_cast<const slg::Metal2Material*>(pMat);
                    if(curKey == constDef::n)
                    {
                        ctx.props.SetString(prefix + constDef::n,texManager.updateTexture(
                            ctx,pMaterial->GetN(),curIdx+1));
                    }else if(curKey == constDef::k)
                    {
                        ctx.props.SetString(prefix + constDef::k,texManager.updateTexture(
                            ctx,pMaterial->GetK(),curIdx+1));
                    }else if(curKey == constDef::uroughness)
                    {
                        ctx.props.SetString(prefix + constDef::uroughness,texManager.updateTexture(
                            ctx,pMaterial->GetNu(),curIdx+1));
                    }else if(curKey == constDef::vroughness)
                    {
                        ctx.props.SetString(prefix + constDef::vroughness,texManager.updateTexture(
                            ctx,pMaterial->GetNv(),curIdx+1));
                    }else{
                        BOOST_ASSERT(false);
                    }
                }
                break;
            default:
                BOOST_ASSERT_MSG(false,"unreachable code");
            }
        }
    }
    return true;
}


void
ExtraMaterialManager::createMaterial(ImportContext &ctx,std::string& id,type_xml_node &xmlnode)
{
    slg::Scene  *scene = ctx.scene();//Doc::instance().pDocData->getSession()->renderConfig->scene;
    const char* name = NULL;
    const char* type = NULL;

    type_xml_attr*  pAttr = xmlnode.first_attribute();
    //shall we need cache attribute/childs to hash map?
    while(pAttr)
    {
        if(boost::iequals(constDef::id,pAttr->name()))
        {
            if(id.length() == 0)
                id = pAttr->value();
        }else if(boost::iequals(constDef::name,pAttr->name()))
        {
            name = pAttr->value();
        }else if(boost::iequals(constDef::type,pAttr->name()))
        {
            type = pAttr->value();
        }
        pAttr = pAttr->next_attribute();
    }

    //确保id有效。
    if(id.length() == 0)
    {
        id = string::uuid_to_string(boost::uuids::random_generator()());
    }
    //如果有，写入name.
    if(name)
        this->m_id2name[id] = name;

    std::stringstream   ss;
    if(type)
    {
        int typeidx = DocMat::getTypeFromTypeName(type);
        if(typeidx != DocMat::INVALID_MATERIAL)
        {
            ss << "scene.materials." << id << ".type = " << type <<std::endl;

            ExtraTextureManager &texManager = Doc::instance().pDocData->texManager;

            type_xml_node   *emission = xmlnode.first_node(constDef::emission);
            if(emission)
            {
                ctx.addAction(slg::AREALIGHTS_EDIT);
                ss << "scene.materials." << id << "." << constDef::emission << " = "<< texManager.createTexture(ctx,*emission) << std::endl;
            }
            appendTexture(ctx,id,xmlnode,ss,constDef::bumptex,texManager);
            appendTexture(ctx,id,xmlnode,ss,constDef::normaltex,texManager);

            switch(typeidx)
            {
            case slg::MATTE:
                appendTexture(ctx,id,xmlnode,ss,constDef::kd,texManager);
                break;
            case slg::MIRROR:
                appendTexture(ctx,id,xmlnode,ss,constDef::kr,texManager);
                break;
            case slg::GLASS:
                appendTexture(ctx,id,xmlnode,ss,constDef::kr,texManager);
                appendTexture(ctx,id,xmlnode,ss,constDef::kt,texManager);
                appendTexture(ctx,id,xmlnode,ss,constDef::ioroutside,texManager);
                appendTexture(ctx,id,xmlnode,ss,constDef::iorinside,texManager);
                break;
            case slg::METAL:
                appendTexture(ctx,id,xmlnode,ss,constDef::kr,texManager);
                appendTexture(ctx,id,xmlnode,ss,constDef::exp,texManager);
                break;
            case slg::ARCHGLASS:
                appendTexture(ctx,id,xmlnode,ss,constDef::kr,texManager);
                appendTexture(ctx,id,xmlnode,ss,constDef::kt,texManager);
                appendTexture(ctx,id,xmlnode,ss,constDef::ioroutside,texManager);
                appendTexture(ctx,id,xmlnode,ss,constDef::iorinside,texManager);
                break;
            case slg::MIX:
                {
                    ///@todo 额外处理material1
                    std::string     ida,idb;
                    bool    bAdded = false;
                    type_xml_node   *pChildA = xmlnode.first_node(constDef::material);
                    if(pChildA)
                    {
                        type_xml_node *pChildB = pChildA->next_sibling(constDef::material);
                        if(pChildB)
                        {
                            bAdded = true;
                            createMaterial(ctx,ida,*pChildA);
                            createMaterial(ctx,idb,*pChildB);
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
                            const std::string  &A = (bSwap ? idb : ida);
                            const std::string  &B = (bSwap ? ida : idb);
                            ss << "scene.materials." << id << ".material1 = " << A <<std::endl;
                            ss << "scene.materials." << id << ".material2 = " << B <<std::endl;
                            appendTexture(ctx,id,xmlnode,ss,constDef::amount,texManager);
                        }
                    }
                    if(!bAdded)
                    {
                        std::cerr << "invalid mix material,degrade to grey matte material." << std::endl;
                        ss.clear();
                    }
                }
                break;
            case slg::NULLMAT:
                break;
            case slg::MATTETRANSLUCENT:
                appendTexture(ctx,id,xmlnode,ss,constDef::kr,texManager);
                appendTexture(ctx,id,xmlnode,ss,constDef::kt,texManager);
                break;
            case slg::GLOSSY2:
                {
                    appendTexture(ctx,id,xmlnode,ss,constDef::kd,texManager);
                    appendTexture(ctx,id,xmlnode,ss,constDef::ks,texManager);
                    appendTexture(ctx,id,xmlnode,ss,constDef::uroughness,texManager);
                    appendTexture(ctx,id,xmlnode,ss,constDef::vroughness,texManager);
                    appendTexture(ctx,id,xmlnode,ss,constDef::ka,texManager);
                    appendTexture(ctx,id,xmlnode,ss,constDef::d,texManager);
                    appendTexture(ctx,id,xmlnode,ss,constDef::index,texManager);
                    type_xml_attr   *pAttr = xmlnode.first_attribute("multibounce");
                    if(pAttr)
                    {
                        ss << "scene.materials." << id << ".multibounce = " << pAttr->value() <<std::endl;
                    }
                }
                break;
            case slg::METAL2:
                {
                    appendTexture(ctx,id,xmlnode,ss,constDef::uroughness,texManager);
                    appendTexture(ctx,id,xmlnode,ss,constDef::vroughness,texManager);
                    appendTexture(ctx,id,xmlnode,ss,constDef::n,texManager);
                    appendTexture(ctx,id,xmlnode,ss,constDef::k,texManager);
                }
                break;
            default:
                BOOST_ASSERT_MSG(false,"unrachable code!");
                break;
            }

            if(!isMaterialTypeCompiled(typeidx))
            {
                ctx.addAction(slg::MATERIAL_TYPES_EDIT);
            }
        }else{
            ss << "scene.materials." << id << ".type = matte" << std::endl;
            ss << "scene.materials." << id << ".kd = 0.75 0.75 0.75" << std::endl;
        }
    }

    scene->DefineMaterials(ss.str());
    ctx.addAction(slg::MATERIALS_EDIT);

    m_mat2id[scene->matDefs.GetMaterial(id)] = id;

}


void
ExtraMaterialManager::createGrayMaterial(slg::Scene *scene,const std::string& id)
{
    BOOST_ASSERT_MSG(!id.empty(),"empty material id is not allowed.");
    std::stringstream   ss;
    ss << "scene.materials." << id << ".type = matte" << std::endl;
    ss << "scene.materials." << id << ".kd = 0.75 0.75 0.75" << std::endl;
    scene->DefineMaterials(ss.str());
    m_mat2id[scene->matDefs.GetMaterial(id)] = id;
}

void
ExtraMaterialManager::createGrayMaterial(ImportContext &ctx,const std::string &id)
{
    createGrayMaterial(ctx.scene(),id);
    ctx.addAction(slg::MATERIALS_EDIT);
    if(!isMaterialTypeCompiled(slg::MATTE))
    {
        ctx.addAction(slg::MATERIAL_TYPES_EDIT);
    }
}

bool
ExtraMaterialManager::isMaterialTypeCompiled(int matType)
{
    slg::RenderSession  *session = Doc::instance().pDocData->m_session.get();
    if(session && session->renderEngine && !session->renderEngine->IsMaterialCompiled((const slg::MaterialType)matType) )
    {
        return false;
    }
    return true;
}



void
ExtraMaterialManager::createMatteMaterial(ImportContext &ctx,const std::string& id,const std::string& name,const std::string &kdpath,const char* emissionPath,const char* normalPath)
{
    BOOST_ASSERT(id.length() > 0);
    std::cerr << "createMatteMaterial = " << kdpath << std::endl;

    std::string  kdId;
    ExtraTextureManager &texManager = Doc::instance().pDocData->texManager;
    if(texManager.defineImageMapTexture(ctx,kdpath,kdId))
    {
        std::stringstream       ss;
        ss << "scene.materials." << id << ".type = matte" << std::endl;
        ss << "scene.materials." << id << ".kd = " << kdId << std::endl;

        std::string  emID;
        if(emissionPath && texManager.defineImageMapTexture(ctx,emissionPath,emID))
        {
            ss << "scene.materials." << id << ".emission = " << emID << std::endl;
        }

        std::string normalId;
        if(normalPath && texManager.defineImageMapTexture(ctx,normalPath,normalId))
        {
            ss << "scene.materials." << id << ".normaltex = " << normalId << std::endl;
        }
        ctx.scene()->DefineMaterials(ss.str());
        m_mat2id[ctx.scene()->matDefs.GetMaterial(id)] = id;
        if(name.length())
            m_id2name[id] = name;
        ctx.addAction(slg::MATERIALS_EDIT);
        if(!isMaterialTypeCompiled(slg::MATTE))
        {
            ctx.addAction(slg::MATERIAL_TYPES_EDIT);
        }
    }
    else{
        createGrayMaterial(ctx,id);
    }
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
