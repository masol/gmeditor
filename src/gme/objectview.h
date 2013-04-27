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

    EventListen<int,type_callback >      m_eventListen;
public:
    enum{
        EVT_SELECTION_CHANGED,
        EVT_ITEMEXPANDING,
        EVT_ITEMEXPANDED
    };

    template<class T>
    inline void    addEventListen(int id,T &v)
    {
        m_eventListen.addEventListen(id,v);
    }
    /** @brief 返回当前选择对象的id.
    **/
    bool    getSelection(std::string &id,std::string *pmatid = NULL);
	bool	delSelection();
	bool    isSelected();

    void    refreshAll(void);
	void	refresh(const std::string &parent);

    ObjectView(wxWindow* parent, wxWindowID id,const wxPoint& pos, const wxSize& size);
    ~ObjectView();
private:
    wxDECLARE_EVENT_TABLE();
};

}

#endif //GME_OBJECTVIEW_H
