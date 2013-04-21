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

#ifndef  GME_DM_SLGUTILS_H
#define  GME_DM_SLGUTILS_H

#include "slg/rendersession.h"

namespace gme{

class SlgUtil
{
public:
    struct   Editor{
    protected:
        slg::RenderSession      *m_session;
    public:
        inline Editor(slg::RenderSession *session)
        {
            m_session = session;
            m_session->BeginEdit();
        }
        inline ~Editor()
        {
            m_session->EndEdit();
        }
        inline void addAction(const slg::EditAction a)
        {
            m_session->editActions.AddAction(a);
        }
    };

    static  inline  void    OutputSlgmat(std::ostream &o,const luxrays::Matrix4x4 &luxmat)
    {
        for(int col = 0; col < 4; col++)
        {
            for(int row = 0; row < 4; row++)
            {
                if(!col && !row)
                    o << ' ';
                o << boost::lexical_cast<std::string>(luxmat.m[row][col]);
            }
        }
    }
};

}

#endif //GME_DM_SLGUTILS_H

