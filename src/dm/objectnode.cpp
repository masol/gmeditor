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
#include "dm/doc.h"
#include "dm/objectnode.h"
#include "dm/docsetting.h"
#include "docprivate.h"
#include "slg/slg.h"
#include <boost/filesystem/fstream.hpp>
#include <boost/scope_exit.hpp>
#include "openctm/openctm.h"

//include OpenGL
#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

static
CTMuint CTMCALL
FStream_CTMwritefn(const void * aBuf, CTMuint aCount, void * aUserData)
{
    boost::filesystem::ofstream *pStream = (boost::filesystem::ofstream*)aUserData;
    pStream->write((const char*)aBuf,aCount);
    return aCount;
}

template<class T>
bool    SaveCtmFile(bool &useplynormals,T *pMesh,const std::string &filename,gme::conditional_md5 &md5)
{
    boost::filesystem::ofstream stream(filename,std::ios::out | std::ios::binary);
    if(!stream)
        return false;

    CTMcontext context = NULL;
    CTMuint    vertCount, triCount, * indices;
    CTMfloat   *vertices;
    CTMfloat   *aNormals = NULL;
    CTMfloat   *aUVCoords = NULL;

    vertCount = pMesh->GetTotalVertexCount ();
    triCount = pMesh->GetTotalTriangleCount ();
    vertices  = (CTMfloat*)(void*)pMesh->GetVertices();
    indices  = (CTMuint*)(void*)pMesh->GetTriangles();

    md5.update((const unsigned char *)(void*)vertices,vertCount * 3 * sizeof(CTMfloat));
    md5.update((const unsigned char *)(void*)indices,triCount * 3 * sizeof(CTMuint));

    if(useplynormals)//pMesh->HasNormals())
    {
        GME_TRACE("has normals ... ");
        aNormals = new CTMfloat[vertCount * 3];
        for(unsigned int idx = 0 ; idx < vertCount; idx++)
        {
            luxrays::Normal n = pMesh->GetShadeNormal(idx);
            aNormals[idx * 3] = n.x;
            aNormals[idx * 3 + 1] = n.y;
            aNormals[idx * 3 + 2] = n.z;
            if(boost::math::isfinite(n.x) || boost::math::isfinite(n.y) || boost::math::isfinite(n.z) )
            {
                gme::Doc::SysLog(gme::Doc::LOG_WARNING,boost::str(boost::format(__("发现法线中有无限值，保存模型到文件'%s'时忽略法线数据。")) % filename ) );
                delete[] aNormals;
                aNormals = NULL;
                useplynormals = false;
                break;
            }
        }
        if(aNormals)
            md5.update((const unsigned char *)(void*)aNormals,vertCount * 3 * sizeof(CTMfloat));
    }

    context = ctmNewContext(CTM_EXPORT);
    ctmDefineMesh(context, vertices, vertCount, indices, triCount, aNormals);
    if(pMesh->HasUVs())
    {
        aUVCoords = new CTMfloat[vertCount * 2];
        for(unsigned int idx = 0 ; idx < vertCount; idx++)
        {
            luxrays::UV uv = pMesh->GetUV(idx);
            aUVCoords[idx * 2] = uv.u;
            aUVCoords[idx * 2 + 1] = uv.v;
        }
        ctmAddUVMap(context,aUVCoords,"def",NULL);
    }

    //ctmSave(context, filename.c_str());
    ctmSaveCustom(context,FStream_CTMwritefn,&stream);

    if(ctmGetError(context) != CTM_NONE)
    {
        gme::Doc::SysLog(gme::Doc::LOG_ERROR,boost::str(boost::format(__("保存模型到文件'%s'时失败。")) % filename ) );
    }

    //ctxHashValue = md5.finalize().hexdigest();

    if(aNormals)
        delete[] aNormals;
    if(aUVCoords)
        delete[] aUVCoords;
    if(context)
        ctmFreeContext(context);

    stream.close();

    return true;
}

