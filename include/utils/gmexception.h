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

#ifndef  GME_UTILS_GMEXCEPTION_H
#define  GME_UTILS_GMEXCEPTION_H


namespace gme
{

///@brief 安装平台相关的Exception Handler.
struct	exception{
	/** @brief 安装异常翻译。这将把se转化为ex抛出。
	**/
	static	void	installTranslator(void);
	/** @brief 移除异常转化。
	**/
    static	void 	removeTranslator(void);
};


}

#endif //GME_UTILS_GMEXCEPTION_H


