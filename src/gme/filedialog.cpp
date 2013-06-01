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

wxString
OpenImageDialog::getDefaultImageWildcard(void)
{
    DECLARE_WXCONVERT;
    std::string content = boost::str(boost::format(__("支持文件|%s|BMP文件(*.bmp)|*.bmp|Dr. Halo CUT文件(*.cut)|*.cut|DDS文件(*.dds)|*.dds|EXR文件(*.exr)|*.exr|Raw Fax G3文件(*.g3)|*.g3|GIF文件(*.gif)|*.gif|HDR文件(*.hdr)|*.hdr|ICO文件(*.ico)|*.ico|IFF文件(*.iff)|*.iff|JBIG文件(*.jbig)|*.jbig|JNG文件(*.jng)|*.jng|JPEG文件(*.jpeg;*.jpg;*.j2k)|*.j2k;*.jpg;*.jpeg|KOALA文件(*.koa)|*.koa|Kodak PhotoCD文件(*.pcd)|*.pcd|MNG文件(*.mng)|*.mng|pcx文件(*.pcx)|*.pcx|PBM文件(*.pbm)|*.pbm|PGM文件(*.pgm)|*.pgm|PPM文件(*.ppm)|*.ppm|PFM文件(*.pfm)|*.pfm|PNG文件(*.png)|*.png|Macintosh PICT文件(*.pict)|*.pict|Photoshop PSD文件(*.psd)|*.psd|RAW camera文件(*.raw)|*.raw|Sun RAS文件(*.ras)|*.ras|SGI文件(*.sgi)|*.sgi|TARGA文件(*.tga)|*.tga|TIFF文件(*.tif;*.tiff)|*.tif;*.tiff|WBMP文件(*.wbmp)|*.wbmp|XBM文件(*.xbm)|*.xbm|XPM文件(*.xpm)|*.xpm|所有文件(*.*)|*.*") )% sv_allsupportedExt);
    return wxString(content.c_str(),gme_wx_utf8_conv);
}


bool
OpenSceneDialog::isSupported(const std::string &filename)
{
    return filenameWithExtension(filename,sv_allsupportedExt);
}



}
