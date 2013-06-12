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
#include "iconlist.h"
#include "stringutil.h"
#include "data/xpmres.h"

namespace gme{

IconList::IconList(){
	m_imagelist = new wxImageList(16, 16, true);
	// 添加所有需要用得到的icon
	this->addIcon("icon_camera",wxBitmap(xpm::camera));
	this->addIcon("icon_object",wxBitmap(xpm::camera));
}


void IconList::addIcon(std::string name,wxBitmap bitmap){
		int index = m_imagelist->Add(bitmap);
		iconmap[name] = index ;
	}

int IconList::getIcon(std::string name){
	return iconmap[name];
}

wxImageList* IconList::getImageList(){
	return m_imagelist;
}

}
