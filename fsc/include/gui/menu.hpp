//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


/// \file menu.hpp
/// Windows menu


#if !defined(__GUI_MENU_HPP__)
#define __GUI_MENU_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <windows.h>

#include <boost/shared_ptr.hpp>
#include <win/exception.hpp>


namespace gui {


inline
boost::shared_ptr<void> 
load_menu(HINSTANCE hInstance, LPCTSTR lpMenuName)
{
    boost::shared_ptr<void> h(::LoadMenu(hInstance, lpMenuName), ::DestroyMenu);
    if (!h)
        throw runtime_error("LoadMenu failed");

    return h;
}


} //namespace gui

#endif //__GUI_MENU_HPP__