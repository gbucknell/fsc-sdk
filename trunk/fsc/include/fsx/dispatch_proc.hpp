//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


#include <fsx/sim_connect.hpp>


template <class CockpitT>
struct context
{
    context(bool& _quit, CockpitT& _cockpit) : quit_(_quit), cockpit_(_cockpit) {}

    bool& quit_;
    CockpitT& cockpit_;
};


/// A callback function to handle all the communications with the SimConnect server
template <class CockpitT>
inline
void CALLBACK dispatch_proc(SIMCONNECT_RECV* pData, DWORD cbData, void *pContext)
{
	typedef context<CockpitT> context_t;

    context_t& ctxt = *(context_t*)pContext;

    switch(pData->dwID)
    {
        case SIMCONNECT_RECV_ID_OPEN:
        {
            //Handle SimConnect version information received in a SIMCONNECT_RECV_OPEN structure.
            SIMCONNECT_RECV_OPEN& openData = *(SIMCONNECT_RECV_OPEN*) pData;
            LDBG_ << openData.szApplicationName
                << " version " << openData.dwApplicationVersionMajor 
                << '.' << openData.dwApplicationVersionMinor
                << '.' << openData.dwApplicationBuildMajor
                << '.' << openData.dwApplicationBuildMinor
                << " with SimConnect"
                << " version " << openData.dwSimConnectVersionMajor
                << '.' << openData.dwSimConnectVersionMinor
                << '.' << openData.dwSimConnectBuildMajor
                << '.' << openData.dwSimConnectBuildMinor;
            break;
        }

        case SIMCONNECT_RECV_ID_QUIT:
            //Handle exiting the application
            ctxt.quit_ = true;
            break;

        case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
        {
            SIMCONNECT_RECV_SIMOBJECT_DATA *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*) pData;
            ctxt.cockpit_.on_rcv_sim_data(*pObjData);

            //switch(pObjData->dwRequestID)
            //{
            //    case REQUEST_THROTTLE:
            //    {
            //        float& throttle = (float&)pObjData->dwData;
            //        LDBG_ << "Throttle : " << throttle;
            //        break;
            //    }

            //    case REQUEST_FLAPS:
            //    {
            //        flaps& f = (flaps&)pObjData->dwData;
            //        LDBG_ << "Flaps index : " << f.idx;
            //        LDBG_ << "Flaps percent : " << f.percent;
            //        break;
            //    }
            //}

            break;
        }

        case SIMCONNECT_RECV_ID_EVENT:
        {
            SIMCONNECT_RECV_EVENT *evt = (SIMCONNECT_RECV_EVENT*)pData;
            ctxt.cockpit_.on_rcv_sim_event(*evt);

            //switch(evt->uEventID)
            //{
            //    case EVENT_BRAKES:
            //        LDBG_ << "Event brakes: " << evt->dwData;
            //        break;

            //    case EVENT_STROBES_TOGGLE:
            //        LDBG_ << "Event strobes toggle: " << evt->dwData;
            //        break;

            //    case EVENT_AUTOPILOT_HEADING_BUG_SET:
            //        LDBG_ << "Event autopilot heading bug: " << evt->dwData;
            //        break;

            //    case EVENT_THROTTLE_SET:
            //        LDBG_ << "Event throttle: " << evt->dwData / 16383.0;
            //        break;

            //    case EVENT_MIXTURE_SET:
            //        LDBG_ << "Event mixture: " << evt->dwData / 16383.0;
            //        break;

            //    case EVENT_DME_SELECT:
            //        LDBG_ << "Event DME select: " << evt->dwData;
            //        break;

            //    case EVENT_MY_EVENT:
            //        LDBG_ << "Event My event: " << evt->dwData;
            //        break;

            //    default:
            //        break;
            //}
            break;
        }

        default:
            LERR_ << "unexpected FSX event;" << pData->dwID;
            break;
    }
}