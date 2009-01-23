//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


/// \file exception.hpp
/// Windows exceptions


#if !defined(__SINGLE_INSTANCE_HPP__)
#define __SINGLE_INSTANCE_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <win/mutex.hpp>


namespace win {


class single_instance
{
public:
    explicit single_instance(LPCTSTR _name) : last_error_(0)
    {
        h_ = mutex_t(::CreateMutex(NULL, FALSE, _name), ::CloseHandle);
        last_error_ = ::GetLastError();
    }

	bool is_already_running() const {return (ERROR_ALREADY_EXISTS == last_error_);}

private:
    mutex_t h_;
    DWORD last_error_;
};


} //namespace win

#endif //__SINGLE_INSTANCE_HPP__