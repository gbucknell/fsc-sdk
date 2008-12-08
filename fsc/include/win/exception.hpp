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


#if !defined(__WIN_EXCEPTION_HPP__)
#define __WIN_EXCEPTION_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <windows.h>
#include <stdexcept>


namespace win {


class runtime_error : public std::exception
{
public:
    explicit runtime_error(const std::string& _msg, DWORD _last_error = ::GetLastError()) : msg_(_msg), last_error_(_last_error) {}
	virtual ~runtime_error() {}
	virtual const char *what() const {return (msg_.c_str());}
    DWORD last_error() const {return (last_error_);}

private:
    std::string msg_;
    DWORD last_error_;
};


} //namespace win

#endif //__WIN_EXCEPTION_HPP__