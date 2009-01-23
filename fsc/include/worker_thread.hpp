//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


/// \file work_thread.hpp
/// The main loop (apart from the GUI one)


#if !defined(__WORKER_THREAD_HPP__)
#define __WORKER_THREAD_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <boost/array.hpp>

#include <cockpit.hpp>
#include <win/event.hpp>
//#include <iocards/usb_expansion_card.hpp>
//#include <fsx/sim_connect.hpp>
#include <fsx/dispatch_proc.hpp>


#define WORK_PRECONDITION(x) _ASSERT((x))


//struct context
//{
//    context(fsx::sim_connect& _sc, iocards::usb_expansion_card& _hid, basic_cockpit& _cockpit)
//        : sc_(_sc), hid_(_hid), cockpit_(_cockpit) {}
//
//    fsx::sim_connect& sc_;
//    iocards::usb_expansion_card& hid_;
//    basic_cockpit& cockpit_;
//};


/// A callable object that implements the main loop
class work
{
public:
    enum event {EVENT_SIMCONNECT, EVENT_HID, EVENT_EXIT};

    work(
        basic_cockpit& _ckpt,
        win::event_t _simconn_event,
        win::event_t _hid_event,
        win::event_t _exit_event) :
        ckpt_(_ckpt)
    {
        WORK_PRECONDITION((_simconn_event.get() != INVALID_HANDLE_VALUE));
        WORK_PRECONDITION((_hid_event.get() != INVALID_HANDLE_VALUE));
        WORK_PRECONDITION((_exit_event.get() != INVALID_HANDLE_VALUE));

        events_[EVENT_SIMCONNECT] = _simconn_event.get();
	    events_[EVENT_HID] = _hid_event.get();
        events_[EVENT_EXIT] = _exit_event.get();
    }

    void operator()()
    {
        bool exit = false;
        while (!exit)
        {
            DWORD EvtId = ::WaitForMultipleObjects(events_.size(), events_.c_array(), FALSE, INFINITE);

            switch (EvtId)
            {
            case WAIT_OBJECT_0 + EVENT_SIMCONNECT:
                ckpt_.sim().call_dispatch(dispatch_proc, &exit);
                break;

            case WAIT_ABANDONED_0 + EVENT_HID:
            case WAIT_TIMEOUT:
                ckpt_.hw().cancel_io();
                break;

            case WAIT_OBJECT_0 + EVENT_HID:
                //LDBG_ << std::setbase(std::ios_base::hex)
                //    << (int) ckpt_.hid_.input_buffer[0] << " "
                //    << (int) input_buffer[1] << " " 
                //    << (int) input_buffer[2] << " " 
                //    << (int) input_buffer[3] << " " 
                //    << (int) input_buffer[4] << " " 
                //    << (int) input_buffer[5] << " " 
                //    << (int) input_buffer[6] << " " 
                //    << (int) input_buffer[7] << " " 
                //    << (int) input_buffer[8];
                {
                DWORD nb_bytes_transferred = ckpt_.hw().overlapped_result();

                //TODO: Parse HID buffer...
                //boost::dynamic_bitset<> bitset(res.second * 8);
                //hid_to_event_mapping bitset

                // Initiate a new read
                DWORD nb_bytes_read = ckpt_.hw().read();

                //hr = sc.transmit_client_event(SIMCONNECT_OBJECT_ID_USER, EVENT_MIXTURE_SET, input_buffer[2] * 0x40, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                }
                break;

            case WAIT_OBJECT_0 + EVENT_EXIT:
                exit = true;
                break;

            default :
                LERR_ << "unexpected event: " << EvtId;
            }
        }
    }

private:
    boost::array<HANDLE, 3> events_;
     basic_cockpit& ckpt_;
};


#endif //__WORKER_THREAD_HPP__