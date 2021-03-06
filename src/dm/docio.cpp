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
#include "utils/i18n.h"
#include <slg/slg.h>
#include <luxrays/utils/properties.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/scope_exit.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/fstream.hpp>
#include "eigenutil.h"
#include "slgutils.h"
#include "docprivate.h"
#include "slgobject.h"
#include "slgmaterial.h"
#include "slgtexture.h"
#include "slgsetting.h"
#include "dm/setting.h"
#include "dm/localsetting.h"

#define __CL_ENABLE_EXCEPTIONS 1
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

static  inline
void    initCamera(slg::Scene *scene,const gme::Camera &cam)
{
    std::stringstream ss;
    ss << "scene.camera.lookat = " << boost::str(boost::format("%f %f %f  %f %f %f")
                            % cam.orig[0] % cam.orig[1]% cam.orig[2]
                            % cam.target[0] % cam.target[1] % cam.target[2] ) << std::endl;
    if(cam.up.norm() == 0.0f)
    {
        std::cerr << "error : cam up is invalid!";
        ss << "scene.camera.up = 0.0 0.0 0.1" << std::endl;
    }else
    {
        ss << "scene.camera.up = " << boost::format("%f %f %f") % cam.up[0] % cam.up[1]% cam.up[2] << std::endl;
    }

    if(!cam.isDefaultFieldOfView())
        ss <<"scene.camera.fieldofview = " << cam.fieldOfView << std::endl;
    if(!cam.isDefaultClipHither())
        ss <<"scene.camera.cliphither = " << cam.clipHither << std::endl;
    if(!cam.isDefaultClipYon())
        ss <<"scene.camera.clipyon = " << cam.clipYon << std::endl;
    if(!cam.isDefaultLensRadius())
        ss <<"scene.camera.lensradius = " << cam.lensRadius << std::endl;
    if(!cam.isDefaultFocalDistance())
        ss <<"scene.camera.focaldistance = " << cam.focalDistance << std::endl;

    std::cerr <<  ss.str();
    scene->CreateCamera(ss.str());
}

/** @brief 初始化时如果未定义摄像机，则显示全部内容。
**/
static  inline
void    initViewAllCamera(slg::Scene *scene,float width,float height)
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

    gme::Camera defCam = gme::Camera::getDefault();
    float aspect = width / height;
    float aspectradius = bs.rad / (aspect < 1.0f ? 1.0f : aspect);
    float  radius = aspectradius / std::tan(((defCam.fieldOfView / 180.0f ) * boost::math::constants::pi<float>() ) / 2.0f);

    defCam.orig[0] = (bs.center.x  + radius);
    defCam.orig[1] = (bs.center.y  + radius);
    defCam.orig[2] = (bs.center.z  + radius);

    defCam.target[0] = bs.center.x;
    defCam.target[1] = bs.center.y;
    defCam.target[2] = bs.center.z;

    defCam.focalDistance = std::abs( (defCam.target - defCam.orig).norm()) / 2;

    initCamera(scene,defCam);
}

namespace gme
{

void
DocIO::loadExtraFromSlgSceneFile(const std::string pathstring)
{
    //从场景文件中加载全部的材质定义并保存。
    boost::filesystem::ifstream file(pathstring.c_str(),std::ifstream::binary);
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
            EigenUtil::AssignFromSlg(obj.m_transformation,pinst->GetTransformation().m);
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
        //{
        //    int	platformId = 0;
        //    if(Option::instance().is_existed(Setting::OPT_PLATFORMID))
        //    {
        //        platformId = boost::lexical_cast<int>(Option::instance().get<std::string>(Setting::OPT_PLATFORMID));
        //    }
        //    cmdLineProp.SetString(Setting::OPT_PLATFORMID,boost::lexical_cast<std::string>(platformId));

        //    if(Option::instance().is_existed(Setting::OPT_DEVICESTR))
        //    {
        //        cmdLineProp.SetString(Setting::OPT_DEVICESTR,Option::instance().get<std::string>(Setting::OPT_DEVICESTR));
        //    }else{
        //        std::string	full = clHardwareInfo::instance().getFullSelectString(platformId);
        //        cmdLineProp.SetString(Setting::OPT_DEVICESTR,full);
        //    }
        //}

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
        pDocData->start();
        return true;
    }
    catch(cl::Error err)
    {
        Doc::SysLog(Doc::LOG_ERROR,boost::str(boost::format(__("无法加载场景:%s。失败原因%s:(%s)")) % path % err.what() % err.err() ) );
        pDocData->m_session.reset();
    }catch(std::runtime_error err)
    {
        Doc::SysLog(Doc::LOG_ERROR,boost::str(boost::format(__("无法加载场景:%s。失败原因:'%s'")) % path % err.what()) );
        pDocData->m_session.reset();
    }
    return false;
}

