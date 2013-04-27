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
#include "dm/docimg.h"
#include "slg/slg.h"
#include "luxrays/utils/properties.h"
#include "docprivate.h"

namespace gme{

bool
DocImg::getSize(int &w,int &h)
{
    if(pDocData->m_session && pDocData->m_session->film)
    {
        w = pDocData->m_session->film->GetWidth ();
        h = pDocData->m_session->film->GetHeight ();
        return true;
    }
    return false;
}

bool
DocImg::getData(ImageDataScale *pdata,int w, int h,const float* pixels)
{
	bool bSameDim = true;
	float scaleWidth,scaleHeight;
	if(w != pdata->width || h != pdata->height)
	{
		bSameDim = false;
		float srcRatio = (float)w / (float)h;
		float destRatio = (float)pdata->width / (float)pdata->height;
    	if(srcRatio > destRatio)
    	{
            pdata->left = 0;
            pdata->right = pdata->width;
            int realHeight = (int)(pdata->width / srcRatio);
            pdata->top = (pdata->height - realHeight) / 2;
            pdata->bottom = pdata->height - pdata->top;
        }else{
            pdata->top = 0;
            pdata->bottom = pdata->height;
            int realWidth = (int)(pdata->height * srcRatio);
            pdata->left = (pdata->width - realWidth) / 2;
            pdata->right = pdata->width - pdata->left;
        }
		scaleWidth = (float)(pdata->right - pdata->left) / (float)w;
    	scaleHeight = (float)(pdata->bottom - pdata->top) / (float)h;
	}else{
        pdata->left = pdata->top = 0;
        pdata->right = w;
        pdata->bottom = h;
    }

	for(int y = 0; y < pdata->height; y++)
	{
	    unsigned char* line = &pdata->data[(pdata->height - y - 1) * pdata->pitch];
	    if(y < pdata->top || y >= pdata->bottom)
	    {
		    for(int x = 0; x < pdata->width; x++)
		    {
			    int c = x * 3;
			    line[c] = pdata->default_red;
			    line[c+1] = pdata->default_green;
			    line[c+2] = pdata->default_blue;
		    }
        }else{
	        const float* srcline;
	        //nearest match resize.
	        if(bSameDim)
	        {
		        srcline = &pixels[y * w * 3];
	        }else{
		        srcline = &pixels[(((int)( (y - pdata->top) / scaleHeight)) * (w *3))];
	        }
	        for(int x = 0; x < pdata->width; x++)
	        {
		        unsigned char r,g,b;
		        if(bSameDim)
		        {
			        int index = x * 3;
			        r = (unsigned char)(srcline[index] * 255 );
			        b = (unsigned char)(srcline[index + 2]* 255 );
			        g = (unsigned char)(srcline[index + 1]* 255 );
		        }else{
		            if(x < pdata->left || x >= pdata->right)
		            {
			            r = pdata->default_red;
			            g = pdata->default_green;
			            b = pdata->default_blue;
                    }else{
    			        int index = (((int)( (x - pdata->left) / scaleWidth)) *3);
			            r = (unsigned char)(srcline[index] * 255 );
			            g = (unsigned char)(srcline[index + 1]* 255 );
			            b = (unsigned char)(srcline[index + 2]* 255 );
                    }
		        }
		        int c = x * 3;
		        line[c] = r;
		        line[c+1] = g;
		        line[c+2] = b;
	        }
        }
	}
    return false;
}


bool
DocImg::getData(ImageDataBase *pdata)
{
    slg::RenderSession* session = pDocData->getSession();

    if(session && session->film)
    {
        char    captionBuffer[1024];
    	sprintf(captionBuffer, "[Pass %3d][Avg. samples/sec % 3.2fM][Avg. rays/sec % 4dK on %.1fK tris]",
		    session->renderEngine->GetPass(),
		    session->renderEngine->GetTotalSamplesSec() / 1000000.0,
		    int(session->renderEngine->GetTotalRaysSec() / 1000.0),
		    session->renderConfig->scene->dataSet->GetTotalTriangleCount() / 1000.f);
		std::cerr << captionBuffer << std::endl;

        session->renderEngine->UpdateFilm();
	    session->film->UpdateScreenBuffer();
	    const float *pixels = session->film->GetScreenBuffer();
	    if(pdata->type == ImageDataBase::ID_SCALE)
	    {
	        return getData(reinterpret_cast<ImageDataScale*>(pdata),session->film->GetWidth(), session->film->GetHeight(),pixels);
        }
    }
    return false;
}


} //end namespace gme.
