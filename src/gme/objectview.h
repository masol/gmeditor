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

#ifndef  GME_OBJECTVIEW_H
#define  GME_OBJECTVIEW_H

#include <wx/wx.h>
#include <wx/treelist.h>
#include "dm/docmat.h"
#include "utils/eventlisten.h"

class wxTreeListCtrl;
class wxSizer;

namespace gme{

class ObjectNode;
class DocObj;

class ObjectView : public wxPanel
{
public:
    typedef boost::function<void (const std::string &,const std::string &) > type_callback;
private:
    typedef wxPanel inherited;
protected:
    wxTreeListCtrl      *m_treelist;
    wxSizer             *m_sizer;
    ///@brief indicator of menu command target.
    std::string         m_menuCmdTarget;
    wxTreeListCtrl*     CreateTreeListCtrl(long style);

    void OnSelectionChanged(wxTreeListEvent& event);
    void OnItemExpanding(wxTreeListEvent& event);
    void OnItemExpanded(wxTreeListEvent& event);
    void OnItemChecked(wxTreeListEvent& event);
    void OnItemActivated(wxTreeListEvent& event);
    void OnItemContextMenu(wxTreeListEvent& event);
    void addChild(wxTreeListItem& parent,const ObjectNode &pNode,DocMat &pobjop);
    void refresh(void);
	wxTreeListItem FindItem(const std::string &id,const wxTreeListItem &parent);
    void deleteChild(wxTreeListItem &parent);


    void    refreshAll(void);
	void	refresh(const std::string &parent);

	///@brief 右钮菜单命令:
	void onMenuImport(wxCommandEvent &event);
	void onMenuViewAll(wxCommandEvent &event);
	void onMenuSetCenter(wxCommandEvent &event);
	void onSaveMaterial(wxCommandEvent &event);
	void onExportMaterial(wxCommandEvent &event);
	void onImportMaterial(wxCommandEvent &event);
    void onImportGlueMaterial(wxCommandEvent &event);
    void onExportGlueMaterial(wxCommandEvent &event);
    //可以通过Option,环境变量来配置glueserver.缺省是www.render001.com
    std::string     m_glueserver;
private:
	void  onDocumentItemSelected(const std::string &id);
	void  onDocumentItemDeselected(const std::string &id);
	void  onDocumentItemAdded(const std::string &parentId);
	void  onDocumentItemRemoved(const std::string &selfId);
	void  onDocumentOpend(void);
	void  onDocumentClosed(void);
public:
    ObjectView(wxWindow* parent, wxWindowID id,const wxPoint& pos, const wxSize& size);
    ~ObjectView();
private:
    wxDECLARE_EVENT_TABLE();
};

}

#endif //GME_OBJECTVIEW_H
