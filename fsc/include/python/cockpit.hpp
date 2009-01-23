//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


#if !defined(__PYTHON_COCKPIT_HPP__)
#define __PYTHON_COCKPIT_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class basic_cockpit;


namespace python {


//Construct a cockpit object
basic_cockpit& make_cockpit(
    std::string _cockpit_file,
    std::string _config_path,
    fsx::sim_connect& _sim,
    iocards::usb_expansion_card& _hw);


} //namespace python


#endif //__PYTHON_COCKPIT_HPP__