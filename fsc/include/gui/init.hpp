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


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// ATL headers are included in the precompiled header
#include <log.hpp>


namespace gui {


/// (Co) initialize ATL and common controls, create the message lood and run the main frame.
void init_wtl(HINSTANCE _hInstance);

/// End the message loop and unitialize ATL
void uninit_wtl();


} //namespace gui

#endif //__GUI_INIT_HPP__