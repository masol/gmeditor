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
#include "utils/option.h"
#include "utils/modulepath.h"
#include <boost/program_options.hpp>
#include <boost/unordered_map.hpp>
#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>
#include <fstream>


namespace gme{

static inline void		outCopyRights(void){
	std::cout << __("(C) copyright 2013 SanPolo Co.LTD") << std::endl;
	std::cout << __("This is free software; see the source for copying conditions.  There is NO ") << std::endl;
	std::cout << __("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.") << std::endl;
	std::cout << __("office website: http://www.render001.com/gmeditor") << std::endl;
	std::cout << __("Written by masol.lee<masol.li@gmail.com>. ") << std::endl;
}

static inline void		outUsage(void){
	std::cout << "Usage: gmeditor [options] [scene-file]" << std::endl;
}

static inline void		initOption_descritpion(boost::program_options::options_description &cmdline,boost::program_options::options_description &generic)
{
	cmdline.add_options()
		("help,h", __("Display this information."))
		("version,v", __("Display  version informationl."))
		("config,c", boost::program_options::value<std::string>(),__("Specify config file."))
		("source,s", boost::program_options::value<std::string>(),__("Specify source scene file."))
		("output,o", boost::program_options::value<std::string>(),__("Place the result image into <file>."))
		;
}

struct  parser_context{
    boost::program_options::options_description     &cmdline_options;
    std::vector<std::string>                        sources;
    Option                                          &option;
    std::string                                     outfile;
    parser_context(boost::program_options::options_description &clo,Option &o) : cmdline_options(clo),option(o)
    {
    }
};

static inline bool loadConfigFile(const std::string &cfgFile,parser_context &ctx);
static	bool	parser_Option(boost::program_options::parsed_options &option,parser_context &ctx)
{
	bool bNeedQuit = false;
	std::vector< boost::program_options::option>::iterator	it = option.options.begin();
	std::string	 window_tag("window.");

	std::vector<std::string>	configFile;
	while(it != option.options.end())
	{
		if(it->unregistered)
		{//发现一个未注册的配置选项。
			//TODO: more flexible mechanism to extend config.
			if(it->string_key.compare(0, window_tag.length(), window_tag) == 0)
			{
				std::vector< std::string>::iterator	value_it = it->value.begin();
				while(value_it != it->value.end() )
				{
					ctx.option.put(it->string_key,*value_it);
					value_it++;
				}
			}else{
				outCopyRights();
				std::cout << __("invalid parameter ") << it->string_key << std::endl;
				outUsage();
				std::cout << ctx.cmdline_options << std::endl;
				bNeedQuit = true;
				break;
			}
		}else if(it->string_key == "config")
		{
			if(it->value.size() == 0)
			{
				outCopyRights();
				std::cout << __("config need a config file as parameter") << std::endl;
				outUsage();
				bNeedQuit = true;
				break;
			}
			configFile = it->value;
			std::vector<std::string>::iterator cfgit = configFile.begin();
			bool	bcfgFileOK = true;
			while(cfgit != configFile.end())
			{
				if(!boost::filesystem::exists(*cfgit))
				{
					outCopyRights();
					std::cout << __("specified config file ") << *cfgit << __(" doesn't exist") << std::endl;
					bcfgFileOK = false;
					bNeedQuit = true;
					break;
				}
				cfgit++;
			}
			if(!bcfgFileOK)
			{
				break;
			}
		}else if(it->string_key == "help")
		{
			outCopyRights();
			outUsage();
			std::cout << ctx.cmdline_options << std::endl;
			bNeedQuit = true;
			break;
		}else if(it->string_key == "version")
		{
			outCopyRights();
			std::cout << __("gmeditor Version ")<< GME_VERSION_MAJOR << '.' << GME_VERSION_MINOR << std::endl;
			bNeedQuit = true;
			break;
		}else if(it->string_key == "source")
		{
			std::vector<std::string>::iterator value_it = it->value.begin();
			while(value_it != it->value.end())
			{
				if(std::find(ctx.sources.begin(),ctx.sources.end(),*value_it) == ctx.sources.end())
				{
					ctx.sources.push_back(*value_it);
				}
				value_it++;
			}
		}else if(it->string_key == "output")
		{
		    if(it->value.size() > 0)
                ctx.outfile = *(it->value.begin());
		}
		it++;
	}

	//开始分析和保存配置文件的内容。
	if(!bNeedQuit && configFile.size())
	{
		std::vector<std::string>::iterator cfgit = configFile.begin();
		while(cfgit != configFile.end())
		{
			if(loadConfigFile(*cfgit,ctx))
			{
				bNeedQuit = true;
				break;
			}
			cfgit++;
		}
	}

	return bNeedQuit;
}

static inline bool loadConfigFile(const std::string &cfgFile,parser_context &ctx)
{
	std::ifstream	input(cfgFile.c_str(),std::ios_base::in);
	BOOST_SCOPE_EXIT( (&input))
	{
		input.close();
	}BOOST_SCOPE_EXIT_END

	bool	bNeedQuit = false;
	if(input.is_open())
	{
		std::cout << __("loading config file ") << cfgFile << " ...";
		boost::program_options::parsed_options po = boost::program_options::parse_config_file(input,ctx.cmdline_options,true);
		bNeedQuit = parser_Option(po,ctx);
		std::cout << __("done!") << std::endl;
	}else{
		outCopyRights();
		std::cout << __("can not open config file ") << cfgFile << std::endl;
		outUsage();
		bNeedQuit = true;
	}
	return bNeedQuit;
}

bool
Option::initFromArgs(int argc,const wchar_t* argv[])
{
    return false;
}


bool
Option::initFromArgs(int argc,const char* argv[])
{
	boost::program_options::options_description		cmdline;
	boost::program_options::options_description		generic;

	initOption_descritpion(cmdline,generic);

	bool	bNeedQuit = false;
	boost::program_options::options_description cmdline_options;
	cmdline_options.add(cmdline).add(generic);

	boost::program_options::positional_options_description p;
	p.add("source", -1);
	boost::program_options::parsed_options po = boost::program_options::command_line_parser(argc,argv).options(cmdline_options).positional(p).allow_unregistered().run();

	parser_context      ctx(cmdline_options,*this);
	bNeedQuit = parser_Option(po,ctx);
	if(!bNeedQuit)
	{//system runing, set the source and config log system.
		if(ctx.sources.size())
		{
			this->put("document.source",ctx.sources);
		}
        if(ctx.outfile.length())
        {
            this->put("document.output",ctx.outfile);
        }
		gme::ModulePath::instance().initialize(argv[0]);
	}

	return !bNeedQuit;
}

}//gme

