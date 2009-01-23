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


#if !defined(__WIN_MUTEX_HPP__)
#define __WIN_MUTEX_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <windows.h>

#include <boost/shared_ptr.hpp>
#include <win/exception.hpp>


namespace win {


typedef boost::shared_ptr<void> mutex_t;


inline
mutex_t
create_mutex(BOOL _bInitialOwner = FALSE, LPCTSTR _szName = NULL)
{
    mutex_t h(::CreateMutex(NULL, _bInitialOwner, _szName), ::CloseHandle);
    if (!h)
        throw runtime_error("CreateMutex failed");

    return h;
}


} //namespace win

#endif //__WIN_MUTEX_HPP__