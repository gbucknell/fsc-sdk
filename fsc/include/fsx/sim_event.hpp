//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


/// \file sim_event.hpp
/// Events of the Sim Connect API


#if !defined(__FSX_SIM_EVENT_HPP__)
#define __FSX_SIM_EVENT_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <cassert>
#include <windows.h>
#include <SimConnect.h>


namespace fsx {


/// Sim connect event.
class sim_event
{
public:
    sim_event(SIMCONNECT_CLIENT_EVENT_ID _id, const DWORD& _data) : id_(_id), data_(_data) {}

private:
    SIMCONNECT_CLIENT_EVENT_ID id_;
    DWORD data_;
};


//template <class T>
//struct sim_data_traits
//{
//    int 
//};


} //namespace fsx


#endif //__FSX_SIM_EVENT_HPP__