void
DocIO::initAndStartScene(slg::Scene *scene,ImportContext *pctx)
{
    const ImportContext &ctx = *pctx;
    Camera *pLoadedCam = pDocData->camManager.getSelected();

    float width,height;
    if( !ctx.m_width.empty() && !ctx.m_height.empty() )
    {
        width = boost::lexical_cast<float>(ctx.m_width);
        height = boost::lexical_cast<float>(ctx.m_height);
    }else{
        gme::LocalSetting::Film::getSize(width,height);
    }

    if(pLoadedCam && pLoadedCam->isValid())
    {//camera already loaded.
        initCamera(scene,*pLoadedCam);
    }else{ // create default camera.
        initViewAllCamera(scene,width,height);
    }

    //如果scene中没有定义灯光，继续定义灯光。
    if(!scene->envLight && !scene->sunLight)
    {
        bool    hasLight = false;
        //gme::LocalSetting::EnvironmentHDR::setFile("/home/gmeditor/build/scenes/simple-mat/arch.exr");
        if(gme::LocalSetting::EnvironmentHDR::hasHDR())
        {
            std::stringstream   ss;

            ss << "scene.infinitelight.file = " << gme::LocalSetting::EnvironmentHDR::getFile() << std::endl;
            float gamma = gme::LocalSetting::EnvironmentHDR::getGamma();
            if(!gme::LocalSetting::EnvironmentHDR::isDefaultGamma(gamma))
            {
                ss << "scene.infinitelight.gamma = " << gamma << std::endl;
            }
            ss << "scene.infinitelight.gain = " << gme::LocalSetting::EnvironmentHDR::getGain() << std::endl;
            scene->AddInfiniteLight(ss.str());
            hasLight = true;
        }

        if(gme::LocalSetting::EnvironmentSky::hasSky())
        {
            std::stringstream   ss;

            ss << "scene.skylight.dir = " << gme::LocalSetting::EnvironmentSky::getDir() << std::endl;
            float t = gme::LocalSetting::EnvironmentSky::getTurbidity();
            if(!gme::LocalSetting::EnvironmentSky::isDefaultTurbidity(t))
            {
                ss << "scene.skylight.turbidity = " << t << std::endl;
            }
            ss << "scene.skylight.gain = " << gme::LocalSetting::EnvironmentSky::getGain() << std::endl;
            scene->AddSkyLight(ss.str());
            hasLight = true;
        }

        if(gme::LocalSetting::EnvironmentSun::hasSun())
        {
            std::stringstream   ss;

            ss << "scene.sunlight.dir = " << gme::LocalSetting::EnvironmentSun::getDir() << std::endl;
            float t = gme::LocalSetting::EnvironmentSun::getTurbidity();
            if(!gme::LocalSetting::EnvironmentSun::isDefaultTurbidity(t))
            {
                ss << "scene.sunlight.turbidity = " << t << std::endl;
            }
            ss << "scene.sunlight.gain = " << gme::LocalSetting::EnvironmentSun::getGain() << std::endl;
            scene->AddSunLight(ss.str());
            hasLight = true;
        }


        if(!hasLight)
        {
            scene->AddSkyLight(
                    "scene.skylight.dir = 1.0 1.0 1.0\n"
                    "scene.skylight.turbidity = 2.2\n"
                    "scene.skylight.gain = 1.0 1.0 1.0\n"
                    );
    //        scene->AddSunLight(
    //                "scene.sunlight.dir = 0.166974 0.59908 0.783085\n"
    //                "scene.sunlight.turbidity = 2.2\n"
    //                "scene.sunlight.gain = 0.8 0.8 0.8\n"
    //                );
        }
    }

    std::stringstream     confgSS;
    confgSS << "image.width = " << (int)width << std::endl;
    confgSS << "image.height = " << (int)height << std::endl;
    confgSS << "opencl.platform.index = " << -1 << std::endl;
    confgSS << "opencl.cpu.use = " << 0 << std::endl;
    confgSS << "opencl.gpu.use = " << 1 << std::endl;
    if(clHardwareInfo::instance().getPlatforms().size() == 0)
    {//no opencl found. rollback to cpu mode.
        Doc::SysLog(Doc::LOG_WARNING,__("找不到opencl环境，使用CPU渲染，这将会严重影响您的体验，请检查您的硬件环境以及驱动设置(可以使用GPU-Z来检查OPENCL环境是否就绪)。"));
        confgSS << "renderengine.type = PATHCPU" << std::endl;
    }else if(!ctx.m_renderengine_type.empty())
    {//
        confgSS << "renderengine.type = " << ctx.m_renderengine_type << std::endl;
    }

    if(!ctx.m_film_filter_type.empty())
    {
        confgSS << "film.filter.type = " << ctx.m_film_filter_type << std::endl;
    }
    if(!ctx.m_film_gamma.empty())
    {
        confgSS << "film.gamma = " << ctx.m_film_gamma << std::endl;
    }

    if(!ctx.m_sampler_type.empty())
    {
        confgSS << "sampler.type = " << ctx.m_sampler_type << std::endl;
        confgSS << "path.sampler.type = " << ctx.m_sampler_type << std::endl;
    }

    if(!ctx.m_largesteprate.empty())
    {
        confgSS << "sampler.largesteprate = " << ctx.m_largesteprate << std::endl;
    }
    if(!ctx.m_imagemutationrate.empty())
    {
        confgSS << "sampler.imagemutationrate = " << ctx.m_imagemutationrate << std::endl;
    }
    if(!ctx.m_maxconsecutivereject.empty())
    {
        confgSS << "sampler.maxconsecutivereject = " << ctx.m_maxconsecutivereject << std::endl;
    }

    if(!ctx.m_pathFilter_type.empty())
    {
        confgSS << "path.filter.type = " << ctx.m_pathFilter_type << std::endl;
    }
    if(!ctx.m_pathFilter_widthx.empty())
    {
        confgSS << "path.filter.width.x = " << ctx.m_pathFilter_widthx << std::endl;
    }
    if(!ctx.m_pathFilter_widthy.empty())
    {
        confgSS << "path.filter.width.y = " << ctx.m_pathFilter_widthy << std::endl;
    }
    if(!ctx.m_pathFilter_alpha.empty())
    {
        confgSS << "path.filter.alpha = " << ctx.m_pathFilter_alpha << std::endl;
    }
    if(!ctx.m_pathFilter_b.empty())
    {
        confgSS << "path.filter.B = " << ctx.m_pathFilter_b << std::endl;
    }
    if(!ctx.m_pathFilter_c.empty())
    {
        confgSS << "path.filter.C = " << ctx.m_pathFilter_c << std::endl;
    }

    if(ctx.m_pTonemapParams)
    {
        if(ctx.m_pTonemapParams->GetType() == slg::TONEMAP_LINEAR)
        {
            slg::LinearToneMapParams *plinear = static_cast<slg::LinearToneMapParams*>(ctx.m_pTonemapParams);
            confgSS << "film.tonemap.type = 0" << std::endl;
            confgSS << "film.tonemap.linear.scale = " << plinear->scale << std::endl;
        }else if(ctx.m_pTonemapParams->GetType() == slg::TONEMAP_REINHARD02){
            slg::Reinhard02ToneMapParams *pReinhard = static_cast<slg::Reinhard02ToneMapParams*>(ctx.m_pTonemapParams);
            confgSS << "film.tonemap.type = 1" << std::endl;
            confgSS << "film.tonemap.reinhard02.prescale = " << pReinhard->preScale << std::endl;
            confgSS << "film.tonemap.reinhard02.postscale = " << pReinhard->postScale << std::endl;
            confgSS << "film.tonemap.reinhard02.burn = " << pReinhard->burn << std::endl;
        }else{
            //no this type!
            BOOST_ASSERT(false);
        }
    }

    //see Pathoclthread.cpp#101
    //confgSS << "opencl.kernelcache = VOLATILE" << std::endl; //
    confgSS << "opencl.gpu.workgroup.size = " << 64 << std::endl;
    if(ctx.m_PathDepth.empty())
    {
        confgSS << "path.maxdepth = " << 8 << std::endl;
    }else{
        confgSS << "path.maxdepth = " << ctx.m_PathDepth << std::endl;
    }

    if(ctx.m_RouletteDepth.empty())
    {
        confgSS << "path.russianroulette.depth = " << 5 << std::endl;
    }else{
        confgSS << "path.russianroulette.depth = " << ctx.m_RouletteDepth << std::endl;
    }

    if(!ctx.m_RouletteCap.empty())
    {
        confgSS << "path.russianroulette.cap = " << ctx.m_RouletteCap << std::endl;
    }


    confgSS << "batch.halttime = 0" << std::endl;

    std::cerr << confgSS.str();
    slg::RenderConfig *config = new slg::RenderConfig(confgSS.str(),*scene);
    pDocData->m_session.reset(new slg::RenderSession(config));
    pDocData->start();
}


