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
#include "dm/doccamera.h"
#include "slg/slg.h"
#include "docprivate.h"
#include <boost/foreach.hpp>
#include "eigenutil.h"

namespace gme{

bool
DocCamera::rotate(int distX,int distY,float optRotateStep)
{
    if(!pDocData->m_started)
        return false;
    slg::RenderSession* session = pDocData->getSession();
    if(session && session->film)
    {
	    session->BeginEdit();

	    session->renderConfig->scene->camera->RotateUp(0.04f * distY * optRotateStep);
	    session->renderConfig->scene->camera->RotateLeft(0.04f * distX * optRotateStep);


	    session->renderConfig->scene->camera->Update(session->film->GetWidth(), session->film->GetHeight());
	    session->editActions.AddAction(slg::CAMERA_EDIT);
	    session->EndEdit();
	    return true;
	}
	return false;
}

bool
DocCamera::restoreFrom(Camera &cam)
{
    slg::RenderSession* session = pDocData->getSession();
    if(session && session->film)
    {
	    session->BeginEdit();

        slg::PerspectiveCamera *slgCam = session->renderConfig->scene->camera;
        EigenUtil::AssignToSlg(cam.orig,slgCam->orig);
        EigenUtil::AssignToSlg(cam.target,slgCam->target);
        EigenUtil::AssignToSlg(cam.up,slgCam->up);

        slgCam->clipHither = cam.clipHither;
        slgCam->clipYon = cam.clipYon;
        slgCam->fieldOfView = cam.fieldOfView;
        slgCam->focalDistance = cam.focalDistance;
        slgCam->lensRadius = cam.lensRadius;

        slgCam->Update(session->film->GetWidth(), session->film->GetHeight());
	    session->editActions.AddAction(slg::CAMERA_EDIT);
	    session->EndEdit();
        return true;
    }
    return false;
}


bool
DocCamera::saveTo(Camera &cam)
{
    slg::RenderSession* session = pDocData->getSession();
    if(session && session->film)
    {
        ExtraCameraManager::saveTo(session,cam);
        return true;
    }
    return false;
}

int
DocCamera::getSelected(void)
{
    return pDocData->camManager.getSelect();
}

bool
DocCamera::setSelected(int select)
{
    if(select == -1)
    {
        pDocData->camManager.setSelect(select);
        return true;
    }
    if(select >= 0 && select < (int)(pDocData->camManager.m_cam_vector.size()))
    {
        pDocData->camManager.setSelect(select);
        return true;
    }
    return false;
}

Camera&
DocCamera::get(int pos)
{
    return pDocData->camManager.m_cam_vector[pos];
}

int
DocCamera::addCam(const Camera &cam)
{
    if(cam.name.length() == 0)
        return -1;
    int pos = 0;
    BOOST_FOREACH( Camera &underly_cam, pDocData->camManager.m_cam_vector )
    {
        if(underly_cam.name.length() == 0)
        {
            underly_cam = cam;
            return pos;
        }
        pos++;
    }
    pDocData->camManager.m_cam_vector.push_back(cam);
    return (int)(pDocData->camManager.m_cam_vector.size() - 1);
}

type_cam_vector::iterator
DocCamera::begin(void)
{
    return pDocData->camManager.m_cam_vector.begin();
}

type_cam_vector::iterator
DocCamera::end(void)
{
    return pDocData->camManager.m_cam_vector.end();
}

type_cam_vector::size_type
DocCamera::size(void)
{
    return pDocData->camManager.m_cam_vector.size();
}

int
DocCamera::dumpAll(type_xml_node &parent)
{
    return pDocData->camManager.dumpAll(parent);
}

void
DocCamera::dumpOne(type_xml_node &parent,const Camera &cam)
{
    type_xml_doc *pDoc = parent.document();
    BOOST_ASSERT_MSG(pDoc,"invalid xml node");

    type_xml_node *pSelf = pDoc->allocate_node(NS_RAPIDXML::node_element,constDef::camera);
    parent.append_node(pSelf);

    pSelf->append_attribute(allocate_attribute(pDoc,constDef::name,cam.name));

    pSelf->append_attribute(allocate_attribute(pDoc,constDef::position,EigenUtil::Vector2String(cam.orig)));
    pSelf->append_attribute(allocate_attribute(pDoc,constDef::target,EigenUtil::Vector2String(cam.target)));
    pSelf->append_attribute(allocate_attribute(pDoc,constDef::up,EigenUtil::Vector2String(cam.up)));

    if(!cam.isDefaultFieldOfView())
        pSelf->append_attribute(allocate_attribute(pDoc,constDef::fieldOfView,boost::lexical_cast<std::string>(cam.fieldOfView)));
    if(!cam.isDefaultClipHither())
        pSelf->append_attribute(allocate_attribute(pDoc,constDef::clipHither,boost::lexical_cast<std::string>(cam.clipHither)));
    if(!cam.isDefaultClipYon())
        pSelf->append_attribute(allocate_attribute(pDoc,constDef::clipYon,boost::lexical_cast<std::string>(cam.clipYon)));
    if(!cam.isDefaultLensRadius())
        pSelf->append_attribute(allocate_attribute(pDoc,constDef::lensRadius,boost::lexical_cast<std::string>(cam.lensRadius)));
    if(!cam.isDefaultFocalDistance())
        pSelf->append_attribute(allocate_attribute(pDoc,constDef::focalDistance,boost::lexical_cast<std::string>(cam.focalDistance)));
}

int
DocCamera::importAll(type_xml_node &parent)
{
    return pDocData->camManager.findAndImportCamera(parent);
}

void
DocCamera::importOne(type_xml_node &self,Camera &cam)
{
    type_xml_attr *pAttr = self.first_attribute(constDef::name);
    if(pAttr)
    {
        cam.name = pAttr->value();
    }

    pAttr = self.first_attribute(constDef::position);
    if(pAttr)
    {
        EigenUtil::AssignFromString(cam.orig,pAttr->value());
    }
    pAttr = self.first_attribute(constDef::target);
    if(pAttr)
    {
        EigenUtil::AssignFromString(cam.target,pAttr->value());
    }
    pAttr = self.first_attribute(constDef::up);
    if(pAttr)
    {
        EigenUtil::AssignFromString(cam.up,pAttr->value());
    }

    pAttr = self.first_attribute(constDef::fieldOfView);
    if(pAttr)
    {
        cam.fieldOfView = boost::lexical_cast<float>(pAttr->value());
    }
    pAttr = self.first_attribute(constDef::clipHither);
    if(pAttr)
    {
        cam.clipHither = boost::lexical_cast<float>(pAttr->value());
    }
    pAttr = self.first_attribute(constDef::clipYon);
    if(pAttr)
    {
        cam.clipYon = boost::lexical_cast<float>(pAttr->value());
    }
    pAttr = self.first_attribute(constDef::lensRadius);
    if(pAttr)
    {
        cam.lensRadius = boost::lexical_cast<float>(pAttr->value());
    }
    pAttr = self.first_attribute(constDef::focalDistance);
    if(pAttr)
    {
        cam.focalDistance = boost::lexical_cast<float>(pAttr->value());
    }

}




} //end namespace gme.
