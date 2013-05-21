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
#include "slgobject.h"
#include "docprivate.h"
#include "dm/doccamera.h"
#include "slg/camera/camera.h"
#include "slg/rendersession.h"
#include "eigenutil.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/math/constants/constants.hpp>
#include <assimp/scene.h>           // Output data structure


namespace gme{


void
ExtraCameraManager::importAiCamera(aiCamera *pCam)
{
    if(!pCam)
        return;
    Camera cam;
    if(pCam->mName.length)
        cam.name = pCam->mName.C_Str();

    cam.fieldOfView = pCam->mHorizontalFOV * 180.0f / boost::math::constants::pi<float>();
    cam.orig[0] = pCam->mPosition.x;
    cam.orig[1] = pCam->mPosition.y;
    cam.orig[2] = pCam->mPosition.z;

    cam.target[0] = pCam->mLookAt.x;
    cam.target[1] = pCam->mLookAt.y;
    cam.target[2] = pCam->mLookAt.z;

    cam.up[0] = pCam->mUp.x;
    cam.up[1] = pCam->mUp.y;
    cam.up[2] = pCam->mUp.z;

    //cam.focalDistance = (pCam->mLookAt - pCam->mPosition).Length();

    this->m_cam_vector.push_back(cam);

    if(this->m_current_select == -1)
        this->m_current_select = 0;
}

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


void
ExtraCameraManager::targetRotate(slg::PerspectiveCamera *camera,const float angle, const luxrays::Vector &axis)
{
    luxrays::Vector p = camera->orig - camera->target;
    luxrays::Transform t = luxrays::Rotate(angle, axis);
    camera->orig = camera->target + t * p;
}

void
ExtraCameraManager::GetObjectBBox(luxrays::BBox &box,ObjectNode *pNode)
{
    if(!pNode->matid().empty())
    {
        luxrays::ExtMesh *pMesh = ExtraObjectManager::getExtMesh(pNode->id());
        if(pMesh)
        {
            box = luxrays::Union(box,pMesh->GetBBox());
        }
    }
    ObjectNode::type_child_container::iterator it = pNode->begin();
    while(it != pNode->end())
    {
        GetObjectBBox(box,&(*it));
        it++;
    }
}

void
ExtraCameraManager::saveCurrentCamera(void)
{
    int selected = getSelect();
    if(selected >= 0)
    {
        Camera& cam = m_cam_vector[selected];
        saveTo(Doc::instance().pDocData->getSession(),cam);
    }
}


void
ExtraCameraManager::viewAll(const std::string &objid)
{
    luxrays::BBox   box;
    if(objid.empty())
    {//view all
        slg::Scene  *scene = Doc::instance().pDocData->m_session->renderConfig->scene;

        BOOST_FOREACH(luxrays::ExtMesh *mesh,scene->extMeshCache.meshes)
        {
            box = luxrays::Union(box,mesh->GetBBox());
        }
    }else
    {//view all with object.
        ExtraObjectManager &objManager = Doc::instance().pDocData->objManager;
        ///@todo use path to caculate transform.
        ObjectNode *pNode = objManager.getRoot().findObject(objid,NULL);
        if(pNode)
        {
            GetObjectBBox(box,pNode);
        }
    }
    if(box.IsValid())
    {
        luxrays::BSphere    bs = box.BoundingSphere();

        SlgUtil::Editor editor(Doc::instance().pDocData->m_session.get());

        slg::PerspectiveCamera *camera = editor.session()->renderConfig->scene->camera;

        u_int width = editor.session()->film->GetWidth();
        u_int height = editor.session()->film->GetHeight();

        float aspect = (float)width / (float)height;
        float aspectradius = bs.rad / (aspect < 1.0f ? 1.0f : aspect);
        float  radius = aspectradius / std::tan(((camera->fieldOfView / 180.0f ) * boost::math::constants::pi<float>() ) / 2.0f);

        camera->orig[0] = (bs.center.x  + radius);
        camera->orig[1] = (bs.center.y  + radius);
        camera->orig[2] = (bs.center.z  + radius);

        camera->target[0] = bs.center.x;
        camera->target[1] = bs.center.y;
        camera->target[2] = bs.center.z;

        if(Doc::instance().pDocData->autoFocus())
        {
            ///@fixme this follow caculate is wrong,Focus distance is the distance between the imaginary plane in the scene that will be in focus and the camera imaging plane.
            ///please refer to slg blender export.
//            float   dist = luxrays::Distance(camera->target,camera->orig);
//            if(dist < 0) dist = -dist;
//            camera->focalDistance =  dist / 2;
        }

	    camera->Update(width, height);
	    editor.addAction(slg::CAMERA_EDIT);
    }
}


}

