//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


#include "precompiled_header.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <log.hpp>
#include <cockpit.hpp>


cockpit::cockpit(const std::string& _path)
{
	boost::property_tree::ptree pt;

	read_xml(_path, pt);
    name_ = pt.get<std::string>("cockpit.name");
	//min_dt_out_ = pt.get<float64>("project.sim.min_dt_out");

	//data_ = pt.get<std::string>("project.data");
	//
	//dem_path_ = pt.get<std::string>("project.in.dem_path");
	//slopes_path_ = pt.get<std::string>("project.in.slopes_path");
	//contrib_area_path_ = pt.get<std::string>("project.in.contrib_area_path");		
	//topo_index_path_ = pt.get<std::string>("project.in.topo_index_path");
	//watershed_path_ = pt.get<std::string>("project.in.watershed_path");
	//channel_path_ = pt.get<std::string>("project.in.channel_path");

	//simplify_tolerance_ = pt.get<float64>("project.pre.simplify_tolerance");

	//min_angle_ = pt.get<float64>("project.pre.min_angle");
	//min_face_area_ = pt.get<float64>("project.pre.min_face_area");
	//max_face_area_ = pt.get<float64>("project.pre.max_face_area");
}


cockpit::~cockpit()
{
}


void cockpit::initialize(fsx::sim_connect& _sc)
{
    HRESULT hr;

    //Map client event id to sim event
    hr = _sc.map_client_event_to_sim_event(EVENT_BRAKES, "BRAKES");
    hr = _sc.map_client_event_to_sim_event(EVENT_STROBES_TOGGLE, "STROBES_TOGGLE");
    hr = _sc.map_client_event_to_sim_event(EVENT_AUTOPILOT_HEADING_BUG_SET, "HEADING_BUG_SET");
    hr = _sc.map_client_event_to_sim_event(EVENT_DME_SELECT, "DME_SELECT");
    hr = _sc.map_client_event_to_sim_event(EVENT_THROTTLE_SET, "THROTTLE_SET");
    hr = _sc.map_client_event_to_sim_event(EVENT_MIXTURE_SET, "MIXTURE_SET");

    //Add client event to notification group
    hr = _sc.add_client_event_to_notification_group(GROUP0, EVENT_BRAKES);
    hr = _sc.add_client_event_to_notification_group(GROUP0, EVENT_STROBES_TOGGLE);
    hr = _sc.add_client_event_to_notification_group(GROUP0, EVENT_AUTOPILOT_HEADING_BUG_SET);
    hr = _sc.add_client_event_to_notification_group(GROUP0, EVENT_DME_SELECT);
    hr = _sc.add_client_event_to_notification_group(GROUP0, EVENT_THROTTLE_SET);
    hr = _sc.add_client_event_to_notification_group(GROUP0, EVENT_MIXTURE_SET);

    //Set group priority
    hr = _sc.set_notification_group_priority(GROUP0, SIMCONNECT_GROUP_PRIORITY_HIGHEST);

    //Read simulation data
    hr = _sc.add_to_data_definition(DEFINITION_THROTTLE, "GENERAL ENG THROTTLE LEVER POSITION:1", "percent", SIMCONNECT_DATATYPE_FLOAT32);
    hr = _sc.add_to_data_definition(DEFINITION_FLAPS, "FLAPS HANDLE INDEX", NULL, SIMCONNECT_DATATYPE_INT32);
    hr = _sc.add_to_data_definition(DEFINITION_FLAPS, "FLAPS HANDLE PERCENT", NULL, SIMCONNECT_DATATYPE_FLOAT32);
    hr = _sc.add_to_data_definition(DEFINITION_FLAPS, "TRAILING EDGE FLAPS LEFT ANGLE", "radians", SIMCONNECT_DATATYPE_FLOAT32);
    hr = _sc.add_to_data_definition(DEFINITION_FLAPS, "TRAILING EDGE FLAPS RIGHT ANGLE", NULL, SIMCONNECT_DATATYPE_INT32);        
    
    hr = _sc.request_data_on_sim_object(REQUEST_THROTTLE, DEFINITION_THROTTLE, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
    hr = _sc.request_data_on_sim_object(REQUEST_FLAPS, DEFINITION_FLAPS, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_ONCE);
}


void cockpit::on_rcv_sim_data(SIMCONNECT_RECV_SIMOBJECT_DATA& _data)
{
    switch(_data.dwRequestID)
    {
        case REQUEST_THROTTLE:
        {
            float& throttle = (float&)_data.dwData;
            LDBG_ << "Throttle : " << throttle;
            break;
        }

        case REQUEST_FLAPS:
        {
            flaps& f = (flaps&)_data.dwData;
            LDBG_ << "Flaps index : " << f.idx;
            LDBG_ << "Flaps percent : " << f.percent;
            break;
        }
    }
}


void cockpit::on_rcv_sim_event(SIMCONNECT_RECV_EVENT& _evt)
{
    switch(_evt.uEventID)
    {
        case EVENT_BRAKES:
            LDBG_ << "Event brakes: " << _evt.dwData;
            break;

        case EVENT_STROBES_TOGGLE:
            LDBG_ << "Event strobes toggle: " << _evt.dwData;
            break;

        case EVENT_AUTOPILOT_HEADING_BUG_SET:
            LDBG_ << "Event autopilot heading bug: " << _evt.dwData;
            break;

        case EVENT_THROTTLE_SET:
            LDBG_ << "Event throttle: " << _evt.dwData / 16383.0;
            break;

        case EVENT_MIXTURE_SET:
            LDBG_ << "Event mixture: " << _evt.dwData / 16383.0;
            break;

        case EVENT_DME_SELECT:
            LDBG_ << "Event DME select: " << _evt.dwData;
            break;

        case EVENT_MY_EVENT:
            LDBG_ << "Event My event: " << _evt.dwData;
            break;

        default:
            break;
    }
}