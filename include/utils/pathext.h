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

#ifndef GME_UTILS_PATHEXT_H
#define GME_UTILS_PATHEXT_H

//#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
/**
 * @file        pathext.h
 * @brief       扩展了boost::path，增加了link友好的canonicalize方法。
 **/

namespace boost{
namespace filesystem{
namespace gme_ext{

	/** @brief 对p执行canonicalize操作，不同于boost::path中的方法，本方法正确处理了符号链接。
	 */
	boost::filesystem::path canonicalize(const boost::filesystem::path& p);

	/** @brief 解析路径p，并将其中含有符号链接的部分替换为实际路径。同时对结果执行canonicalize操作。
	 */
	boost::filesystem::path resolve_sym(const boost::filesystem::path& p);

	/** @brief 读取符号链接result所对应的真实路径。
	 *   @param result result保存了请求转化的符号链接，必须其是一个符号链接。
	 */
	bool read_symlink(boost::filesystem::path &result);

	/** @brief 获取指定路径的后缀。
	**/
	std::string   get_extension(const std::string &path);

}//gme_ext
}//boost
}//filesystem


#endif //GME_UTILS_PATHEXT_H

