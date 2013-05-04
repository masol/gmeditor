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
#include "stringutil.h"

namespace gme{

class FileDialogBase
{
protected:
    FileDialogBase()
    {
    }
    wxFileDialog    *pDialog;
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
public:
    OpenImageDialog(wxWindow *parent)
    {
        DECLARE_WXCONVERT;
	    pDialog = new wxFileDialog(parent, gmeWXT("选择打开文件"), _(""), _(""), 
						   gmeWXT("所有文件|*.*"), 
						   wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    }
};


class OpenSceneDialog : public FileDialogBase
{
public:
    OpenSceneDialog(wxWindow *parent)
    {
        DECLARE_WXCONVERT;
	    pDialog = new wxFileDialog(parent, gmeWXT("选择打开文件"), _(""), _(""), 
						   gmeWXT("所有文件|*.*|CFG文件(*.cfg)|*.cfg|SPS文件(*.sps)|*.sps|CTM文件(*.ctm)|*.ctm"), 
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
						       gmeWXT("所有文件|*.*"),
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
						       gmeWXT("所有文件|*.*|模型文件(*.ply)|*.ply"),
						       wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    }
};

}
#endif