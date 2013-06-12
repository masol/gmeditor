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
#ifndef  GME_ICONLIST_H
#define  GME_ICONLIST_H

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include "utils/singleton.h"

namespace gme{


class IconList : public Singleton<IconList>
{
    friend  class Singleton<IconList>;
    typedef Singleton<IconList> inherit;
public:
	int getIcon(std::string name);
	wxImageList* getImageList();

	~IconList()
	{
		if(m_imagelist)
			delete m_imagelist;
	}
private:
	IconList();
	void addIcon(std::string name,wxBitmap bitmap);
	boost::unordered_map<std::string,int> iconmap;
	wxImageList *m_imagelist;
};

}

#endif
