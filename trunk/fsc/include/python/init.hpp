//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


#if !defined(__PYTHON_INIT_HPP__)
#define __PYTHON_INIT_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class basic_cockpit;


namespace python {

/// Initialize the python interpreter
void init();

/// Get the last error message
std::string get_error();


} //namespace python


#endif //__PYTHON_INIT_HPP__