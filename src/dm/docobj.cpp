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
#include "dm/docobj.h"
#include "slg/slg.h"
#include "docprivate.h"
#include "slgobject.h"
#include <boost/assert.hpp>

namespace gme{

ObjectNode&
DocObj::getRootObject()
{
    return pDocData->objManager.getRoot();
}

bool
DocObj::importObject(const std::string &path,ObjectNode *pParent)
{
    if(pDocData->m_session.get() == NULL)
        return false;

    ImportContext   ctx(pDocData->m_session->renderConfig->scene,path);

    SlgUtil::Editor editor(pDocData->m_session.get());
    if(!pParent)
    {
        pParent = &getRootObject();
    }

    bool    bLoadSuc = false;
    if(boost::iends_with(path,".sps"))
    {//importSpScene中假定传入Parent对象。
        int count = ExtraObjectManager::importSpScene(path,*pParent,ctx);
        bLoadSuc = (count > 0);
    }else{
        ObjectNode  node;
        if(pDocData->objManager.importObjects(path,node,ctx))
        {
            if(node.isPureGroup())
            {//减少层数。
                ObjectNode::type_child_container::iterator it = node.begin();
                while(it != node.end())
                {
                    pParent->addChild(*it);
                    it++;
                }
            }else{
                pParent->addChild(node);
            }
            bLoadSuc = true;
        }
    }

    if(bLoadSuc)
    {
        editor.addAction(ctx.getAction());
        pDocData->fireSelection(DocPrivate::SEL_ITEMCHILDADDED,pParent->id());
    }

    return bLoadSuc;
}

bool
DocObj::deleteObject(const std::string &id)
{
    return pDocData->objManager.removeMesh(id);
}

void
DocObj::deleteAllSelection(void)
{
    while(size_t size = pDocData->getSelection().size())
    {
        const std::string   &id = pDocData->getSelection().back();
        deleteObject(id);
        if(size == pDocData->getSelection().size())
        {
            BOOST_ASSERT_MSG(false,"why selection not clear??");
            pDocData->getSelection().pop_back();
            pDocData->fireSelection(DocPrivate::SEL_ITEMSELFREMOVED,id);
        }
    }
}


void
DocObj::clearSelection(void)
{
    pDocData->clearSelection();
}

bool
DocObj::select(const std::string &id)
{
    return pDocData->addSelection(id);
}

bool
DocObj::deselect(const std::string &id)
{
    return pDocData->removeSelection(id);
}

void
DocObj::onSelected(type_selection_handler handler)
{
    pDocData->selection_Evt.addEventListen(DocPrivate::SEL_ITEMSELECTED,handler);
}

void
DocObj::onDeselected(type_selection_handler handler)
{
    pDocData->selection_Evt.addEventListen(DocPrivate::SEL_ITEMDELSELECTED,handler);
}

void
DocObj::onChildAdded(type_selection_handler handler)
{
    pDocData->selection_Evt.addEventListen(DocPrivate::SEL_ITEMCHILDADDED,handler);
}

void
DocObj::onSelfRemoved(type_selection_handler handler)
{
    pDocData->selection_Evt.addEventListen(DocPrivate::SEL_ITEMSELFREMOVED,handler);
}


const std::vector<std::string>&
DocObj::getSelection(void)
{
    return pDocData->getSelection();
}

} //end namespace gme.
