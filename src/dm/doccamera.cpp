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
#include "utils/option.h"

namespace gme{


template<class T>
static  inline  void
assignIfExist(gme::Option &option,const std::string &key,T &result)
{
    boost::optional<T> opt = Option::instance().get_optional<T>(key);
    if(opt.is_initialized())
        result = *opt;
}

Camera
Camera::getDefault(void)
{
    Camera  cam;
    boost::optional<char> up_opt = Option::instance().get_optional<char>("scene.camera.up");
    if(up_opt.is_initialized())
    {
        switch(*up_opt)
        {
        case 'X':
        case 'x':
            cam.up << 0.1f,0.0f,0.0f;
            break;
        case 'Y':
        case 'y':
            cam.up << 0.0f,0.1f,0.0f;
            break;
        }
    }


    gme::Option     &option = gme::Option::instance();
    assignIfExist(option,"scene.camera.fieldofview",cam.fieldOfView);
    assignIfExist(option,"scene.camera.cliphither",cam.clipHither);
    assignIfExist(option,"scene.camera.clipyon",cam.clipYon);
    assignIfExist(option,"scene.camera.lensradius",cam.lensRadius);
    assignIfExist(option,"scene.camera.focaldistance",cam.focalDistance);

    return cam;
}

template<class T>
static  inline  void
putOrErase(gme::Option &option,bool bDefault,const std::string &key,T value)
{
    if(!bDefault)
    {
        option.put(key,value);
    }else{
        option.erase(key);
    }
}

void
Camera::setDefault(const Camera &cam)
{
    {
        Eigen::Vector3f     v;
        v.UnitZ();
        float zdot = v.norm() - v.dot(cam.up);

        v.UnitY();
        float ydot = v.norm() - v.dot(cam.up);

        v.UnitX();
        float xdot = v.norm() - v.dot(cam.up);

        if(xdot <= zdot && xdot <= ydot)
        {// up is X
            gme::Option::instance().put("scene.camera.up",'X');
        }else if(ydot <= zdot && ydot <= xdot )
        {//up is y.
            gme::Option::instance().put("scene.camera.up",'Y');
        }else
        {//up is z.
        }
    }

    gme::Option &option = gme::Option::instance();
    putOrErase(option,cam.isDefaultFieldOfView(),"scene.camera.fieldofview",cam.fieldOfView);
    putOrErase(option,cam.isDefaultClipHither(),"scene.camera.cliphither",cam.clipHither);
    putOrErase(option,cam.isDefaultClipYon(),"scene.camera.clipyon",cam.clipYon);
    putOrErase(option,cam.isDefaultLensRadius(),"scene.camera.lensradius",cam.lensRadius);
    putOrErase(option,cam.isDefaultFocalDistance(),"scene.camera.focaldistance",cam.focalDistance);
}


////////////////////////////////////////////////////////////////////////////////

bool
DocCamera::translate(int distx,int disty,float optTranslateFactor)
{
    if(!pDocData->isRunning())
        return false;
    slg::RenderSession* session = pDocData->getSession();
    if(session && session->film)
    {
        BOOST_ASSERT_MSG(session->renderConfig->scene->dataSet != NULL,"why no dateSet? scene not init!");
	    session->BeginEdit();

        float basic_step = ( session->renderConfig->scene->dataSet->GetBSphere().rad ) * optTranslateFactor ;

        slg::PerspectiveCamera *camera = session->renderConfig->scene->camera;

        float distxInWorld = ((float)distx / (float)camera->GetFilmWeight()) * basic_step;
        float distyInWorld = ((float)disty / (float)camera->GetFilmHeight()) * basic_step;

        camera->TranslateLeft(distxInWorld);
        camera->Translate(luxrays::Normalize(camera->up) * distyInWorld);

        pDocData->updateFocus();

	    camera->Update(session->film->GetWidth(), session->film->GetHeight());
	    session->editActions.AddAction(slg::CAMERA_EDIT);
	    session->EndEdit();
        pDocData->cachefilm().invalidate();
        pDocData->camManager.saveCurrentCamera();
        pDocData->setModified();
	    return true;
	}
	return false;
}

bool
DocCamera::straightTranslate(float factor)
{
    if(!pDocData->isRunning())
        return false;
    slg::RenderSession* session = pDocData->getSession();
    if(session && session->film)
    {
        BOOST_ASSERT_MSG(session->renderConfig->scene->dataSet != NULL,"why no dateSet? scene not init!");
	    session->BeginEdit();

        float basic_step = ( session->renderConfig->scene->dataSet->GetBSphere().rad / 10.0f ) * factor ;

        slg::PerspectiveCamera *camera = session->renderConfig->scene->camera;

        camera->TranslateForward(basic_step);

        pDocData->updateFocus();

	    camera->Update(session->film->GetWidth(), session->film->GetHeight());
	    session->editActions.AddAction(slg::CAMERA_EDIT);
	    session->EndEdit();
	    pDocData->cachefilm().invalidate();
	    pDocData->camManager.saveCurrentCamera();
	    pDocData->setModified();
	    return true;
	}
	return false;
}

bool
DocCamera::autoTarget(void)
{
    return pDocData->autoTarget();
}

void
DocCamera::autoTarget(bool at)
{
    pDocData->autoTarget(at);
}

bool
DocCamera::autoFocus(void)
{
    return pDocData->autoFocus();
}

void
DocCamera::autoFocus(bool af)
{
    pDocData->autoFocus(af);
}

bool
DocCamera::targetRotate(int distx,int disty,float optRotateFactor)
{
    if(!pDocData->isRunning())
        return false;
    slg::RenderSession* session = pDocData->getSession();
    if(session && session->film)
    {
	    session->BeginEdit();

	    slg::PerspectiveCamera  *camera = session->renderConfig->scene->camera;

        float xangle = ((float)distx / (float)camera->GetFilmWeight()) * ( 180.0f * optRotateFactor);
        float yangle = ((float)disty / (float)camera->GetFilmHeight()) * ( 180.0f * optRotateFactor);


	    ExtraCameraManager::targetRotateUp(camera,yangle);
	    ExtraCameraManager::targetRotateLeft(camera,xangle);

        pDocData->updateFocus();

	    camera->Update(session->film->GetWidth(), session->film->GetHeight());
	    session->editActions.AddAction(slg::CAMERA_EDIT);
	    session->EndEdit();
	    pDocData->cachefilm().invalidate();
	    pDocData->camManager.saveCurrentCamera();
	    pDocData->setModified();
	    return true;
	}
	return false;
}



bool
DocCamera::rotate(int distX,int distY,float optRotateFactor)
{
    if(!pDocData->isRunning())
        return false;
    slg::RenderSession* session = pDocData->getSession();
    if(session && session->film)
    {
	    session->BeginEdit();

	    slg::PerspectiveCamera *camera = session->renderConfig->scene->camera;

        float xangle = ((float)distX / (float)camera->GetFilmWeight()) * ( 180.0f * optRotateFactor);
        float yangle = ((float)distY / (float)camera->GetFilmHeight()) * ( 180.0f * optRotateFactor);

	    camera->RotateUp(yangle);
	    camera->RotateLeft(xangle);

        pDocData->updateFocus();

	    camera->Update(session->film->GetWidth(), session->film->GetHeight());
	    session->editActions.AddAction(slg::CAMERA_EDIT);
	    session->EndEdit();
	    pDocData->cachefilm().invalidate();
	    pDocData->camManager.saveCurrentCamera();
	    pDocData->setModified();
	    return true;
	}
	return false;
}

bool
DocCamera::setTarget(void)
{
    if(!pDocData->isRunning())
        return false;
    slg::RenderSession* session = pDocData->getSession();
    if(session && session->film)
    {
	    session->BeginEdit();

	    slg::PerspectiveCamera *camera = session->renderConfig->scene->camera;
        camera->focalDistance = (camera->target - camera->orig).Length();

	    camera->Update(session->film->GetWidth(), session->film->GetHeight());
	    session->editActions.AddAction(slg::CAMERA_EDIT);
	    session->EndEdit();

	    pDocData->cachefilm().invalidate();
	    pDocData->camManager.saveCurrentCamera();
	    pDocData->setModified();
	    return true;
    }
	return false;
}


bool
DocCamera::viewAll(const std::string &objID)
{
    slg::RenderSession* session = pDocData->getSession();
    if(session && session->film)
    {
        ExtraCameraManager::viewAll(objID);
        //this is done in slgutils::Editor.
//        pDocData->cachefilm().invalidate();
        pDocData->camManager.saveCurrentCamera();
        pDocData->setModified();
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
	    pDocData->cachefilm().invalidate();
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

void
DocCamera::onCameraSwitched(type_state_handler handler)
{
    pDocData->state_Evt.addEventListen(DocPrivate::STATE_CAMSELSWITCHED,handler);
}


bool
DocCamera::setSelected(int select)
{
    if(select == -1)
    {
        pDocData->camManager.setSelect(select);
        pDocData->fireStateChanged(DocPrivate::STATE_CAMSELSWITCHED);
        return true;
    }
    if(select >= 0 && select < (int)(pDocData->camManager.m_cam_vector.size()))
    {
        pDocData->camManager.setSelect(select);
        pDocData->fireStateChanged(DocPrivate::STATE_CAMSELSWITCHED);
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