static
bool    SaveCtmFile(bool &useplynormals,luxrays::ExtMesh* extMesh,const std::string &filename,gme::conditional_md5 &md5)
{
    luxrays::ExtInstanceTriangleMesh*   pMesh = dynamic_cast<luxrays::ExtInstanceTriangleMesh*>(extMesh);
    if(pMesh)
    {
        return SaveCtmFile<luxrays::ExtInstanceTriangleMesh>(useplynormals,pMesh,filename,md5);
    }
    luxrays::ExtTriangleMesh*   pMesh2 = dynamic_cast<luxrays::ExtTriangleMesh*>(extMesh);
    if(pMesh2)
    {
        return SaveCtmFile<luxrays::ExtTriangleMesh>(useplynormals,pMesh2,filename,md5);
    }
    return false;
}

namespace gme{

const char* constDef::object = "object";
const char* constDef::material = "material";
const char* constDef::texture = "texture";
const char* constDef::id = "id";
const char* constDef::name = "name";
const char* constDef::type = "type";
const char* constDef::mapping = "mapping";
const char* constDef::ctxmd5 = "ctxmd5";
const char* constDef::transformation = "transformation";
const char* constDef::position = "position";
const char* constDef::file = "file";
const char* constDef::groupfile = "groupfile";
const char* constDef::emission = "emission";
const char* constDef::bumptex = "bumptex";
const char* constDef::normaltex = "normaltex";
const char* constDef::kd = "kd";
const char* constDef::kr = "kr";
const char* constDef::kt = "kt";
const char* constDef::ioroutside = "ioroutside";
const char* constDef::iorinside = "iorinside";
const char* constDef::exp = "exp";
const char* constDef::amount = "amount";
const char* constDef::ks = "ks";
const char* constDef::uroughness = "uroughness";
const char* constDef::vroughness = "vroughness";
const char* constDef::ka = "ka";
const char* constDef::d = "d";
const char* constDef::index = "index";
const char* constDef::n = "n";
const char* constDef::k = "k";
const char* constDef::camera = "camera";
const char* constDef::active = "active";
const char* constDef::target = "target";
const char* constDef::up = "up";
const char* constDef::fieldOfView = "fieldOfView";
const char* constDef::clipHither = "clipHither";
const char* constDef::clipYon = "clipYon";
const char* constDef::lensRadius = "lensRadius";
const char* constDef::focalDistance = "focalDistance";
const char* constDef::envlight = "envlight";
const char* constDef::sunlight = "sunlight";
const char* constDef::lights = "lights";
const char* constDef::film = "film";
const char* constDef::alpha = "alpha";
const char* constDef::setting = "setting";
const char* constDef::width = "width";
const char* constDef::height = "height";
const char* constDef::gamma = "gamma";
const char* constDef::filter = "filter";
const char* constDef::renderengine = "renderengine";
const char* constDef::tonemapping = "tonemapping";
const char* constDef::scale = "scale";
const char* constDef::prescale = "prescale";
const char* constDef::postscale = "postscale";
const char* constDef::burn = "burn";
const char* constDef::usenormal = "usenormal";
const char* constDef::truevalue = "true";
const char* constDef::falsevalue = "false";
const char* constDef::pass = "pass";
const char* constDef::sys = "sys";
const char* constDef::sampler = "sampler";
const char* constDef::path = "path";
const char* constDef::roulette = "roulette";
const char* constDef::roulettecap = "roulettecap";


bool    ObjectNode::sv_exprtNewMesh = false;

ObjectNode*
ObjectNode::findObject(const std::string &id,ObjectNodePath *pPath)
{
    if(pPath)
        pPath->push(this);
    if(this->id() == id)
        return this;
    ObjectNode* result = NULL;
    type_child_container::iterator  it = this->begin();
    while(it != this->end())
    {
        result = it->findObject(id,pPath);
        if(result)
        {
            break;
        }
        it++;
    }
    if(pPath && !result)
        pPath->pop();
    return result;
}


void
ObjectNode::applyMatrix(Eigen::Matrix4f &matrix)
{
}

ObjectNode::ObjectNode()
{
    m_useplynormals = !DocSetting::ignoreNormals();
    m_alpha = 1.0f;
    //创建者需要自行维护nil.
//  m_id = boost::uuids::nil_generator()();
//  m_matid = boost::uuids::nil_generator()();
}

void
ObjectNode::drawSelf(void)
{
    if(!this->matid().empty())
    {
        luxrays::ExtMesh *pMesh = ExtraObjectManager::getExtMesh(this->id());
        if(pMesh)
        {
            ///TODO: use vbo & displaylist to improve the effective.
//            unsigned int vertCount = pMesh->GetTotalVertexCount();
            unsigned int triCount = pMesh->GetTotalTriangleCount();

            luxrays::Point *ppt = pMesh->GetVertices();
            luxrays::Triangle *ptri = pMesh->GetTriangles();

#if 0
            glBegin(GL_TRIANGLES);

            for(unsigned int idx = 0; idx < triCount; idx++)
            {
                luxrays::Normal n = pMesh->GetGeometryNormal(idx);
//                glNormal3f(n.x,n.y,n.z);
                for(unsigned int i = 0; i < 3; i++)
                {
                    unsigned vidx = ptri[idx].v[i];
                    luxrays::Point  &pt = ppt[vidx];
                    glColor3f(1,1,1);
                    glVertex3f(pt.x,pt.y,pt.z);
                }
            }

            glEnd();
#else
// we can not use glVertexPointer, slg is left-handed,but opengl use right-handed coordinate....
            glVertexPointer(3,GL_FLOAT,0,ppt);
            glDrawElements(GL_TRIANGLES,triCount * 3,GL_UNSIGNED_INT,ptri);
#endif
        }
    }
}

void
ObjectNode::draw(const Eigen::Matrix4f &matrix)
{
    drawSelf();
    BOOST_FOREACH(ObjectNode &child,m_children)
    {
        child.draw(matrix);
    }
}

void
ObjectNode::unionBBox(luxrays::BBox  *pbox)
{
    if(!this->matid().empty())
    {
        luxrays::ExtMesh *pMesh = ExtraObjectManager::getExtMesh(this->id());
        if(pMesh)
        {
            *pbox = luxrays::Union(*pbox,pMesh->GetBBox());
        }
    }
    BOOST_FOREACH(ObjectNode &child,m_children)
    {
        child.unionBBox(pbox);
    }
}



type_xml_node*
ObjectNode::dump(type_xml_node &parent,dumpContext &ctx)
{
    type_xml_doc  *pDoc = parent.document();
    BOOST_ASSERT_MSG(pDoc,"node usage error!");
    type_xml_node *pSelf = pDoc->allocate_node(NS_RAPIDXML::node_element, constDef::object);

    parent.append_node(pSelf);
    pSelf->append_attribute(pDoc->allocate_attribute(constDef::id,allocate_string(pDoc,m_id)));

    if(this->m_name.length())
    {
        pSelf->append_attribute(pDoc->allocate_attribute(constDef::name,allocate_string(pDoc,m_name)));
    }

    if(!this->matid().empty())
    {//只有matid不为空时才会有mesh。
	    luxrays::ExtMesh*   extMesh = Doc::instance().pDocData->objManager.getExtMesh(id());
        if(extMesh)
        {//只有模型存在，我们才继续输出与模型相关的信息。
            std::string     write_file;
            conditional_md5 md5(ctx);
            //boost::filesystem::path target_model = ctx.target / "mesh%%%%%%.ply";
            //只有在不是multiMesh时我们才可以保存。否则会引发下次加载的模型重复。从而导致材质无法配对。
            if(!this->filepath().empty())
            {//获取映射的文件名。
                if(ctx.isCopyResource())
                {//保存资源。
                    boost::filesystem::path target;
                    if(!sv_exprtNewMesh && boost::iends_with(this->filepath(),".ctm"))
                    {//文件已经是一个ctm，可以拷贝之。
                        boost::filesystem::path     origpath(this->filepath());
                        target = ctx.target;
                        target /= origpath.filename();
                        if(!boost::filesystem::exists(target))
                        {
                            boost::filesystem::copy(origpath,target);
                        }
                    }else{
		                boost::filesystem::path target_model = ctx.target / "mesh%%%%%%.ctm";
                        target = boost::filesystem::unique_path(target_model);
                        bool useplynormals = this->useplynormals();
                        SaveCtmFile(useplynormals,extMesh,target.string(),md5);
                        this->useplynormals(useplynormals);
                    }
                    write_file = target.filename().string();
                }else{//不保存资源，直接保存m_filepath.
                    write_file = this->filepath();
                }
            }else{
                if(ctx.isCopyResource())
                {
                    //没有定义文件名。此时直接保存资源。
	                boost::filesystem::path target_model = ctx.target / "mesh%%%%%%.ctm";
                    boost::filesystem::path target = boost::filesystem::unique_path(target_model);
                    //extMesh->WritePly(target.string());
                    bool usenormals = this->useplynormals();
                    SaveCtmFile(usenormals,extMesh,target.string(),md5);
                    this->useplynormals(usenormals);
                    write_file = target.filename().string();
                }
            }
            if(md5.isGenerateMD5())
            {
                std::string ctxHashValue = md5.hexdigest();
                pSelf->append_attribute(pDoc->allocate_attribute(constDef::ctxmd5,allocate_string(pDoc,ctxHashValue)));

                const std::string *pName = ctx.queryObjFilepath(ctxHashValue);
                if(pName)
                {
                    ///@todo 由于内容重复，删除刚保存的模型文件。 需要判断，不能删除外部文件。
                    if(ctx.isCopyResource())
                        boost::filesystem::remove(write_file);
                    write_file = *pName;
                }else{
                    ctx.addObjMapper(ctxHashValue,write_file);
                }
            }

            if(m_alpha < 1.0f)
            {
                std::string   alpha = boost::lexical_cast<std::string>(m_alpha);
                pSelf->append_attribute(pDoc->allocate_attribute(constDef::alpha,allocate_string(pDoc,alpha)));
            }

            pSelf->append_attribute(pDoc->allocate_attribute(constDef::usenormal,(m_useplynormals ? constDef::truevalue : constDef::falsevalue) ) );

            if(!write_file.empty())
            {//我们可能使用一个group model file.
                //BOOST_ASSERT_MSG(!ctx.isCopyResource(),"can not copy resource");
                pSelf->append_attribute(pDoc->allocate_attribute(constDef::file,allocate_string(pDoc,write_file)));
            }
            const slg::Material* pMat = ExtraMaterialManager::getSlgMaterial(this->matid());
            BOOST_ASSERT_MSG(pMat,"invalid ref material?");
            Doc::instance().pDocData->matManager.dump(*pSelf,pMat,ctx);
        }
    }else if(!this->filepath().empty() && !ctx.isCopyResource() )
    {
        pSelf->append_attribute(pDoc->allocate_attribute(constDef::file,allocate_string(pDoc,this->filepath())));
    }

    //不需要输出materialid.这个id在子节点中自己创建。

    ObjectNode::type_child_container::iterator  it = this->begin();
    while(it != this->end())
    {
        it->dump(*pSelf,ctx);
        it++;
    }

    return pSelf;
}

void
ObjectNode::onChildRemoved(const std::string &childid)
{
    Doc::instance().pDocData->removeSelection(childid);
    Doc::instance().pDocData->fireSelection(DocPrivate::SEL_ITEMSELFREMOVED,childid);
}


}
