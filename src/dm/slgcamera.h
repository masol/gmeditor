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

#ifndef  GME_DM_SLGCAMERA_H
#define  GME_DM_SLGCAMERA_H

#include "dm/doccamera.h"
#include "dm/objectnode.h"
#include "slg/slg.h"
#include "slg/camera/camera.h"

struct aiCamera;

namespace gme{

class ExtraCameraManager{
    friend class DocCamera;
private:
    type_cam_vector     m_cam_vector;
    int                 m_current_select;
public:
    ///@brief save current camera information to current selected camera.
    void    saveCurrentCamera(void);
    ExtraCameraManager(){
        m_current_select = -1;
    }
    inline  void    clear(void)
    {
        m_cam_vector.clear();
        m_current_select = -1;
    }
    Camera* getSelected(void)
    {
        if(m_current_select >= 0)
            return &m_cam_vector[m_current_select];
        return NULL;
    }
    inline int getSelect(void)const
    {
        return m_current_select;
    }
    inline void setSelect(int sel)
    {
        m_current_select = sel;
    }
    int findAndImportCamera(type_xml_node &node);
    int dumpAll(type_xml_node &parent);
    void importAiCamera(aiCamera *pCam);
public:
    ///@brief viewall object.
    static void viewAll(const std::string &objid);
    static void saveTo(slg::RenderSession *session,Camera &cam);
    static void targetRotate(slg::PerspectiveCamera *camera,const float angle, const luxrays::Vector &axis);
    static inline void targetRotateUp(slg::PerspectiveCamera *camera,const float angle)
    {
        targetRotate(camera,angle,camera->GetX());
    }

    static inline void targetRotateLeft(slg::PerspectiveCamera *camera,const float angle)
    {
        targetRotate(camera,angle,camera->GetY());
    }
private:
    static  void    GetObjectBBox(luxrays::BBox &box,ObjectNode *pNode);
};

}

#endif  //GME_DM_SLGCAMERA_H
