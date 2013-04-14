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
#include "utils/pathext.h"
#include <slg/slg.h>
#include <luxrays/utils/properties.h>
#include <boost/algorithm/string/predicate.hpp>
#include "docprivate.h"

namespace gme{

void
DocIO::loadObjectFromScene(void)
{
    slg::Scene  *scene = pDocData->getSession()->renderConfig->scene;
    std::vector<std::string> names = scene->meshDefs.GetExtMeshNames();
    u_int i = 0;
    for(std::vector<std::string>::const_iterator iter = names.begin(); iter < names.end(); ++iter,++i)
    {
        luxrays::ExtMesh * extmesh = scene->meshDefs.GetExtMesh((*iter));
        BOOST_ASSERT_MSG(extmesh != NULL,"Mesh Array Panic?");
        ObjectNode  obj;
        obj.m_id = boost::uuids::random_generator()();
        obj.m_name = (*iter);
        pDocData->m_object_map[obj.m_id] = obj.m_name;
        
        obj.m_matid = boost::uuids::random_generator()();
        pDocData->m_material_map[obj.m_matid] = scene->objectMaterials[i]->GetName();
        
        obj.m_useplynormals = extmesh->HasNormals();
        luxrays::ExtInstanceTriangleMesh* pinst = dynamic_cast<luxrays::ExtInstanceTriangleMesh*>(extmesh);
        if(pinst)
        {
            obj.m_transformation = pinst->GetTransformation().m;
        }

		pDocData->m_objectGroup.m_children.push_back(obj);
    }
}


bool
DocIO::loadScene(const std::string &path)
{
    std::string ext = boost::filesystem::gme_ext::get_extension(path);
    if(boost::iequals(ext,".cfg"))
    {
        luxrays::Properties cmdLineProp;
        slg::RenderConfig *config = new slg::RenderConfig(&path, &cmdLineProp);
        if(pDocData->m_session)
        {
            pDocData->m_session->Stop();
        }
        pDocData->m_session.reset(new slg::RenderSession(config));
        loadObjectFromScene();
        pDocData->m_session->Start();
        pDocData->m_started = true;
        return true;
    }
    return false;
}


} //end namespace gme.
