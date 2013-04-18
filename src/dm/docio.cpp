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
#include <slg/slg.h>
#include <luxrays/utils/properties.h>
#include <boost/algorithm/string/predicate.hpp>
#include <fstream>
#include <boost/scope_exit.hpp>
#include "eigenutil.h"
#include "docprivate.h"
#include "slgobject.h"
#include "slgmaterial.h"

#define __CL_ENABLE_EXCEPTIONS 1
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif


namespace gme{

void
DocIO::loadSlgSceneFile(const std::string pathstring)
{//从场景文件中加载全部的材质定义并保存。
}

void
DocIO::loadObjectFromScene(void)
{
    slg::Scene  *scene = pDocData->getSession()->renderConfig->scene;
    std::vector<std::string> names = scene->meshDefs.GetExtMeshNames();
    
    SlgMaterial2Name    mat2name;
    for(std::vector<std::string>::const_iterator iter = names.begin(); iter < names.end(); ++iter)
    {
        luxrays::ExtMesh * extmesh = scene->meshDefs.GetExtMesh((*iter));
        BOOST_ASSERT_MSG(extmesh != NULL,"Mesh Array Panic?");
        ObjectNode  obj;
        obj.m_id = boost::uuids::random_generator()();
        obj.m_name = (*iter);

        obj.m_matid = boost::uuids::random_generator()();

        ExtraMaterial  &materialInfo = ExtraMaterialManager::instance().get(obj.m_matid);
        
        u_int meshIdx = scene->meshDefs.GetExtMeshIndex(extmesh);
        materialInfo.m_slgname = mat2name.getMaterialName(scene->objectMaterials[meshIdx]);
        materialInfo.m_name = materialInfo.m_slgname; 
        //SLG中有缺陷。namearray的index不同于matarray!所以下面的检查是错误的。请查看上文如何获取的材质名称以了解nameidx和matidx之间的换算。
        //BOOST_ASSERT_MSG(scene->matDefs.GetMaterial(materialInfo.m_slgname) == scene->objectMaterials[i], "material panic");

        //所有从slg读取的场景，不支持normal。
        obj.m_useplynormals = false;//extmesh->HasNormals();
        luxrays::ExtInstanceTriangleMesh* pinst = dynamic_cast<luxrays::ExtInstanceTriangleMesh*>(extmesh);
        if(pinst)
        {
            EigenUtil::AssignFromSlgMatrix(obj.m_transformation,pinst->GetTransformation().m);
        }
        
        ExtraObjectManager::instance().getRoot().addChild(obj);
        ExtraObjectManager::instance().updateSlgMap(obj.m_id,obj.m_name);
    }
}


bool
DocIO::loadScene(const std::string &path)
{
    std::string ext = boost::filesystem::gme_ext::get_extension(path);
    if(boost::iequals(ext,".cfg"))
    {
		try{
			luxrays::Properties cmdLineProp;
			slg::RenderConfig *config = new slg::RenderConfig(&path, &cmdLineProp);
			if(pDocData->m_session)
			{
				pDocData->m_session->Stop();
			}
			pDocData->m_session.reset(new slg::RenderSession(config));
			loadObjectFromScene();
			std::string scnFile = pDocData->m_session->renderConfig->cfg.GetString("scene.file","");
			if(scnFile.length())
			{
				loadSlgSceneFile(boost::filesystem::absolute(scnFile,boost::filesystem::absolute(path)).string());
			}
			pDocData->m_session->Start();
			pDocData->m_started = true;
	        return true;
		}catch(cl::Error err)
		{
			std::cerr << err.what() << "(" << err.err() << ")" << std::endl;
			pDocData->m_session.reset();
			return false;
		}
    }else if(boost::iequals(ext,".sps"))
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
		}BOOST_SCOPE_EXIT_END
    
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
        //write material.
        ofstream << "<materials>" << std::endl;
        {
            MaterialWriteContext    context(bExportRes,root_path,ofstream);
            ExtraMaterialManager::instance().write(context);
        }
        ofstream << "</materials>" << std::endl;
        
        //write object.
        ofstream << "<objects>" << std::endl;
        {
            ObjectWriteContext  context(bExportRes,root_path,ofstream);
            ExtraObjectManager::instance().write(context);
        }
        ofstream << "</objects>" << std::endl;

        ofstream << "</scene>" << std::endl;
        return true;
    }

    return false;
}

bool
DocIO::exportScene(const std::string &pathstring)
{
    bool    bExportOK = false;
    std::string ext = boost::filesystem::gme_ext::get_extension(pathstring);
    if(boost::iequals(ext,".sps"))
    {//export sp scene file.
        bExportOK = exportSpoloScene(pathstring,true);
    }else if(boost::iequals(ext,".cfg"))
    {//export slg scene file.
    }else if(boost::iequals(ext,".ocs"))
    {//export octane scene file.
    }else if(boost::iequals(ext,".vray"))
    {//export vray scene file.
    }else if(boost::iequals(ext,".blend"))
    {//export blend scene with cycles material file.
    }

    return bExportOK;
}



} //end namespace gme.
