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
#include <wx/sizer.h>
#include <boost/bind.hpp>
#include <boost/locale.hpp>
#include "cameraview.h"
#include "dm/docio.h"
#include "dm/doccamera.h"
#include "utils/i18n.h"
#include "stringutil.h"
#include "cmdids.h"
#include "iconlist.h"

namespace gme{


BEGIN_EVENT_TABLE(CameraView, CameraView::inherited)
    EVT_LIST_ITEM_SELECTED(wxID_ANY, CameraView::OnSelectionChanged)
    EVT_LIST_END_LABEL_EDIT(wxID_ANY, CameraView::OnEndLabelEdit)
    EVT_CONTEXT_MENU(CameraView::OnContextMenu)
	EVT_MENU(cmd::GID_CAM_NEWFROMCURRENT,CameraView::onMenuNewFromCurrent)
	EVT_MENU(cmd::GID_CAM_DELETE,CameraView::onMenuDelete)
END_EVENT_TABLE()

CameraView::CameraView(wxWindow* parent, wxWindowID id,const wxPoint& pos, const wxSize& size)
    : inherited(parent,id,pos,size,wxLC_LIST | wxLC_SINGLE_SEL | wxBORDER_THEME | wxLC_EDIT_LABELS)
{
    m_menuCmdTarget = -1;
    DocIO   dio;
    dio.onSceneLoaded(boost::bind(&CameraView::onDocumentOpend,this));
    dio.onSceneClosed(boost::bind(&CameraView::onDocumentClosed,this));

	this->SetImageList(IconList::instance().getImageList(),wxIMAGE_LIST_SMALL);
}

CameraView::~CameraView()
{

}

void
CameraView::onDocumentOpend(void)
{
    refresh();
}

void
CameraView::onDocumentClosed(void)
{
    m_menuCmdTarget = -1;
    this->DeleteAllItems();
}

void
CameraView::OnSelectionChanged(wxListEvent& event)
{
    long idx = this->GetItemData(event.m_itemIndex);
    DocCamera doccam;
    if(idx >= 0 && doccam.setSelected(idx))
    {
        Camera  &cam = doccam.get(idx);
        doccam.restoreFrom(cam);
    }
}

void
CameraView::onMenuDelete(wxCommandEvent& event)
{
    if(m_menuCmdTarget >= 0)
    {
        ///@todo implement camera delete.
        m_menuCmdTarget = -1;
    }
}


void
CameraView::newCamFromCurrent(void)
{
    DocCamera   doccam;

    Camera  cam;
    if(doccam.saveTo(cam))
    {
        cam.name = __("未命名");
        int idx = doccam.addCam(cam);

        std::cerr << "idx = " << idx <<  std::endl;

      	DECLARE_WXCONVERT;

        wxString    name( (cam.name.empty() ? "未命名" : cam.name.c_str()),gme_wx_utf8_conv);

		long itemIdx = this->InsertItem(idx, name, IconList::instance().getIcon("icon_camera"));
        if(itemIdx >= 0)
        {
            this->SetItemData(itemIdx,idx);
            this->SetItemState(itemIdx,wxLIST_STATE_SELECTED,wxLIST_STATE_SELECTED);
        }
    }
}


void
CameraView::onMenuNewFromCurrent(wxCommandEvent &event)
{
    newCamFromCurrent();
}


void
CameraView::OnContextMenu(wxContextMenuEvent& event)
{
    if (GetEditControl() == NULL)
    {
        int flags;
        long id = this->HitTest(this->ScreenToClient(event.GetPosition()),flags,NULL);

        DECLARE_WXCONVERT;
        wxMenu  menu;
        menu.Append(cmd::GID_CAM_NEWFROMCURRENT, gmeWXT("保存当前视角"));
        if(id != wxNOT_FOUND && this->GetItemCount() > 1)
        {
            m_menuCmdTarget = id;
            menu.Append(cmd::GID_CAM_DELETE, gmeWXT("删除视角"));
        }
        PopupMenu(&menu);
    }
}

void
CameraView::OnEndLabelEdit(wxListEvent& event)
{
    if(!event.IsEditCancelled())
    {
        DocCamera   doccam;
        long id = event.GetData();
        BOOST_ASSERT_MSG(id >= 0 && id < (long)doccam.size(), "invalid camera position!");
        Camera   &cam = doccam.get(id);
        cam.name = boost::locale::conv::utf_to_utf<char>(event.m_item.m_text.ToStdWstring());
    }
}


void
CameraView::refresh(void)
{
	DECLARE_WXCONVERT;
    DocCamera   doccam;
    size_t count = doccam.size();

    for(size_t i = 0; i <count; i++)
    {
        Camera   &cam = doccam.get(i);
        wxString    name( (cam.name.empty() ? "未命名" : cam.name.c_str()),gme_wx_utf8_conv);
		long itemIdx = this->InsertItem(i, name, IconList::instance().getIcon("icon_camera"));
        if(itemIdx >= 0)
        {
            this->SetItemData(itemIdx,i);
        }
    }
    int sel = doccam.getSelected();
    if(sel >=0 && sel < (int)count)
    {
        this->SetItemState(sel,wxLIST_STATE_SELECTED,wxLIST_STATE_SELECTED);
    }
}



}
