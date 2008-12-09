//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


/// \file icon.hpp
/// Windows icon


#if !defined(__GUI_ICON_HPP__)
#define __GUI_ICON_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <windows.h>

#include <boost/shared_ptr.hpp>
#include <win/exception.hpp>


namespace gui {


inline
boost::shared_ptr<void> 
load_icon(HINSTANCE hInstance, LPCTSTR lpIconName)
{
    boost::shared_ptr<void> h(::LoadIcon(hInstance, IDI_APPLICATION), ::DestroyIcon);
    if (!h)
        throw runtime_error("LoadIcon failed");

    return h;
}


} //namespace gui

#endif //__GUI_ICON_HPP__