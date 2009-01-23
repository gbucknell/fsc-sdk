//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


#include "precompiled_header.h"
#include <gui/init.hpp>
#include <gui/main_frame.hpp>


using namespace gui;


void gui::init(HINSTANCE _hInstance)
{
    HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ATLASSERT(SUCCEEDED(hRes));

    // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
    ::DefWindowProc(NULL, 0, 0, 0L);

    AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

    hRes = _Module.Init(NULL, _hInstance);
    ATLASSERT(SUCCEEDED(hRes));

    AtlAxWinInit();
}


void gui::run()
{
    //Create and register message loop
    WTL::CMessageLoop loop;
	_Module.AddMessageLoop(&loop);
    
    //Create main frame
    gui::main_frame wnd;
	if (wnd.CreateEx() == NULL)
    	LERR_ << "Main window creation failed!";

	wnd.ShowWindow(SW_HIDE);

    int nRet = loop.Run();
}


void gui::uninit()
{
	_Module.RemoveMessageLoop();

    _Module.Term();
    ::CoUninitialize();
}