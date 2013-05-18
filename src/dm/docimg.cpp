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

//include OpenGL
#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

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

void
DocImg::onImagesizeChanged(type_imagesize_handler handler)
{
    pDocData->imageSize_Evt.addEventListen(handler);
}


bool
DocImg::setSize(int w,int h)
{
    if(pDocData->m_session && pDocData->m_session->film)
    {
        slg::RenderSession  *session = pDocData->m_session.get();
        int ow = session->film->GetWidth ();
        int oh = session->film->GetHeight ();
        if(ow != w && oh != h)
        {
            // RTPATHOCL doesn't support FILM_EDIT so I use a stop/start here
            session->Stop();

            session->renderConfig->scene->camera->Update(w,h);
            session->film->Init(session->renderConfig->scene->camera->GetFilmWeight(),
                    session->renderConfig->scene->camera->GetFilmHeight());

            session->Start();

            pDocData->imageSize_Evt.fire(w,h);

        }
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


const float*
DocImg::getPixels(void)
{
    slg::RenderSession* session = pDocData->getSession();

    if(session && session->film)
    {
        session->renderEngine->UpdateFilm();
	    session->film->UpdateScreenBuffer();
	    return session->film->GetScreenBuffer();
    }
    return NULL;
}

bool
DocImg::saveImage(const std::string &fullpath)
{
    slg::RenderSession* session = pDocData->getSession();

    if(session && session->film)
    {
        try{
            session->renderEngine->UpdateFilm();
            session->film->UpdateScreenBuffer();
            session->renderConfig->cfg.SetString("image.filename", fullpath);
            session->SaveFilmImage();
        }catch(std::exception err)
        {
            std::cerr << err.what() << std::endl;
        }
        return true;
    }
    return false;
}

void
DocImg::drawSelectedObject(ViewPort &vp)
{
    if(!pDocData->m_session.get() || !pDocData->m_session->renderConfig->scene)
        return;
    slg::Scene *scene = pDocData->m_session->renderConfig->scene;
    std::vector<std::string>&   selection = pDocData->getSelection();
    if(selection.size() == 0)
        return;

    // switch to projection matrix
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    // reset projection matrix
    glLoadIdentity();

//    std::cerr << "vp.x = " << vp.x << ",vp.y=" << vp.y << ",vp.width=" << vp.width << ",vp.height = " << vp.height << std::endl;
    glViewport(vp.x,vp.y,vp.width,vp.height);

    u_int width = pDocData->m_session->film->GetWidth();
    u_int height = pDocData->m_session->film->GetHeight();

    double aspect = (double)width/ (double)height ;

    //adjustment fovy.
    double fovy = scene->camera->fieldOfView;
    if(aspect > 1.0f)
        fovy = fovy / aspect;
    //float aspectradius = (aspect < 1.0f ? 1.0f : aspect);
    ///@todo coordinate covert.need to know detail about slg coordinate.
    gluPerspective( fovy, aspect , scene->camera->clipHither, scene->camera->clipYon);
    // switch to modelview matrix
    glMatrixMode(GL_MODELVIEW);
    // save current modelview matrix
    glPushMatrix();
    // reset modelview matrix
    glLoadIdentity();
    //setup camera.
    gluLookAt(scene->camera->orig.x,scene->camera->orig.y,scene->camera->orig.z,\
        scene->camera->target.x,scene->camera->target.y,scene->camera->target.z,\
        scene->camera->up.x,scene->camera->up.y,scene->camera->up.z);

    //set selection color. 
    ///@todo configure this.
    glColor3f(1.0f,1.0f,1.0f);
    //ensure mode. do not need restore. @fixme: move it to init?
    glEnableClientState(GL_VERTEX_ARRAY);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    BOOST_FOREACH(const std::string &oid,selection)
    {
        ObjectNodePath  path;
        ObjectNode *pSelf = pDocData->objManager.getRoot().findObject(oid,&path);
        if(pSelf)
        {
            Eigen::Matrix4f matrix;
            //从根路径下开始设置矩阵。
            size_t size = path.size() - 1;
            for(size_t idx = 0; idx < size; idx++)
            {
                path.getNode(idx)->applyMatrix(matrix);
            }
            pSelf->draw(matrix);
        }
    }
    // restore modelview matrix
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    // restore projection matrix
    glPopMatrix();

}



bool
DocImg::getRenderInfo(RenderInfo &ri)
{
    slg::RenderSession* session = pDocData->getSession();

    if(session && session->film)
    {
        ri.convergence = session->renderEngine->GetConvergence();
        ri.elapsedTime = session->renderEngine->GetRenderingTime();
        ri.pass = session->renderEngine->GetPass();
        ri.totalRaysSec = session->renderEngine->GetTotalRaysSec();
        ri.totalSamplesSec = session->renderEngine->GetTotalSamplesSec();
        return true;
    }
    return false;
}

bool
DocImg::getData(ImageDataBase *pdata)
{
    slg::RenderSession* session = pDocData->getSession();

    if(session && session->film)
    {
//        char    captionBuffer[1024];
//    	sprintf(captionBuffer, "[Pass %3d][Avg. samples/sec % 3.2fM][Avg. rays/sec % 4dK on %.1fK tris]",
//		    session->renderEngine->GetPass(),
//		    session->renderEngine->GetTotalSamplesSec() / 1000000.0,
//		    int(session->renderEngine->GetTotalRaysSec() / 1000.0),
//		    session->renderConfig->scene->dataSet->GetTotalTriangleCount() / 1000.f);
//		std::cerr << captionBuffer << std::endl;

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
