//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


/// \file sim_connect.hpp
/// Sim Connect c++ API


#if !defined(__FSX_SIM_CONNECT_HPP__)
#define __FSX_SIM_CONNECT_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <cassert>
#include <windows.h>
#include <SimConnect.h>

#include <boost/shared_ptr.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/utility.hpp>

#include <assert.hpp>


// Call a function and record its description (to help in error tracking)
#if ! defined(NDEBUG)
    #define SIM_CONNECT_CALL(x) \
        x;                      \
        add_record(#x)
#else
    #define SIM_CONNECT_CALL(x)
#endif

namespace fsx {

namespace detail {

struct record
{
    record(DWORD _id, std::string _descr) : id_(_id), descr_(_descr) {}

    DWORD id_;
    std::string descr_;
};

} //namespace detail


/// Sim connect object. Encapuslate the SimConnect handle.
class sim_connect //: public boost::noncopyable
{
    typedef boost::shared_ptr<void> handle_t;

public:
    /// Used to send a request to the Flight Simulator server to open up communications with a new client.
    HRESULT open(LPCSTR _szName, HWND _hWnd, DWORD _UserEventWin32, HANDLE _hEventHandle, DWORD _dwConfigIndex)
    {
        HANDLE hsc;
        HRESULT hr = SimConnect_Open(&hsc, _szName, _hWnd, _UserEventWin32, _hEventHandle, _dwConfigIndex);
        if (hr == S_OK)
            hsc_ =  handle_t(hsc, SimConnect_Close);

        return hr;
    }

    /// Used to request that the communication with the server is ended.
    void close() {hsc_.reset(); }

    /// Return true is the sim_conncet object is currently connected, false otherwise.
    bool is_open() {return hsc_; }

    /// \name Dispatch
    //@{
    /// Used to process the next SimConnect message received through the specified callback function.
    HRESULT call_dispatch(DispatchProc _pfcnDispatch, void * _pContext = NULL)
    {
        return SimConnect_CallDispatch(handle(), _pfcnDispatch, _pContext);
    }

    /// Used to process the next SimConnect message received, without the use of a callback function.
    HRESULT get_next_dispatch(SIMCONNECT_RECV** _ppData, DWORD* _pcbData)
    {
        return SimConnect_GetNextDispatch(handle(), _ppData, _pcbData);
    }
    //@}

    /// \name Notification groups
    //@{
    /// Used to add an individual client defined event to a notification group.
    HRESULT add_client_event_to_notification_group(SIMCONNECT_NOTIFICATION_GROUP_ID _GroupID, SIMCONNECT_CLIENT_EVENT_ID _EventID, BOOL _bMaskable = FALSE)
    {
        return SIM_CONNECT_CALL(SimConnect_AddClientEventToNotificationGroup(handle(), _GroupID, _EventID, _bMaskable));
    }

    /// Used to set the priority of a notification group.
    HRESULT set_notification_group_priority(SIMCONNECT_NOTIFICATION_GROUP_ID _GroupID, DWORD _dwPriority)
    {
        return SIM_CONNECT_CALL(SimConnect_SetNotificationGroupPriority(handle(), _GroupID, _dwPriority));
    }

    /// Used to request events are transmitted from a notification group, when the simulation is in Dialog Mode. 
    HRESULT request_notification_group(SIMCONNECT_NOTIFICATION_GROUP_ID _GroupID, DWORD _dwReserved = 0, DWORD _dwFlags = 0)
    {
        return SIM_CONNECT_CALL(SimConnect_RequestNotificationGroup(handle(), _GroupID, _dwReserved, _dwFlags));
    }    
    //@}

    /// \name Client data
    //@{
    /// Used to add an offset and a size in bytes, or a type, to a client data definition.
    HRESULT add_to_client_data_defintinion(SIMCONNECT_CLIENT_DATA_DEFINITION_ID _DefineID, DWORD _dwOffset, DWORD _dwSizeOrType, float _fEpsilon = 0.0f, DWORD _dwDatumID = SIMCONNECT_UNUSED)
    {
        return SIM_CONNECT_CALL(SimConnect_AddToClientDataDefinition(handle(), _DefineID, _dwOffset, _dwSizeOrType, _fEpsilon, _dwDatumID));
    }

    /// Used to clear the definition of the specified client data.
    HRESULT clear_client_data_definition(SIMCONNECT_CLIENT_DATA_DEFINITION_ID _DefineID)
    {
        return SIM_CONNECT_CALL(SimConnect_ClearClientDataDefinition(handle(), _DefineID));
    }

    /// Used to associate an ID with a named client date area.
    HRESULT map_client_data_name_to_id(const char* _szClientDataName, SIMCONNECT_CLIENT_DATA_ID _ClientDataID)
    {
        return SIM_CONNECT_CALL(SimConnect_MapClientDataNameToID(handle(), _szClientDataName, _ClientDataID));
    }

    /// Used to request that the data in an area created by another client be sent to this client.
    HRESULT request_client_data(SIMCONNECT_CLIENT_DATA_ID _ClientDataID, SIMCONNECT_DATA_REQUEST_ID _RequestID, SIMCONNECT_CLIENT_DATA_DEFINITION_ID _DefineID, SIMCONNECT_CLIENT_DATA_PERIOD _Period = SIMCONNECT_CLIENT_DATA_PERIOD_ONCE, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG _Flags = 0, DWORD _dwOrigin = 0, DWORD _dwInterval = 0, DWORD _dwLimit = 0)
    {
        return SIM_CONNECT_CALL(SimConnect_RequestClientData(handle(), _ClientDataID, _RequestID, _DefineID, _Period, _Flags, _dwOrigin, _dwInterval, _dwLimit));
    }    
    //@}

    /// \name Client data
    //@{
    /// Used to add a Flight Simulator simulation variable name to a client defined object definition.
    HRESULT add_to_data_definition(SIMCONNECT_DATA_DEFINITION_ID _DefineID, const char* _szDatumName, const char* _szUnitsName, SIMCONNECT_DATATYPE _DatumType = SIMCONNECT_DATATYPE_FLOAT64, float _fEpsilon = 0, DWORD _dwDatumID = SIMCONNECT_UNUSED)
    {
        return SIM_CONNECT_CALL(SimConnect_AddToDataDefinition(handle(), _DefineID, _szDatumName, _szUnitsName, _DatumType, _fEpsilon, _dwDatumID));
    }

    /// Used to remove all simulation variables from a client defined object.
    HRESULT clear_data_definition(SIMCONNECT_DATA_DEFINITION_ID _DefineID) 
    {
        return SIM_CONNECT_CALL(SimConnect_ClearDataDefinition(handle(), _DefineID));
    }

    /// Used to write one or more units of data to a client data area.
    HRESULT set_client_data(SIMCONNECT_CLIENT_DATA_ID _ClientDataID, SIMCONNECT_CLIENT_DATA_DEFINITION_ID _DefineID, DWORD _dwFlags, DWORD _dwReserved, DWORD _cbUnitSize, void* _pDataSet)
    {
        return SIM_CONNECT_CALL(SimConnect_SetClientData(handle(), _ClientDataID, _DefineID, _dwFlags, _dwReserved, _cbUnitSize, _pDataSet));
    }
    //@}

    /// \name Sim objects
    //@{
    /// Used to make changes to the data properties of an object.
    HRESULT set_data_on_sim_object(SIMCONNECT_DATA_DEFINITION_ID _DefineID, SIMCONNECT_OBJECT_ID _ObjectID, SIMCONNECT_DATA_SET_FLAG _Flags, DWORD _dwArrayCount, DWORD _cbUnitSize, void* _pDataSet)
    {
        return SIM_CONNECT_CALL(SimConnect_SetDataOnSimObject(handle(), _DefineID, _ObjectID, _Flags, _dwArrayCount, _cbUnitSize, _pDataSet));
    }
    
    /// Used to request when the SimConnect client is to receive data values for a specific object.
    HRESULT request_data_on_sim_object(SIMCONNECT_DATA_REQUEST_ID _RequestID, SIMCONNECT_DATA_DEFINITION_ID _DefineID, SIMCONNECT_OBJECT_ID _ObjectID, SIMCONNECT_PERIOD _Period, SIMCONNECT_DATA_REQUEST_FLAG _Flags = 0, DWORD _dwOrigin = 0, DWORD _dwInterval = 0, DWORD _dwLimit = 0)
    {
        return SIM_CONNECT_CALL(SimConnect_RequestDataOnSimObject(handle(), _RequestID, _DefineID, _ObjectID, _Period, _Flags, _dwOrigin, _dwInterval, _dwLimit));
    }

    /// Used to retrieve information about simulation objects of a given type that are within a specified radius of the user's aircraft.
    HRESULT request_data_on_sim_object_type(SIMCONNECT_DATA_REQUEST_ID _RequestID, SIMCONNECT_DATA_DEFINITION_ID _DefineID, DWORD _dwRadiusMeters, SIMCONNECT_SIMOBJECT_TYPE _Type)
    {
        return SIM_CONNECT_CALL(SimConnect_RequestDataOnSimObjectType(handle(), _RequestID, _DefineID, _dwRadiusMeters, _Type));
    }
    //@}

    /// \name Client Event
    //@{
    /// Used to associate a client defined event ID with a Flight Simulator event name.
    HRESULT map_client_event_to_sim_event(SIMCONNECT_CLIENT_EVENT_ID _EventID, const char* _szEventName)
    {
        return SIM_CONNECT_CALL(SimConnect_MapClientEventToSimEvent(handle(), _EventID, _szEventName));
    }

    /// Used to request that the Flight Simulator server transmit to all SimConnect clients the specified client event.
    HRESULT transmit_client_event(SIMCONNECT_OBJECT_ID _ObjectID, SIMCONNECT_CLIENT_EVENT_ID _EventID, DWORD _dwData, SIMCONNECT_NOTIFICATION_GROUP_ID _GroupID, SIMCONNECT_EVENT_FLAG _Flags)
    {
        return SIM_CONNECT_CALL(SimConnect_TransmitClientEvent(handle(), _ObjectID, _EventID, _dwData, _GroupID, _Flags));
    }

    /// Used to remove a client defined event from a notification group.
    HRESULT remove_client_event(SIMCONNECT_NOTIFICATION_GROUP_ID _GroupID, SIMCONNECT_CLIENT_EVENT_ID _EventID)
    {
        return SIM_CONNECT_CALL(SimConnect_RemoveClientEvent(handle(), _GroupID, _EventID));
    }   

    //@}

    /// \name System event
    //@{
    /// Used to request that a specific system event is notified to the client.
    HRESULT subscribe_to_system_event(SIMCONNECT_CLIENT_EVENT_ID _EventID, const char* _szSystemEventName)
    {
        return SIM_CONNECT_CALL(SimConnect_SubscribeToSystemEvent(handle(), _EventID, _szSystemEventName));
    }

    /// Used to request that notifications are no longer received for the specified system event.
    HRESULT unsubscribe_from_system_event(SIMCONNECT_CLIENT_EVENT_ID _EventID)
    {
        return SIM_CONNECT_CALL(SimConnect_UnsubscribeFromSystemEvent(handle(), _EventID));
    }

    /// Used to turn requests for event information from the server on and off.
    HRESULT set_system_event_state(SIMCONNECT_CLIENT_EVENT_ID _EventID, SIMCONNECT_STATE _dwState)
    {
        return SIM_CONNECT_CALL(SimConnect_SetSystemEventState(handle(), _EventID, _dwState));
    }
    
    /// Used to access a number of Flight Simulator system components. 
    HRESULT set_system_state(const char* _szState, DWORD _dwInteger, float _fFloat, char* _szString)
    {
        return SIM_CONNECT_CALL(SimConnect_SetSystemState(handle(), _szState, _dwInteger, _fFloat, _szString));
    }
    //@}

    /// \name Input event
    //@{
    /// Used to connect input events (such as keystrokes, joystick or mouse movements) with the sending of appropriate event notifications.
    HRESULT map_input_event_to_client_event(SIMCONNECT_INPUT_GROUP_ID _GroupID, const char* _szInputDefinition, SIMCONNECT_CLIENT_EVENT_ID _DownEventID, DWORD _dwDownValue = 0, SIMCONNECT_CLIENT_EVENT_ID _UpEventID =(SIMCONNECT_CLIENT_EVENT_ID)SIMCONNECT_UNUSED, DWORD _dwUpValue = 0, BOOL _bMaskable = FALSE)
    {
        return SIM_CONNECT_CALL(SimConnect_MapInputEventToClientEvent(handle(), _GroupID, _szInputDefinition, _DownEventID, _dwDownValue, _UpEventID, _dwUpValue, _bMaskable));
    }

    /// Used to remove an input event from a specified input group object.
    HRESULT remove_input_event(SIMCONNECT_INPUT_GROUP_ID _GroupID, const char* _szInputDefinition)
    {
        return SIM_CONNECT_CALL(SimConnect_RemoveInputEvent(handle(), _GroupID, _szInputDefinition));
    }
    //@}
    
    /// Used to remove all the client defined events from a notification group.
    HRESULT clear_notification_group(SIMCONNECT_NOTIFICATION_GROUP_ID _GroupID)
    {
        return SIM_CONNECT_CALL(SimConnect_ClearNotificationGroup(handle(), _GroupID));
    }

    /// Used to request the creation of a reserved data area for this client. 
    HRESULT create_client_data(SIMCONNECT_CLIENT_DATA_ID _ClientDataID, DWORD _dwSize, SIMCONNECT_CREATE_CLIENT_DATA_FLAG _Flags)
    {
        return SIM_CONNECT_CALL(SimConnect_CreateClientData(handle(), _ClientDataID, _dwSize, _Flags));
    }

    /// \name Flight and flight plan
    //@{
    /// Used to load an existing flight file.
    HRESULT flight_load(const char* _szFileName)
    {
        return SIM_CONNECT_CALL(SimConnect_FlightLoad(handle(), _szFileName));
    }

    /// Used to load an existing flight plan. 
    HRESULT flight_plan_load(const char* _szFileName)
    {
        return SIM_CONNECT_CALL(SimConnect_FlightPlanLoad(handle(), _szFileName));
    }

    /// Used to save the current state of a flight to a flight file. 
    HRESULT flight_save(const char* _szFileName, const char* _szTitle, const char* _szDescription, DWORD _dwFlags)
    {
        return SIM_CONNECT_CALL(SimConnect_FlightSave(handle(), _szFileName, _szTitle, _szDescription, _dwFlags));
    }
    //@}

    /// Used to request a specific keyboard TAB-key combination applies only to this client.
    HRESULT request_reserved_key(SIMCONNECT_CLIENT_EVENT_ID _EventID, const char* _szKeyChoice1, const char* _szKeyChoice2 = "", const char* _szKeyChoice3 = "")
    {
        return SimConnect_RequestReservedKey(handle(), _EventID, _szKeyChoice1, _szKeyChoice2, _szKeyChoice3);
    }

    /// Used to request information from a number of Flight Simulator system components.
    HRESULT request_system_state(SIMCONNECT_DATA_REQUEST_ID _RequestID, const char* _szState)
    {
        return SimConnect_RequestSystemState(handle(), _RequestID, _szState);
    }

    /// \name Input group
    //@{
    /// Used to set the priority for a specified input group object.
    HRESULT set_input_group_priority(SIMCONNECT_INPUT_GROUP_ID _GroupID, DWORD _dwPriority)
    {
        return SimConnect_SetInputGroupPriority(handle(), _GroupID, _dwPriority);
    }

    /// Used to turn requests for input event information from the server on and off.
    HRESULT set_input_group_state(SIMCONNECT_INPUT_GROUP_ID _GroupID, DWORD _dwState)
    {
        return SimConnect_SetInputGroupState(handle(), _GroupID, _dwState);
    }

    /// Used to remove all the input events from a specified input group object.
    HRESULT clear_input_group(SIMCONNECT_INPUT_GROUP_ID _GroupID)
    {
        return SIM_CONNECT_CALL(SimConnect_ClearInputGroup(handle(), _GroupID));
    }
    //@}

    /// \name Variable length strings
    //@{
    /// Used to assist in adding variable length strings to a structure. 
    static HRESULT insert_string(char* _pDest, DWORD _cbDest, void** _ppEnd, DWORD* _pcbStringV, const char* _pSource)
    {
        return  SimConnect_InsertString(_pDest, _cbDest, _ppEnd, _pcbStringV, _pSource);
    }

    static HRESULT retrieve_string(SIMCONNECT_RECV* _pData, DWORD _cbData, void* _pStringV, char** _ppszString, DWORD* _pcbString)
    {
        return SimConnect_RetrieveString(_pData, _cbData, _pStringV, _ppszString, _pcbString);
    }
    //@}

    /// \name Tracking Errors 
    //@{
    /// Returns the ID of the last packet sent to the SimConnect server.
    HRESULT get_last_sent_packet_id(DWORD& dwSendID)
    {
        return SimConnect_GetLastSentPacketID(handle(), &dwSendID);
    }

    /// Given the ID of an erroneous packet, return the description string of that call
    std::string get_record(DWORD _id) const
    {
        const ordered_records_t& or = boost::multi_index::get<1>(records);
        ordered_records_t::const_iterator it = or.find(_id);

        if (it != or.end())
            return it->descr_;
        else
            return "Description not found";
    }
    //@}

private:
    handle_t hsc_;

    HANDLE handle() const
    {
        FSC_PRECONDITION((bool) hsc_);
        return hsc_.get();
    }

    //The most recent calls list type
    typedef boost::multi_index::multi_index_container<
        detail::record,
        boost::multi_index::indexed_by<
            boost::multi_index::sequenced<>,
            boost::multi_index::hashed_unique<BOOST_MULTI_INDEX_MEMBER(detail::record, DWORD, id_)> >
    > records_t;

    typedef records_t::nth_index<1>::type ordered_records_t;

    //The maximum number of recorded calls
    static const size_t max_records = 10;
    records_t records;    

    // Record the ID along with the identification string
    void add_record(std::string _desc)
    {
        DWORD id;
        if (S_OK == SimConnect_GetLastSentPacketID(handle(), &id))
        {
            records.push_front(detail::record(id, _desc));
            if (records.size() > max_records)
                records.pop_back();
        }
    }
};


} //namespace fsx

#endif //__FSX_SIM_CONNECT_HPP__
