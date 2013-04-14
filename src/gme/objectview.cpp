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
#include "objectview.h"
#include "stringutil.h"
#include "dm/docobj.h"
#include <wx/sizer.h>


namespace gme{

BEGIN_EVENT_TABLE(ObjectView, inherited)
    EVT_TREELIST_SELECTION_CHANGED(wxID_ANY, ObjectView::OnSelectionChanged)
    EVT_TREELIST_ITEM_EXPANDING(wxID_ANY, ObjectView::OnItemExpanding)
    EVT_TREELIST_ITEM_EXPANDED(wxID_ANY, ObjectView::OnItemExpanded)
    EVT_TREELIST_ITEM_CHECKED(wxID_ANY, ObjectView::OnItemChecked)
    EVT_TREELIST_ITEM_ACTIVATED(wxID_ANY, ObjectView::OnItemActivated)
    EVT_TREELIST_ITEM_CONTEXT_MENU(wxID_ANY, ObjectView::OnItemContextMenu)
END_EVENT_TABLE()

ObjectView::ObjectView(wxWindow* parent, wxWindowID id,const wxPoint& pos, const wxSize& size)
    : inherited(parent,id,pos,size)
{
    m_treelist = CreateTreeListCtrl(wxTL_DEFAULT_STYLE);
    refresh();
    m_sizer = new wxBoxSizer(wxVERTICAL);
    m_sizer->Add(m_treelist,wxSizerFlags(1).Expand());
    SetSizer(m_sizer);
    Show();
}

void
ObjectView::refresh(void)
{
    gme::DocObj dobj;
    const gme::ObjectNode &root = dobj.getRootObject();
    std::vector<ObjectNode>::const_iterator it = root.m_children.begin();
    while(it != root.m_children.end())
    {
        addChild(m_treelist->GetRootItem(),&(*it),&dobj);
        it++;
    }
}

void
ObjectView::addChild(wxTreeListItem& parent,const ObjectNode* pNode,DocObj *pobjop)
{
    wxTreeListItem item = m_treelist->AppendItem(parent,pobjop->getObjectName(*pNode));
    m_treelist->SetItemText(item, 1, pobjop->getMatName(*pNode));
    m_treelist->SetItemText(item, 2, "");
    std::vector<ObjectNode>::const_iterator it = pNode->m_children.begin();
    while(it != pNode->m_children.end())
    {
        addChild(item,&(*it),pobjop);
        it++;
    }
}


wxTreeListCtrl*
ObjectView::CreateTreeListCtrl(long style)
{
	DECLARE_WXCONVERT;
    wxTreeListCtrl* const tree = new wxTreeListCtrl(this, wxID_ANY,
                                  wxDefaultPosition, wxDefaultSize,style);
    tree->AppendColumn(gmeWXT("名称"),
                       wxCOL_WIDTH_AUTOSIZE,
                       wxALIGN_LEFT,
                       wxCOL_RESIZABLE | wxCOL_SORTABLE);
    tree->AppendColumn(gmeWXT("材质"),
                       tree->WidthFor("0123456789"),
                       wxALIGN_RIGHT,
                       wxCOL_RESIZABLE | wxCOL_SORTABLE);
    tree->AppendColumn(gmeWXT("引用对象"),
                       tree->WidthFor("0123456789"),
                       wxALIGN_RIGHT,
                       wxCOL_RESIZABLE | wxCOL_SORTABLE);
    return tree;
}

ObjectView::~ObjectView()
{

}

void
ObjectView::OnSelectionChanged(wxTreeListEvent& event)
{

}

void
ObjectView::OnItemExpanding(wxTreeListEvent& event)
{

}

void
ObjectView::OnItemExpanded(wxTreeListEvent& event)
{

}

void
ObjectView::OnItemChecked(wxTreeListEvent& event)
{

}

void
ObjectView::OnItemActivated(wxTreeListEvent& event)
{

}

void
ObjectView::OnItemContextMenu(wxTreeListEvent& event)
{

}

} //end namespace gme
