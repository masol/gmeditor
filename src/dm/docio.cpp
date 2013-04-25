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
#include "dm/docio.h"
#include "dm/objectnode.h"
#include "utils/pathext.h"
#include "utils/option.h"
#include "utils/rapidxml_print.hpp"
#include <slg/slg.h>
#include <luxrays/utils/properties.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/scope_exit.hpp>
#include <boost/format.hpp>
#include <fstream>
#include "eigenutil.h"
#include "slgutils.h"
#include "docprivate.h"
#include "slgobject.h"
#include "slgmaterial.h"
#include "slgtexture.h"
#include "dm/setting.h"

#define __CL_ENABLE_EXCEPTIONS 1
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

/** @brief 初始化时如果未定义摄像机，则显示全部内容。
**/
static  inline
void    initViewAllCamera(slg::Scene *scene,float fov = 60.0f)
{
    BOOST_ASSERT_MSG(scene->dataSet == NULL, "wrong usage initViewAllCamera");
    luxrays::BBox   box;
    std::vector<luxrays::ExtMesh *>::iterator    it = scene->extMeshCache.meshes.begin();
    while(it != scene->extMeshCache.meshes.end())
    {
        box = luxrays::Union(box,(*it)->GetBBox());
        it++;
    }
    luxrays::BSphere    bs = box.BoundingSphere();
    std::stringstream   ss;

    float aspect = 640.0f / 480.0f;
    float aspectradius = bs.rad / (aspect < 1.0f ? 1.0f : aspect);
    ///@fixme can not get aspect here,how to do?
    float  radius = aspectradius / std::tan(((fov / 180.0 ) * boost::math::constants::pi<float>() ) / 2.0);
    ss <<"scene.camera.lookat = " << boost::format("%f %f %f  %f %f %f")
                            % (bs.center.x  + radius) % (bs.center.y  + radius)% (bs.center.z  + radius)
                            % bs.center.x % bs.center.y % bs.center.z << std::endl;
    ss << "scene.camera.fieldofview = " << fov <<std::endl;
    scene->CreateCamera(ss.str());
}

