//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


/// \file shared_handle.hpp
/// Windows shared handle


#if !defined(__WIN_SHARED_HANDLE_HPP__)
#define __WIN_SHARED_HANDLE_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <windows.h>
#include <boost/shared_ptr.hpp>


namespace win {


/// Reference counted HANDLE.
///
/// The handle is closed when not referenced anymore.
struct shared_handle : public boost::shared_ptr<void>
{
    shared_handle(HANDLE _h) : boost::shared_ptr<void>(_h, ::CloseHandle) {}
    operator HANDLE() const {return this->get();}
};


} //namespace win

#endif //__WIN_SHARED_HANDLE_HPP__