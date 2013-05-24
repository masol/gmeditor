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
#include <boost/algorithm/string.hpp>
#include "filedialog.h"

namespace gme{

const char*     OpenImageDialog::sv_allsupportedExt = "*.bmp;*.cut;*.dds;*.exr;*.g3;*.gif;*.hdr;*.ico;*.iff;*.jbig;*.jng;*.jpg;*.jpeg;*.jpg2;*.j2k;*.koa;*.pcd;*.mng;*.pcx;*.pbm;*.pgm;*.ppm;*.pfm;*.png;*.pic;*.psd;*.raw;*.ras;*.sgi;*.tga;*.tif;*.wbmp;*.xbm;*.xpm";
const char*     OpenSceneDialog::sv_allsupportedExt = "*.3ds;*.blend;*.dae;*.fbx;*.ifc;*.ase;*.dxf;*.hmp;*.md2;*.md3;*.md5;*.mdc;*.mdl;*.nff;*.ply;*.stl;*.x;*.obj;*.smd;*.lwo;*.lxo;*.lws;*.ter;*.ac3d;*.ms3d;*.cob;*.q3bsp;*.xgl;*.csm;*.bvh;*.b3d;*.ndo;*.xml;*.q3d;*.cfg;*.sps;*.slg;*.fbx";

bool
FileDialogBase::filenameWithExtension(const std::string &filename,const char* extSet)
{
    std::vector< std::string >    suffixArray;
    boost::split(suffixArray,extSet,boost::is_any_of(";"),boost::token_compress_on);
    BOOST_FOREACH(const std::string &suf,suffixArray)
    {
        if(!suf.empty() && boost::iends_with(filename,&suf.c_str()[1]))
        {
            return true;
        }
    }
    return false;
}

bool
OpenImageDialog::isSupported(const std::string &filename)
{
    return filenameWithExtension(filename,sv_allsupportedExt);
}

bool
OpenSceneDialog::isSupported(const std::string &filename)
{
    return filenameWithExtension(filename,sv_allsupportedExt);
}



}