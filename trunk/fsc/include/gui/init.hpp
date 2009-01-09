//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


/// \file init.hpp
/// Initialize WTL


#if !defined(__GUI_INIT_HPP__)
#define __GUI_INIT_HPP__


// ATL headers are included in the precompiled header


namespace gui {


void init_wtl(HINSTANCE _hInstance)
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


void uninit_wtl()
{
    _Module.Term();
    ::CoUninitialize();
}

} //namespace gui

#endif //__GUI_INIT_HPP__