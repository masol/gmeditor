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
#include "slgcamera.h"
#include "dm/doccamera.h"
#include "slg/camera/camera.h"
#include "slg/rendersession.h"
#include "eigenutil.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

namespace gme{

int
ExtraCameraManager::findAndImportCamera(type_xml_node &node)
{
    int ret = 0;

    if(this->m_current_select == -1 && boost::iequals("cameras",node.name()))
    {//read m_current_select
        type_xml_attr *pAttr = node.first_attribute(constDef::active);
        if(pAttr)
            this->m_current_select = boost::lexical_cast<int>(pAttr->value());
    }

    if(boost::iequals(constDef::camera,node.name()))
    {
        Camera cam;
        DocCamera::importOne(node,cam);
        this->m_cam_vector.push_back(cam);
        ret++;
    }else{
        type_xml_node *pChild = node.first_node();
        while(pChild)
        {
            ret += findAndImportCamera(*pChild);
            pChild = pChild->next_sibling();
        }
    }
    return ret;
}

int
ExtraCameraManager::dumpAll(type_xml_node &parent)
{
    int     count = 0;
    BOOST_FOREACH( Camera &cam, m_cam_vector )
    {
        if(cam.name.length() != 0)
        {
            DocCamera::dumpOne(parent,cam);
            count++;
        }
    }
    int sel = m_current_select;
    if(sel != -1)
    {//append slected to parent.
        type_xml_doc *pDoc = parent.document();
        BOOST_ASSERT_MSG(pDoc,"invalid xml node");
        parent.append_attribute(allocate_attribute(pDoc,constDef::active,boost::lexical_cast<std::string>(sel)));
    }
    return count;
}

void
ExtraCameraManager::saveTo(slg::RenderSession *session,Camera &cam)
{
    slg::PerspectiveCamera *slgCam = session->renderConfig->scene->camera;
    EigenUtil::AssignFromSlg(cam.orig,slgCam->orig);
    EigenUtil::AssignFromSlg(cam.target,slgCam->target);
    EigenUtil::AssignFromSlg(cam.up,slgCam->up);

    cam.clipHither = slgCam->clipHither;
    cam.clipYon = slgCam->clipYon;
    cam.fieldOfView = slgCam->fieldOfView;
    cam.focalDistance = slgCam->focalDistance;
    cam.lensRadius = slgCam->lensRadius;
}


}