bool
DocIO::loadAssimpScene(const std::string &path)
{
    try
    {
        pDocData->closeScene();

        slg::Scene *scene = new slg::Scene();

        ImportContext   ctx(scene,path);

        ObjectNode  obj;
        //首先，尝试将path指定的资源加载。
        int count = ExtraObjectManager::importObjects(path,obj,ctx);

        if(count > 0)
        {//如果加载数量大于0才继续。
            if(obj.matid().length() == 0)
            {//只是组节点，将孩子加入root.
                //BOOST_ASSERT_MSG(obj.name().length() == 0,"loading panic!!");
                ObjectNode &root = pDocData->objManager.getRoot();
                BOOST_FOREACH( ObjectNode &node, obj.m_children )
                {
                    root.addChild(node);
                }
            }else{
                pDocData->objManager.getRoot().addChild(obj);
            }

            if(!pDocData->forceExport && pDocData->objManager.getRoot().getChildCount() > 2)
            {//如果包含的孩子大于1.(2是因为包含了自己)
                pDocData->forceExport = true;
            }

            initAndStartScene(scene,&ctx);
            return true;
        }else{
            delete scene;
            Doc::SysLog(Doc::LOG_ERROR,boost::str(boost::format(__("场景加载失败:场景'%s'中未包含可加载模型。")) % path ) );
        }
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

        ImportContext   ctx(scene,path);
        //load mode, try to restore film if exist.
        ctx.loadFilm(true);

        //首先，尝试将path指定的资源加载。
        int count = ExtraObjectManager::importSpScene(path,pDocData->objManager.getRoot(),ctx);

        if(count > 0)
        {//如果加载数量大于0才继续。
            initAndStartScene(scene,&ctx);
            return true;
        }else{
            Doc::SysLog(Doc::LOG_ERROR,boost::str(boost::format(__("从文件'%s'中未能加载任何场景。")) % path ) );
            delete scene;
        }
    }
    catch(cl::Error err)
    {
        std::cerr << err.what() << "(" << err.err() << ")" << std::endl;
        pDocData->m_session.reset();
    }
    return false;
}

