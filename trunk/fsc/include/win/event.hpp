//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


/// \file event.hpp
/// Windows event


#if !defined(__WIN_EVENT_HPP__)
#define __WIN_EVENT_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <windows.h>

#include <boost/shared_ptr.hpp>
#include <win/exception.hpp>


namespace win {


typedef boost::shared_ptr<void> event_t;


inline
event_t create_event(BOOL _bManualReset = FALSE)
{
    event_t h(::CreateEvent(NULL, _bManualReset, FALSE, NULL), ::CloseHandle);
    if (!h)
        throw runtime_error("CreateEvent failed");

    return h;
}

inline
void set(event_t _h)
{
    if (::SetEvent(_h.get()))
        throw runtime_error("SetEvent failed");
}


} //namespace win

#endif //__WIN_EVENT_HPP__