namespace gme
{

void
DocIO::loadExtraFromSlgSceneFile(const std::string pathstring)
{
    //从场景文件中加载全部的材质定义并保存。
    std::ifstream file(pathstring.c_str(),std::ifstream::binary);
    if (file)
    {
        BOOST_SCOPE_EXIT( (&file) )
        {
            file.close();
        }
        BOOST_SCOPE_EXIT_END

        luxrays::Properties props;
        props.Load(file);

        //获取每个模型对象的原始文件信息。
        pDocData->objManager.loadExtraFromProps(props);
        //保存每个贴图对应的原始文件信息。
        pDocData->texManager.loadExtraFromProps(props);
    }
}

void
DocIO::loadExtraFromScene(void)
{
    slg::Scene  *scene = pDocData->getSession()->renderConfig->scene;
    std::vector<std::string> names = scene->meshDefs.GetExtMeshNames();

    for(std::vector<std::string>::const_iterator iter = names.begin(); iter < names.end(); ++iter)
    {
        luxrays::ExtMesh * extmesh = scene->meshDefs.GetExtMesh((*iter));
        BOOST_ASSERT_MSG(extmesh != NULL,"Mesh Array Panic?");
        ObjectNode  obj;
        obj.m_id = (*iter);
        obj.m_name = obj.m_id;

        //SLG中有缺陷。namearray的index不同于matarray!所以下面的检查是错误的。请查看上文如何获取的材质名称以了解nameidx和matidx之间的换算。
        //BOOST_ASSERT_MSG(scene->matDefs.GetMaterial(materialInfo.m_slgname) == scene->objectMaterials[i], "material panic");
        u_int meshIdx = scene->meshDefs.GetExtMeshIndex(extmesh);
        obj.m_matid = pDocData->matManager.getMaterialId(scene->objectMaterials[meshIdx]);

        pDocData->matManager.get(obj.m_matid) = obj.m_matid;

        //所有从slg读取的场景，不支持normal。
        obj.m_useplynormals = false;//extmesh->HasNormals();
        luxrays::ExtInstanceTriangleMesh* pinst = dynamic_cast<luxrays::ExtInstanceTriangleMesh*>(extmesh);
        if(pinst)
        {
            EigenUtil::AssignFromSlgMatrix(obj.m_transformation,pinst->GetTransformation().m);
        }

        pDocData->objManager.getRoot().addChild(obj);
    }
}

bool
DocIO::loadSlgScene(const std::string &path)
{
    try
    {
        pDocData->closeScene();
        luxrays::Properties cmdLineProp;
        //我们需要自动应用本地的渲染配置(例如平台选择以及设备选择,未指定的话采用本地配置文件，未配置的自动使用最大集合)。
        {
            int	platformId = 0;
            if(Option::instance().is_existed(Setting::OPT_PLATFORMID))
            {
                platformId = boost::lexical_cast<int>(Option::instance().get<std::string>(Setting::OPT_PLATFORMID));
            }
            cmdLineProp.SetString(Setting::OPT_PLATFORMID,boost::lexical_cast<std::string>(platformId));

            if(Option::instance().is_existed(Setting::OPT_DEVICESTR))
            {
                cmdLineProp.SetString(Setting::OPT_DEVICESTR,Option::instance().get<std::string>(Setting::OPT_DEVICESTR));
            }else{
                std::string	full = clHardwareInfo::instance().getFullSelectString(platformId);
                cmdLineProp.SetString(Setting::OPT_DEVICESTR,full);
            }
        }

        slg::RenderConfig *config = new slg::RenderConfig(&path, &cmdLineProp);
        pDocData->m_session.reset(new slg::RenderSession(config));

        //第一步必须把反向表建立起来，后面依赖本表做数据更新。
        pDocData->matManager.appendMat2IdFromSlg();
        pDocData->texManager.appendTex2IdFromSlg();

        loadExtraFromScene();
        std::string scnFile = pDocData->m_session->renderConfig->cfg.GetString("scene.file","");
        if(scnFile.length())
        {
            //slg以当前路径为拼接原则，而不是相对于文件。
            loadExtraFromSlgSceneFile(boost::filesystem::canonical(scnFile/*,boost::filesystem::absolute(path.parent_path())*/).string());
        }
        pDocData->m_session->Start();
        pDocData->m_started = true;
        return true;
    }
    catch(cl::Error err)
    {
        std::cerr << err.what() << "(" << err.err() << ")" << std::endl;
        pDocData->m_session.reset();
    }
    return false;
}

bool
DocIO::loadSpsScene(const std::string &path)
{
    try
    {
        pDocData->closeScene();

        slg::Scene *scene = new slg::Scene();

		// Setup the camera
//		scene->CreateCamera(
//			"scene.camera.lookat = 1.0 6.0 3.0  0.0 0.0 0.5\n"
//			"scene.camera.fieldofview = 60.0\n"
//			);

        ImportContext   ctx(scene);

        //首先，尝试将path指定的资源加载。
        int count = ExtraObjectManager::importSpScene(path,pDocData->objManager.getRoot(),ctx);

        if(count > 0)
        {//如果加载数量大于0才继续。
            initViewAllCamera(scene,60.0f);

            scene->AddSkyLight(
                    "scene.skylight.dir = 0.166974 0.59908 0.783085\n"
                    "scene.skylight.turbidity = 2.2\n"
                    "scene.skylight.gain = 0.8 0.8 0.8\n"
                    );
            scene->AddSunLight(
                    "scene.sunlight.dir = 0.166974 0.59908 0.783085\n"
                    "scene.sunlight.turbidity = 2.2\n"
                    "scene.sunlight.gain = 0.8 0.8 0.8\n"
                    );

            slg::RenderConfig *config = new slg::RenderConfig(
				"image.width = 640\n"
				"image.height = 480\n"
				"opencl.platform.index = 0\n"
				"opencl.cpu.use = 0\n"
				"opencl.gpu.use = 1\n"
				"opencl.gpu.workgroup.size = 64\n"
				"path.maxdepth = 8\n"
				"path.russianroulette.depth = 5\n"
				"batch.halttime = 0\n",
				*scene);
            pDocData->m_session.reset(new slg::RenderSession(config));
            pDocData->m_session->Start();
            pDocData->m_started = true;
            return true;
        }else{
            delete scene;
        }
#if 0
        luxrays::Properties cmdLineProp;
        //我们需要自动应用本地的渲染配置(例如平台选择以及设备选择,未指定的话采用本地配置文件，未配置的自动使用最大集合)。
        {
            int	platformId = 0;
            if(Option::instance().is_existed(Setting::OPT_PLATFORMID))
            {
                platformId = boost::lexical_cast<int>(Option::instance().get<std::string>(Setting::OPT_PLATFORMID));
            }
            cmdLineProp.SetString(Setting::OPT_PLATFORMID,boost::lexical_cast<std::string>(platformId));

            if(Option::instance().is_existed(Setting::OPT_DEVICESTR))
            {
                cmdLineProp.SetString(Setting::OPT_DEVICESTR,Option::instance().get<std::string>(Setting::OPT_DEVICESTR));
            }else{
                std::string	full = clHardwareInfo::instance().getFullSelectString(platformId);
                cmdLineProp.SetString(Setting::OPT_DEVICESTR,full);
            }
        }

        slg::RenderConfig *config = new slg::RenderConfig(&path, &cmdLineProp);
        pDocData->m_session.reset(new slg::RenderSession(config));

        //第一步必须把反向表建立起来，后面依赖本表做数据更新。
        pDocData->matManager.appendMat2IdFromSlg();
        pDocData->texManager.appendTex2IdFromSlg();

        loadExtraFromScene();
        std::string scnFile = pDocData->m_session->renderConfig->cfg.GetString("scene.file","");
        if(scnFile.length())
        {
            //slg以当前路径为拼接原则，而不是相对于文件。
            loadExtraFromSlgSceneFile(boost::filesystem::canonical(scnFile/*,boost::filesystem::absolute(path.parent_path())*/).string());
        }
        pDocData->m_session->Start();
        pDocData->m_started = true;
        return true;
#endif
    }
    catch(cl::Error err)
    {
        std::cerr << err.what() << "(" << err.err() << ")" << std::endl;
        pDocData->m_session.reset();
    }
    return false;
}



bool
DocIO::loadScene(const std::string &path)
{
    if(boost::iends_with(path,".cfg"))
    {
        return loadSlgScene(path);
    }
    else if(boost::iends_with(path,".sps"))
    {
        return loadSpsScene(path);
    }else if(boost::iends_with(path,".ctm"))
    {//load open ctm format.
    }else{
    //load assimp format.
    }
    return false;
}

bool
DocIO::exportSpoloScene(const std::string &pathstring,bool bExportRes)
{
    boost::filesystem::path path(pathstring);
    boost::filesystem::path root_path = path.parent_path();

    std::ofstream   ofstream;
    ofstream.open(pathstring.c_str());
    if(ofstream.is_open())
    {
        BOOST_SCOPE_EXIT( (&ofstream))
        {
            ofstream.close();
        }
        BOOST_SCOPE_EXIT_END

        ///boost自带的rapidxml没有print函数.....加入到utils中。
        ofstream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
        ofstream << "<scene>" << std::endl;

//        //write scene config.
//        ofstream << "<settings>" << std::endl;
//        ofstream << "</settings>" << std::endl;
//
//        //write camera.
//        //write tone-mapping.
//        //write light.
//        //write object.
        {
            std::ostream_iterator<char>   oit(ofstream);
            type_xml_doc    xmldoc;
            type_xml_node * pNode = xmldoc.allocate_node(NS_RAPIDXML::node_element,"objects");
            xmldoc.append_node(pNode);

            int flags = dumpContext::DUMP_NORMAL;
            if(bExportRes)
            {
                flags = (dumpContext::DUMP_SAVECTM | dumpContext::DUMP_COPYRES);
            }
            dumpContext     ctx(flags);
            pDocData->objManager.dump(*pNode,ctx);
            rapidxml::print(oit,xmldoc);
        }
        ofstream << "</scene>" << std::endl;

        return true;
    }

    return false;
}

bool
DocIO::exportScene(const std::string &pathstring,bool bExportResource)
{
    bool    bExportOK = false;
    if(boost::iends_with(pathstring,".sps"))
    {
        //export sp scene file.
        bExportOK = exportSpoloScene(pathstring,bExportResource);
    }
    else if(boost::iends_with(pathstring,".cfg"))
    {
        //export slg scene file.
    }
    else if(boost::iends_with(pathstring,".ocs"))
    {
        //export octane scene file.
    }
    else if(boost::iends_with(pathstring,".vray"))
    {
        //export vray scene file.
    }
    else if(boost::iends_with(pathstring,".blend"))
    {
        //export blend scene with cycles material file.
    }

    return bExportOK;
}

bool
DocIO::importScene(const std::string &path,ObjectNode *pParent)
{
    ImportContext   ctx(pDocData->m_session->renderConfig->scene);
    SlgUtil::Editor editor(pDocData->m_session.get());
    if(!pParent)
    {
        pParent = &pDocData->objManager.getRoot();
    }

    bool    bLoadSuc = false;
    if(boost::iends_with(path,".sps"))
    {//importSpScene中假定传入Parent对象。
        int count = ExtraObjectManager::importSpScene(path,*pParent,ctx);
        bLoadSuc = (count > 0);
    }else{
        ObjectNode  node;
        if(pDocData->objManager.importObjects(path,node,ctx))
        {
            pParent->addChild(node);
            bLoadSuc = true;
        }
    }

    if(bLoadSuc)
        editor.addAction(ctx.getAction());

    return bLoadSuc;
}


bool
DocIO::deleteModel(const std::string &id)
{
    return pDocData->objManager.removeMesh(id);
}


} //end namespace gme.
