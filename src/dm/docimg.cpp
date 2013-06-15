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

class PrepareDrawEnv
{
private:
    void gldPerspective(GLdouble fovx, GLdouble aspect, GLdouble zNear, GLdouble zFar)
    {
       // This code is based off the MESA source for gluPerspective
       // *NOTE* This assumes GL_PROJECTION is the current matrix


       GLdouble xmin, xmax, ymin, ymax;
       GLdouble m[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

       xmax = zNear * tan(fovx * M_PI / 360.0);
       xmin = -xmax;

       ymin = xmin / aspect;
       ymax = xmax / aspect;

       // Set up the projection matrix
       m[0] = (2.0 * zNear) / (xmax - xmin);
       m[5] = (2.0 * zNear) / (ymax - ymin);
       m[10] = -(zFar + zNear) / (zFar - zNear);

       m[2] = (xmax + xmin) / (xmax - xmin);
       m[6] = (ymax + ymin) / (ymax - ymin);
       m[11] = -1.0;

       m[14] = -(2.0 * zFar * zNear) / (zFar - zNear);

       // Add to current matrix
       glMultMatrixd(m);
    }
public:
    PrepareDrawEnv(DocImg::ViewPort &vp,slg::RenderSession *session)
    {
        // switch to projection matrix
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        // reset projection matrix
        glLoadIdentity();

        glViewport(vp.x,vp.y,vp.width,vp.height);

        u_int width = session->film->GetWidth();
        u_int height = session->film->GetHeight();

        double aspect = (double)width/ (double)height ;

        slg::PerspectiveCamera *camera = session->renderConfig->scene->camera;
        //adjustment fovy.
        double fovx = camera->fieldOfView;
        //coordinate covert.need to know detail about slg coordinate.
        if(aspect > 1.0f)
        {
            gldPerspective(fovx,aspect, camera->clipHither, camera->clipYon);
        }else{
            double fovy = fovx;
            gluPerspective( fovy, aspect , camera->clipHither, camera->clipYon);
        }
        // switch to modelview matrix
        glMatrixMode(GL_MODELVIEW);
        // save current modelview matrix
        glPushMatrix();
        // reset modelview matrix
        glLoadIdentity();
        //setup camera.
        gluLookAt(camera->orig.x,camera->orig.y,camera->orig.z,\
            camera->target.x,camera->target.y,camera->target.z,\
            camera->up.x,camera->up.y,camera->up.z);

        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    }

    ~PrepareDrawEnv()
    {
        // restore modelview matrix
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        // restore projection matrix
        glPopMatrix();
    }
};

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
            pDocData->cachefilm().invalidate();
            pDocData->imageSize_Evt.fire(w,h);
            pDocData->setModified();
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

void
DocImg::updateNative(void)
{
    pDocData->cachefilm().updateNativeFilm();
}

const float*
DocImg::getPixels(void)
{
    return pDocData->cachefilm().getPixels();
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

int
DocImg::getScreenRefreshInterval(void)
{
    if(pDocData->getSession())
        pDocData->getSession()->renderConfig->GetScreenRefreshInterval();
    return 0;
}

void
DocImg::setScreenRefreshInterval(int ms)
{
    if(pDocData->getSession())
        pDocData->getSession()->renderConfig->SetScreenRefreshInterval(ms);
}


void
DocImg::drawSkylightDir(ViewPort &vp)
{
    if(!pDocData->m_session.get() || !pDocData->m_session->renderConfig->scene || !pDocData->getSession()->renderConfig->scene->sunLight)
        return;
    slg::SunLight *sunLight = pDocData->getSession()->renderConfig->scene->sunLight;
    slg::PerspectiveCamera *camera = pDocData->getSession()->renderConfig->scene->camera;

    PrepareDrawEnv  drawEnv(vp,pDocData->getSession());

    float length = ExtraCameraManager::getCurrentRadius(pDocData->getSession(),camera);

    glEnable( GL_LINE_SMOOTH );
    //glLineWidth( 1.0f );

    ///@todo configure sphere color.
    glColor3f(0.5f,0.25f,0.0f);

    glPushMatrix();
    glTranslatef(camera->target.x, camera->target.y, camera->target.z);
    GLUquadricObj* Sphere = gluNewQuadric();
    gluSphere(Sphere,length,10,10);
    gluDeleteQuadric(Sphere);

    //x coordinate
    glBegin( GL_LINES );
    glColor3f( 1.0f, 0.0f, 0.0f );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    glVertex3f( length, 0.0f, 0.0f );
    glEnd();
    //y coordinate
    glBegin( GL_LINES );
    glColor3f( 0.0f, 1.0f, 0.0f );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    glVertex3f( 0.0f, length, 0.0f );
    glEnd();
    //z coordinate
    glBegin( GL_LINES );
    glColor3f( 0.0f, 0.0f, 1.0f );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    glVertex3f( 0.0f, 0.0f, length );
    glEnd();

    glPopMatrix();

    luxrays::Point drawEnd = (camera->target - sunLight->GetDir() * length );

    glBegin( GL_LINES );
    glColor3f( 0.855f, 0.44f, 0.839f );
    glVertex3f( camera->target.x, camera->target.y, camera->target.z );
    glVertex3f( drawEnd.x, drawEnd.y, drawEnd.z );
    glEnd();
}


void
DocImg::drawSelectedObject(ViewPort &vp)
{
    if(!pDocData->m_session.get() || !pDocData->m_session->renderConfig->scene)
        return;
    std::vector<std::string>&   selection = pDocData->getSelection();
    if(selection.size() == 0)
        return;

    PrepareDrawEnv  drawEnv(vp,pDocData->getSession());

    //set selection color.
    ///@todo configure this.
    glColor3f(1.0f,1.0f,1.0f);

    //ensure mode. do not need restore. @fixme: move it to init?
    glEnableClientState(GL_VERTEX_ARRAY);

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
}

double
DocImg::getRenderTime(void)
{
    if(pDocData->getSession())
        return pDocData->getSession()->renderEngine->GetRenderingTime();
    return 0.0f;
}

bool
DocImg::getRenderInfo(int type,RenderInfo &ri)
{
    if(type == RI_NATIVE)
    {
        return pDocData->getNativeRenderInfo(ri);
    }else if(type == RI_CONTRIBUTE)
    {
        return pDocData->cachefilm().getContributeRenderInfo(ri);
    }else
    {
        if(pDocData->getNativeRenderInfo(ri))
        {
            pDocData->cachefilm().getContributeRenderInfo(ri);
            return true;
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

bool
DocImg::getImage(const std::string& filepath,int &w,int &h,const float* &pixels)
{
    bool bDataOk = false;
    if(pDocData->getSession())
    {
        if(pDocData->getSession()->renderConfig->scene->imgMapCache.IsImageMapDefined(filepath))
        {
            try{
                slg::ImageMap *im = pDocData->getSession()->renderConfig->scene->imgMapCache.GetImageMap(filepath,2.2f);
                if(im)
                {
                    w = im->GetWidth();
                    h = im->GetHeight();
                    pixels = im->GetPixels();
                    bDataOk = true;
                }
            }catch(std::exception &e)
            {
                (void)e;
            }
        }
    }
    return bDataOk;
}



} //end namespace gme.
