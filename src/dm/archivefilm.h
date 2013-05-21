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

#ifndef  GME_DM_ARCHIVEFILM_H
#define  GME_DM_ARCHIVEFILM_H

#include "slg/slg.h"
#include "slg/film/tonemapping.h"
#include "slg/film/film.h"
#include <boost/cstdint.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_free.hpp>
#include "dm/doc.h"


namespace boost { namespace serialization {

    template<class Archive>
    void save(Archive & ar, const slg::Film &film,const unsigned int version)
	{
		uint_least32_t	w = film.GetWidth(),h = film.GetHeight();
		ar << w << h;
        uint_least32_t  hasPerPixelNormalizedBuffer = (film.HasPerPixelNormalizedBuffer() ? 1 : 0),
			hasPerScreenNormalizedBuffer =  (film.HasPerScreenNormalizedBuffer() ? 1 : 0),
			hasAlphaChannel = (film.IsAlphaChannelEnabled() ? 1 : 0);
		ar << hasPerPixelNormalizedBuffer << hasPerScreenNormalizedBuffer << hasAlphaChannel;
		if(film.HasPerPixelNormalizedBuffer())
		{
			for(u_int x = 0; x < w; x++)
			{
				for(u_int y = 0; y < h; y++)
				{
					const slg::SamplePixel *sp = film.GetSamplePixel(slg::PER_PIXEL_NORMALIZED,x,y);
					ar << sp->radiance.r << sp->radiance.g << sp->radiance.b << sp->weight;
				}
			}
		}
		if(film.HasPerScreenNormalizedBuffer())
		{
			for(u_int x = 0; x < w; x++)
			{
				for(u_int y = 0; y < h; y++)
				{
					const slg::SamplePixel *sp = film.GetSamplePixel(slg::PER_SCREEN_NORMALIZED,x,y);
					ar << sp->radiance.r << sp->radiance.g << sp->radiance.b << sp->weight;
				}
			}
		}
		if(film.IsAlphaChannelEnabled())
		{
			for(u_int x = 0; x < w; x++)
			{
				for(u_int y = 0; y < h; y++)
				{
					const slg::AlphaPixel *ap = film.GetAlphaPixel(x,y);
					ar << ap->alpha;
				}
			}
		}
	}
	template<class Archive>
    void load(Archive & ar, slg::Film &film, const unsigned int version)
    {
		uint_least32_t	w,h;
		ar >> w >> h;
		if(w != film.GetWidth() || h != film.GetHeight() )
			throw std::runtime_error("invalid format");
		uint_least32_t  hasPerPixelNormalizedBuffer,hasPerScreenNormalizedBuffer,hasAlphaChannel;
		ar >> hasPerPixelNormalizedBuffer >> hasPerScreenNormalizedBuffer >> hasAlphaChannel;
		if( (hasPerPixelNormalizedBuffer  != (film.HasPerPixelNormalizedBuffer() ? 1 : 0)) ||
			(hasPerScreenNormalizedBuffer != (film.HasPerScreenNormalizedBuffer() ? 1 : 0)) ||
			(hasAlphaChannel != (film.IsAlphaChannelEnabled() ? 1 : 0))  )
		{
			throw std::runtime_error("invalid format");
		}
		if(hasPerPixelNormalizedBuffer == 1)
		{
			for(u_int x = 0; x < w; x++)
			{
				for(u_int y = 0; y < h; y++)
				{
					luxrays::Spectrum	radiance;
					float 	weight;
					ar >> radiance.r >> radiance.g >> radiance.b >> weight;
					film.AddRadiance(slg::PER_PIXEL_NORMALIZED,x,y,radiance / weight,weight);
				}
			}
		}
		if(hasPerScreenNormalizedBuffer == 1)
		{
			for(u_int x = 0; x < w; x++)
			{
				for(u_int y = 0; y < h; y++)
				{
					luxrays::Spectrum	radiance;
					float 	weight;
					ar >> radiance.r >> radiance.g >> radiance.b >> weight;
					film.AddRadiance(slg::PER_SCREEN_NORMALIZED,x,y,radiance / weight,weight);
				}
			}
		}
		if(hasAlphaChannel == 1)
		{
			for(u_int x = 0; x < w; x++)
			{
				for(u_int y = 0; y < h; y++)
				{
					float alpha;
					ar >> alpha;
					film.SetAlpha(x,y,alpha);
				}
			}
		}
    }

    template<class Archive>
    void load(Archive & ar, gme::RenderInfo & ri, const unsigned int version)
    {
        uint_least32_t	pass;
        ar >> pass >> ri.convergence >> ri.elapsedTime >> ri.totalRaysSec >> ri.totalSamplesSec;
        ri.pass = pass;
    }

    template<class Archive>
    void save(Archive & ar, gme::RenderInfo & ri, const unsigned int version)
    {
        uint_least32_t	pass = ri.pass;
        ar << pass << ri.convergence << ri.elapsedTime << ri.totalRaysSec << ri.totalSamplesSec;
    }

}
}

BOOST_SERIALIZATION_SPLIT_FREE(slg::Film)

#endif //GME_DM_ARCHIVEFILM_H
