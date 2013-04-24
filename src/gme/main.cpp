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
#include "mainframe.h"
#include "utils/option.h"
#include <boost/scope_exit.hpp>
#include "dm/docio.h"

class MyApp : public wxApp
{
protected:
    gme::MainFrame      *m_mainFrame;
public:
    MyApp() : m_mainFrame(NULL)
    {
    }
    ~MyApp()
    {
    }
    bool OnInit() 
    {
        //initionlize config.
        ///@FIXME: 根据valgrind报告，如果使用wx提供的argv自动转化，其数据在进程销毁时才会清理。因此下文自行处理utf8转化。
        {
            typedef const char*   str_point;
            str_point*   mb_args = new str_point[argc];
            for(int i = 0; i < argc;i++)
            {
                wxString string(argv[i]);
                mb_args[i] = strdup(string.mb_str());
            }
	        BOOST_SCOPE_EXIT( (&mb_args) (&argc))
	        {
	            for(int i = 0; i < argc; i++)
	            {
	                free(const_cast<char*>(mb_args[i]));
                }
                delete[] mb_args;
	        }BOOST_SCOPE_EXIT_END
            
        	if(!gme::Option::instance().initFromArgs(argc,mb_args ))
		        return false;
        }
	    std::string source("document.source");
	    if(gme::Option::instance().is_existed(source))
	    {
	        gme::DocIO docio;
		    std::vector<std::string> srcset = gme::Option::instance().get<std::vector<std::string> >(source);
		    std::vector<std::string>::iterator it = srcset.begin();
		    while(it != srcset.end())
		    {
    		    if(docio.loadScene(*it))
    		        break;
		        it++;
            }
        }

        m_mainFrame = new gme::MainFrame(NULL);
        SetTopWindow(m_mainFrame);
        m_mainFrame->Maximize();
        m_mainFrame->Show();
        return true;
    }
};

DECLARE_APP(MyApp)
IMPLEMENT_APP(MyApp)