const std::string&
DocIO::getLastLoadedPath(void)
{
    return pDocData->currentFile();
}

void
DocIO::setLastLoadedPath(const std::string &path)
{
    pDocData->m_currentFile = path;
}



bool
DocIO::loadScene(const std::string &path)
{
    bool    bLoadOK = false;
    if(boost::iends_with(path,".cfg"))
    {
        bLoadOK = loadSlgScene(path);
    }
    else if(boost::iends_with(path,".sps") || boost::iends_with(path,".slg"))
    {
        bLoadOK = loadSpsScene(path);
    }else if(boost::iends_with(path,".ctm"))
    {//load open ctm format.
    }else{
    //load assimp format.
        bLoadOK = loadAssimpScene(path);
    }

    pDocData->m_currentFile.clear();
    if(bLoadOK)
    {
        pDocData->m_currentFile = path;
        return true;
    }
    return false;
}

bool
DocIO::exportSpoloScene(const std::string &pathstring,bool bExportRes)
{
    boost::filesystem::path path(pathstring);
    boost::filesystem::path root_path = path.parent_path();

    boost::filesystem::ofstream   ofstream;
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

        //如果场景中包含了外部引入的模型。这里强制资源导出。
        if(!bExportRes && pDocData->forceExport)
        {
            bExportRes = true;
            Doc::SysLog(Doc::LOG_STATUS,boost::str(boost::format(__("由于场景中包含了外部引入的组模型，在保存到场景'%s'时，自动转入导出模式。")) % pathstring ) );
        }

//        //write scene config.
//        ofstream << "<settings>" << std::endl;
//        ofstream << "</settings>" << std::endl;
//
//        //write tone-mapping.
//        //write light.
        {
            std::ostream_iterator<char>   oit(ofstream);
            type_xml_doc    xmldoc;

            int flags = dumpContext::DUMP_NORMAL;
            if(bExportRes)
            {
                flags = (dumpContext::DUMP_SAVECTM | dumpContext::DUMP_COPYRES);
            }
            dumpContext     ctx(flags,boost::filesystem::path(pathstring).parent_path());

            //dump object.
            {
                type_xml_node * pNode = xmldoc.allocate_node(NS_RAPIDXML::node_element,"objects");
                xmldoc.append_node(pNode);
                pDocData->objManager.dump(*pNode,ctx);
            }

            //dump lights.
            {
                type_xml_node   *pLights = xmldoc.allocate_node(NS_RAPIDXML::node_element,constDef::lights);
                xmldoc.append_node(pLights);
                ExtraSettingManager::dumpLights(*pLights,ctx);
            }

            //dump setting.
            {
                type_xml_node   *pSetting = xmldoc.allocate_node(NS_RAPIDXML::node_element,constDef::setting);
                xmldoc.append_node(pSetting);
                ExtraSettingManager::dumpSettings(*pSetting,ctx);
            }


            //dump camera.
            {
                type_xml_node * pCameras = xmldoc.allocate_node(NS_RAPIDXML::node_element,"cameras");
                xmldoc.append_node(pCameras);
                if(pDocData->camManager.getSelect() != -1)
                {
                    pDocData->camManager.dumpAll(*pCameras);
                }else{
                    Camera  cam;
                    cam.name = "default";
                    ExtraCameraManager::saveTo(pDocData->m_session.get(),cam);
                    DocCamera::dumpOne(*pCameras,cam);
                    pCameras->append_attribute(allocate_attribute(&xmldoc,constDef::active,"0"));
                }
            }

            //dump film.
            {
                std::string   write_file;
                boost::filesystem::path target;
                if( ctx.isCopyResource() || pDocData->filmFilePath().empty() )
                {
                    //没有定义文件名。此时直接保存资源。
	                boost::filesystem::path target_model = ctx.target / "film%%%%%%.dmp";
                    target = boost::filesystem::unique_path(target_model);
                    if(pDocData->filmFilePath().empty())
                    {
                        pDocData->filmFilePath(target.string(),false);
                    }
                }else{
                    target = pDocData->filmFilePath();
                }
                if(pDocData->saveFilm(target.string()))
                {
                    if(ctx.isCopyResource())
                    {
                        write_file = target.filename().string();
                    }else{
                        write_file = target.string();
                    }
                    type_xml_node * pFilm = xmldoc.allocate_node(NS_RAPIDXML::node_element,constDef::film);
                    xmldoc.append_node(pFilm);
                    pFilm->append_attribute(allocate_attribute(&xmldoc,constDef::file,write_file));
                }
            }

            rapidxml::print(oit,xmldoc);
        }
        ofstream << "</scene>" << std::endl;

        if(pDocData->m_currentFile == pathstring)
        {
            pDocData->clearModified();
        }

        return true;
    }

    return false;
}

bool
DocIO::exportScene(const std::string &pathstring,bool bExportResource)
{
    if(!isValid())
        return false;
    bool    bExportOK = false;
    if(boost::iends_with(pathstring,".sps") || boost::iends_with(pathstring,".slg"))
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

void
DocIO::onSceneLoaded(type_state_handler handler)
{
    pDocData->state_Evt.addEventListen(DocPrivate::STATE_OPEN,handler);
}


void
DocIO::onSceneClosed(type_state_handler handler)
{
    pDocData->state_Evt.addEventListen(DocPrivate::STATE_CLOSE,handler);
}

void
DocIO::onSceneModified(type_state_handler handler)
{
    pDocData->state_Evt.addEventListen(DocPrivate::STATE_MODIFIED,handler);
}

bool
DocIO::isModified(void)
{
    return pDocData->isModified();
}



} //end namespace gme.
