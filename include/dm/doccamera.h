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

#ifndef  GME_DM_DOCCAMERA_H
#define  GME_DM_DOCCAMERA_H


#include "dm/doc.h"
#include <Eigen/Core>
#include <vector>
#include "dm/xmlutil.h"

namespace gme{

/** @brief 名称为空意味者本slot无效。
**/
class Camera{
public:
    Eigen::Vector3f     orig;
    Eigen::Vector3f     target;
    Eigen::Vector3f     up;
    float 	            fieldOfView;
    float 	            clipHither;
    float 	            clipYon;
    float 	            lensRadius;
    float 	            focalDistance;
    std::string         name;
public:
    Camera()
    {
        up << 0.0f,0.0f,0.1f;
        fieldOfView = 45.0f;
        clipHither = 1e-3f;
        clipYon = 1e30f;
        lensRadius = 0.f;
        focalDistance = 10.f;
    }
    inline  bool    isValid(void)const
    {
        return (orig != target);
    }
    inline  bool    isDefaultFieldOfView(void)const
    {
        return (fieldOfView == 45.f);
    }
    inline  bool    isDefaultClipHither(void)const
    {
        return (clipHither == 1e-3f);
    }
    inline  bool    isDefaultClipYon(void)const
    {
        return (clipYon == 1e30f);
    }
    inline  bool    isDefaultLensRadius(void)const
    {
        return (lensRadius == 0.f);
    }
    inline  bool    isDefaultFocalDistance(void)const
    {
        return (focalDistance == 10.f);
    }
    Camera(const Camera &ref)
    {
        assignFrom(ref);
    }
    inline Camera& operator=(const Camera &ref)
    {
        assignFrom(ref);
        return *this;
    }
public:
    ///@brief get/set the default camera.the orig,target and name are ignored.
    static  Camera     getDefault(void);
    static  void       setDefault(const Camera &cam);
private:
    void    assignFrom(const Camera &ref)
    {
        this->orig = ref.orig;
        this->target = ref.target;
        this->up = ref.up;
        this->fieldOfView = ref.fieldOfView;
        this->clipHither = ref.clipHither;
        this->clipYon = ref.clipYon;
        this->lensRadius = ref.lensRadius;
        this->focalDistance = ref.focalDistance;
        this->name = ref.name;
    }
};

///@brief camera storage, use position as it's id.
typedef std::vector<Camera>     type_cam_vector;

class DocCamera : public DocScopeLocker
{
public:
    typedef boost::function<void (void)>     type_state_handler;
public:
    bool    autoTarget(void);
    void    autoTarget(bool at);
    bool    autoFocus(void);
    void    autoFocus(bool af);
    /** @brief rotating around target.distx and disty is in document coordinate.
    **/
    bool   targetRotate(int distx,int disty,float optRotateFactor);

    /** @brief distx and disty is in document coordinate.
    **/
    bool   rotate(int distx,int disty,float optRotateFactor);

    /** @brief factor.
    **/
    bool   straightTranslate(float factor);

    /** @brief use the bounding-sphere-radius/1000 as the basic step.
    **/
    bool   translate(int distx,int disty,float optTranslateFactor);
    /** @brief save active camera to camera object.
    **/
    bool    saveTo(Camera &cam);
    /** @brief restore active camera from camera object.
    **/
    bool    restoreFrom(Camera &cam);
    /** @brief set camera to viewall object.
     * @param objID : indicate object.if it's empty, viewall the scene.
    **/
    bool    viewAll(const std::string &objID);
    /** @brief set center to current camera target.
    **/
    bool    setTarget(void);
public: //camera storage.
    void    onCameraSwitched(type_state_handler handler);
    int     getSelected(void);
    bool    setSelected(int select);
    /** @return number of imported.
    **/
    int    importAll(type_xml_node &parent);
    static void   importOne(type_xml_node &self,Camera &cam);
    static void   dumpOne(type_xml_node &parent,const Camera &cam);
    /** @return number of dumpped.
    **/
    int    dumpAll(type_xml_node &parent);
    type_cam_vector::iterator   begin(void);
    type_cam_vector::iterator   end(void);
    type_cam_vector::size_type  size(void);
    /** @brief search first empty slot and assign from cam.if no empty,add one.
     * @details cam.name must have some value.
    **/
    int          addCam(const Camera &cam);
    /** @brief try to get a camera object at position pos.throw exception if pos invalid.
    **/
    Camera&      get(int pos);
    /** @brief 查找名称为name的camera.
     * @return -1 if not found.
    **/
    int           find(const std::string &name);
};

}

#endif //GME_DM_DOCCAMERA_H

