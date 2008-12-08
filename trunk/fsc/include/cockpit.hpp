//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


/// \file cockpit.hpp
/// cockpit definition


#if !defined(__COCKPIT_HPP__)
#define __COCKPIT_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <fsx/sim_connect.hpp>


class cockpit
{
public:
    enum GROUP_ID {
        GROUP0,
    };

    enum EVENT_ID {
        EVENT_BRAKES,
        EVENT_STROBES_TOGGLE,
        EVENT_AUTOPILOT_HEADING_BUG_SET,
        EVENT_THROTTLE_SET,
        EVENT_MIXTURE_SET,
        EVENT_DME_SELECT,
        EVENT_MY_EVENT
    };

    enum DATA_REQUEST_ID { 
        REQUEST_THROTTLE,
        REQUEST_FLAPS
    };

    enum DATA_DEFINE_ID { 
        DEFINITION_THROTTLE,
        DEFINITION_FLAPS
    };

    struct flaps
    {
        int idx;
        float percent;
        float left_angle;
        int right_angle;
    };

    cockpit(const std::string& _path);
    ~cockpit();

    void initialize(fsx::sim_connect& _sc);

    void on_rcv_sim_data(SIMCONNECT_RECV_SIMOBJECT_DATA& _data);
    void on_rcv_sim_event(SIMCONNECT_RECV_EVENT& _evt);

    void map_sim_event_to_output();
    void map_input_to_sim_event();

private:
    std::string name_;
};


#endif //__COCKPIT_HPP__