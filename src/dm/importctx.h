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

#ifndef  GME_DM_IMPORTCTX_H
#define  GME_DM_IMPORTCTX_H


#include "slg/slg.h"
#include "slg/sdl/scene.h"
#include "slg/editaction.h"

namespace gme{

/** @brief Implements the import context.
 * @details ImportContext isolation new and import
**/
class ImportContext
{
private:
    slg::Scene      *m_scene;
    int             m_editAction;
public:
    ImportContext(slg::Scene* s)
    {
        m_scene = s;
        m_editAction = 0;
    }
    inline void addAction(const slg::EditAction a)
    {
        m_editAction |= a;
    }
    inline slg::EditAction getAction()const
    {
        return (slg::EditAction)m_editAction;
    }
    inline slg::Scene*  scene()const{
        return m_scene;
    }
};

}

#endif //GME_DM_IMPORTCTX_H

