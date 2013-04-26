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

#ifndef  GME_DM_EIGENUTIL_H
#define  GME_DM_EIGENUTIL_H

#include <Eigen/Core>
#include "slg/rendersession.h"
#include <boost/format.hpp>

namespace gme{

class EigenUtil
{
public:
    static  inline  void    AssignFromSlg(Eigen::Matrix4f &mat,const luxrays::Matrix4x4 &luxmat)
    {
        for(int col = 0; col < 4; col++)
        {
            for(int row = 0; row < 4; row++)
            {
                mat(row,col) = luxmat.m[col][row];
            }
        }
    }
    static  inline  void    AssignFromSlg(Eigen::Vector3f &v,const luxrays::Point &pt)
    {
        v[0] = pt.x;
        v[1] = pt.y;
        v[2] = pt.z;
    }
    static  inline  void    AssignFromSlg(Eigen::Vector3f &v,const luxrays::Vector &vec)
    {
        v[0] = vec[0];
        v[1] = vec[1];
        v[2] = vec[2];
    }

    static  inline  void    AssignToSlg(const Eigen::Vector3f &vec,luxrays::Vector &v)
    {
        v[0] = vec[0];
        v[1] = vec[1];
        v[2] = vec[2];
    }
    static  inline  void    AssignToSlg(const Eigen::Vector3f &v,luxrays::Point &pt)
    {
        pt.x = v[0];
        pt.y = v[1];
        pt.z = v[2];
    }

    ///@brief may throw exception!
    static  inline  void    AssignFromString(Eigen::Vector3f &v,const std::string value)
    {
        std::vector<float> va = luxrays::Properties::ConvertToFloatVector(value);
        v[0] = va[0];
        v[1] = va[1];
        v[2] = va[2];
    }

    static  inline  void    OutputLuxmat(std::ostream &o,const luxrays::Matrix4x4 &luxmat)
    {
        for(int col = 0; col < 4; col++)
        {
            for(int row = 0; row < 4; row++)
            {
                if(!col && !row)
                    o << ' ';
                o << boost::lexical_cast<std::string>(luxmat.m[row][col]);
            }
        }
    }

    static  inline  std::string   Vector2String(const Eigen::Vector3f &v)
    {
        return boost::str(boost::format("%.3f %.3f %.3f") % v[0] % v[1] % v[2]);
    }
};

}

#endif //GME_DM_EIGENUTIL_H

