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
#include <slg/slg.h>
#include <luxrays/utils/properties.h>
#include <boost/algorithm/string/predicate.hpp>
#include <fstream>
#include <boost/scope_exit.hpp>
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

        pDocData->matManager.appendMat2IdFromSlg();

        //保存每个贴图对应的原始文件信息。
        pDocData->texManager.loadExtraFromProps(props);
        pDocData->texManager.appendTex2IdFromSlg();
    }
}

void
DocIO::loadExtraFromScene(void)
{
    slg::Scene  *scene = pDocData->getSession()->renderConfig->scene;
    std::vector<std::string> names = scene->meshDefs.GetExtMeshNames();

    SlgMaterial2Name    mat2name;
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
        obj.m_matid = mat2name.getMaterialName(scene->objectMaterials[meshIdx]);

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
DocIO::loadScene(const std::string &path)
{
    std::string ext = boost::filesystem::gme_ext::get_extension(path);
    if(boost::iequals(ext,".cfg"))
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
            return false;
        }
    }
    else if(boost::iequals(ext,".sps"))
    {
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

        //slg::Scene  *scene = pDocData->getSession()->renderConfig->scene;
        //u_int idx;


        ofstream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;

        ofstream << "<scene>" << std::endl;

        //write scene config.
        ofstream << "<settings>" << std::endl;
        ofstream << "</settings>" << std::endl;

        //write camera.
        //write tone-mapping.
        //write light.
        //write object.
        {
            ObjectWriteContext  objctx(bExportRes,root_path,ofstream);
            ofstream << "<objects>" << std::endl;
            {
                //pDocData->objManager.write(objctx);
            }
            ofstream << "</objects>" << std::endl;

            //write material.
            ofstream << "<materials>" << std::endl;
            {
                MaterialWriteContext    context(bExportRes,root_path,ofstream);
                //pDocData->matManager.write(context,objctx.refMaterials());
            }
            ofstream << "</materials>" << std::endl;
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
    std::string ext = boost::filesystem::gme_ext::get_extension(pathstring);
    if(boost::iequals(ext,".sps"))
    {
        //export sp scene file.
        bExportOK = exportSpoloScene(pathstring,bExportResource);
    }
    else if(boost::iequals(ext,".cfg"))
    {
        //export slg scene file.
    }
    else if(boost::iequals(ext,".ocs"))
    {
        //export octane scene file.
    }
    else if(boost::iequals(ext,".vray"))
    {
        //export vray scene file.
    }
    else if(boost::iequals(ext,".blend"))
    {
        //export blend scene with cycles material file.
    }

    return bExportOK;
}

bool
DocIO::importScene(const std::string &path,ObjectNode *pParent)
{
    SlgUtil::Editor      editor(pDocData->m_session.get());
    return pDocData->objManager.loadObjectsFromFile(path,pParent,editor);
}


bool
DocIO::deleteModel(const std::string &id)
{
    return pDocData->objManager.removeMesh(id);
}


} //end namespace gme.
