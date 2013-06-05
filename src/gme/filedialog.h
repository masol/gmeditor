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
#ifndef  GME_FILEDIALOG_H
#define  GME_FILEDIALOG_H

#include <wx/wx.h>
#include <boost/shared_ptr.hpp>
#include <boost/locale.hpp>
#include <boost/format.hpp>
#include "utils/i18n.h"
#include "stringutil.h"

namespace gme{

class FileDialogBase
{
protected:
    FileDialogBase()
    {
    }
    wxFileDialog    *pDialog;
    static  bool    filenameWithExtension(const std::string &filename,const char* extSet);
public:
    ~FileDialogBase()
    {
        pDialog->Destroy();
    }
    inline int ShowModal(void)
    {
        return pDialog->ShowModal();
    }
    inline std::string GetPath()
    {
        return boost::locale::conv::utf_to_utf<char>(pDialog->GetPath().ToStdWstring());
    }
};

class OpenImageDialog : public FileDialogBase
{
private:
    static  const char*     sv_allsupportedExt;
public:
    static  bool    isSupported(const std::string &filename);
    static  wxString  getDefaultImageWildcard(void);
    OpenImageDialog(wxWindow *parent)
    {
        DECLARE_WXCONVERT;

	    pDialog = new wxFileDialog(parent, gmeWXT("选择打开文件"), _(""), _(""),
                           getDefaultImageWildcard(),
						   wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    }
};

class SaveImageDialog : public FileDialogBase
{
public:
    SaveImageDialog(wxWindow *parent)
    {
        DECLARE_WXCONVERT;
	    pDialog = new wxFileDialog(parent, gmeWXT("图片保存为"), _(""), _(""),
						       gmeWXT("支持文件|*.bmp;*.cut;*.dds;*.exr;*.g3;*.gif;*.hdr;*.ico;*.iff;*.jbig;*.jng;*.jpg;*.jpeg;*.jpg2;*.j2k;*.koa;*.pcd;*.mng;*.pcx;*.pbm;*.pgm;*.ppm;*.pfm;*.png;*.pic;*.psd;*.raw;*.ras;*.sgi;*.tga;*.tif;*.wbmp;*.xbm;*.xpm|BMP文件(*.bmp)|*.bmp|EXR文件(*.exr)|*.exr|GIF文件(*.gif)|*.gif|HDR文件(*.hdr)|*.hdr|ICO文件(*.ico)|*.ico|JBIG文件(*.jbig)|*.jbig|JNG文件(*.jng)|*.jng|JPEG文件(*.jpeg)|*.jpeg|JPEG-2000文件(*.jpg2)|*.jpg2|JPEG-2000 codestream文件(*.j2k)|*.j2k|PBM文件(*.pbm)|*.pbm|PGM文件(*.pgm)|*.pgm|PPM文件(*.ppm)|*.ppm|PFM文件(*.pfm)|*.pfm|PNG文件(*.png)|*.png|TARGA文件(*.targa)|*.targa|TIFF文件(*.tiff)|*.tiff|WBMP文件(*.wbmp)|*.wbmp|XPM文件(*.xpm)|*.xpm|所有文件(*.*)|*.*"),
						       wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    }
};

class SaveMaterialDialog : public FileDialogBase
{
public:
    SaveMaterialDialog(wxWindow *parent)
    {
        DECLARE_WXCONVERT;
	    pDialog = new wxFileDialog(parent, gmeWXT("材质保存为"), _(""), _(""),
						       gmeWXT("SPM文件(*.spm)|*.spm|所有文件(*.*)|*.*"),
						       wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    }
};


class ImportMaterialDialog : public FileDialogBase
{
public:
    ImportMaterialDialog(wxWindow *parent)
    {
	    DECLARE_WXCONVERT;
	    pDialog = new wxFileDialog(parent, gmeWXT("选择打开文件"), _(""), _(""),
						       gmeWXT("SPM文件(*.spm)|*.spm|所有文件(*.*)|*.*"),
						       wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    }
};




class OpenSceneDialog : public FileDialogBase
{
private:
    static  const char*     sv_allsupportedExt;
public:
    static  bool    isSupported(const std::string &filename);
    OpenSceneDialog(wxWindow *parent)
    {
        DECLARE_WXCONVERT;

        std::string content = boost::str(boost::format(__("支持文件|%s|3D MAX模型文件(*.3ds)|*.3ds|Blender文件(*.blend)|*.blend|Collada文件(*.dae)|*.dae|Kaydara数据交换文件(*.fbx)|*.fbx|IFC-STEP文件(*.ifc)|*.ifc|ASII场景导出文件(*.ase)|*.ase|图形交换文件(*.dxf)|*.dxf|地形实体文件(*.hmp)|*.hmp|Quake 2模型文件(*.md2)|*.md2|Quake 3模型文件(*.md3)|*.md3|校验和文件(*.md5)|*.md5|MDC文件(*.mdc)|*.mdc|3DGS模型实体文件(*.mdl)|*.mdl|场景描述文件(*.nff)|*.nff|PLY文件(*.ply)|*.ply|STL文件(*.stl)|*.stl|Amapi模型文件(*.x)|*.x|OBJ文件(*.obj)|*.obj|源引擎SDK模型文件(*.smd)|*.smd|LightWave物体文件(*.lwo)|*.lwo|modo 302图形文件(*.lxo)|*.lxo|LightWave场景文件(*.lws)|*.lws|TER文件(*.ter)|*.ter|3D格式数据文件(*.ac3d)|*.ac3d|MilkShape模型文件(*.ms3d)|*.ms3d|Caligari trueSpace 3D物体文件(*.cob)|*.cob|Q3BSP文件(*.q3bsp)|*.q3bsp|XGL 3D CAD模型文件(*.xgl)|*.xgl|Character Studio标识文件(*.csm)|*.csm|Biovision层次动画文件(*.bvh)|*.bvh|Blitz3D/Max/Plus 贴图及网格文件(*.b3d)|*.b3d|Nendo文件(*.ndo)|*.ndo|Ogre XML文件(*.xml)|*.xml|Quickdraw 3D文件(*.q3d)|*.q3d|SLG文件(*.cfg)|*.cfg|SPS文件(*.slg;*.sps)|*.slg;*.sps|CTM文件(*.ctm)|*.ctm|所有文件(*.*)|*.*") )% sv_allsupportedExt);

	    pDialog = new wxFileDialog(parent, gmeWXT("选择打开文件"), _(""), _(""),
						   wxString(content.c_str(),gme_wx_utf8_conv),
						   wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    }
};

class SaveSceneDialog : public FileDialogBase
{
public:
    SaveSceneDialog(wxWindow *parent)
    {
	    DECLARE_WXCONVERT;
	    pDialog = new wxFileDialog(parent, gmeWXT("选择导出文件"), _(""), _(""),
						       gmeWXT("SPS文件(*.slg;*.sps)|*.slg;*.sps|所有文件(*.*)|*.*"),
						       wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    }
};

class ImportDialog : public FileDialogBase
{
public:
    ImportDialog(wxWindow *parent)
    {
	    DECLARE_WXCONVERT;
	    pDialog = new wxFileDialog(parent, gmeWXT("选择打开文件"), _(""), _(""),
						       gmeWXT("支持文件|*.3ds;*.blend;*.dae;*.sps;*.slg;*.fbx;*.ifc;*.ase;*.dxf;*.hmp;*.md2;*.md3;*.md5;*.mdc;*.mdl;*.nff;*.ply;*.stl;*.x;*.obj;*.smd;*.lwo;*.lxo;*.lws;*.ter;*.ac3d;*.ms3d;*.cob;*.q3bsp;*.xgl;*.csm;*.bvh;*.b3d;*.ndo;*.xml;*.q3d|3D MAX模型文件(*.3ds)|*.3ds|Blender文件(*.blend)|*.blend|Collada文件(*.dae)|*.dae|Kaydara数据交换文件(*.fbx)|*.fbx|IFC-STEP文件(*.ifc)|*.ifc|ASII场景导出文件(*.ase)|*.ase|图形交换文件(*.dxf)|*.dxf|地形实体文件(*.hmp)|*.hmp|Quake 2模型文件(*.md2)|*.md2|Quake 3模型文件(*.md3)|*.md3|校验和文件(*.md5)|*.md5|MDC文件(*.mdc)|*.mdc|3DGS模型实体文件(*.mdl)|*.mdl|场景描述文件(*.nff)|*.nff|PLY文件(*.ply)|*.ply|STL文件(*.stl)|*.stl|Amapi模型文件(*.x)|*.x|OBJ文件(*.obj)|*.obj|源引擎SDK模型文件(*.smd)|*.smd|LightWave物体文件(*.lwo)|*.lwo|modo 302图形文件(*.lxo)|*.lxo|LightWave场景文件(*.lws)|*.lws|TER文件(*.ter)|*.ter|3D格式数据文件(*.ac3d)|*.ac3d|MilkShape模型文件(*.ms3d)|*.ms3d|Caligari trueSpace 3D物体文件(*.cob)|*.cob|Q3BSP文件(*.q3bsp)|*.q3bsp|XGL 3D CAD模型文件(*.xgl)|*.xgl|Character Studio标识文件(*.csm)|*.csm|Biovision层次动画文件(*.bvh)|*.bvh|Blitz3D/Max/Plus 贴图及网格文件(*.b3d)|*.b3d|Nendo文件(*.ndo)|*.ndo|Ogre XML文件(*.xml)|*.xml|Quickdraw 3D文件(*.q3d)|*.q3d|SLG文件(*.cfg)|*.cfg|SPS文件(*.slg;*.sps)|*.slg;*.sps|CTM文件(*.ctm)|*.ctm|所有文件(*.*)|*.*"),
						       wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    }
};

}
#endif
