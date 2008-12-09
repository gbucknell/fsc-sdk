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
#include "resource.h"

#include <iomanip>
#include <log.hpp>
#include <cockpit.hpp>
#include <win/event.hpp>
#include <win/exception.hpp>
#include <fsx/sim_connect.hpp>
#include <fsx/dispatch_proc.hpp>
#include <iocards/usb_expansion_card.hpp>


extern "C"
{
#include <hidsdi.h>
#include <hidpi.h>
}

#include <vector>
#include <string>
#include <boost/array.hpp>
#include <boost/shared_array.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/program_options.hpp>
#include <boost/python.hpp>

#include <gui/splash_wnd.hpp>
#include <gui/taskbar_notify_icon.hpp>

namespace po = boost::program_options;
namespace py = boost::python;
namespace lg = boost::logging;


//Global ATL module
CAppModule _Module;

void init_wtl(HINSTANCE _hInstance);
void uninit_wtl();


//int _tmain(int argc, _TCHAR* argv[])
int WINAPI _tWinMain(HINSTANCE _hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR _lpstrCmdLine, int _nCmdShow)
{
    //Split command line
    std::vector<std::wstring> args = po::split_winmain(_lpstrCmdLine);

    std::string prog_name(args[0].begin(), args[0].end());
    std::wstring cockpit_file;

    // Declare the supported options.
    lg::level::type log_level;
    po::options_description other_opts("Other options");
    other_opts.add_options()
        ("nologo", "suppress startup splash screen")
        ("version,v", "print version string")
        ("help", "produce help message")
        ("log-level", po::wvalue<lg::level::type>(&log_level)->default_value(lg::level::error), "set log level (0-5000)")
        ;

    //Cockpit positional option
    po::options_description cockpit_opts("Cockpit options");
    cockpit_opts.add_options()
		("cockpit", po::wvalue<std::wstring>(&cockpit_file), "the path of the cockpit file");

    po::positional_options_description p;
    p.add("cockpit", -1);

    po::options_description cmd_line_opts("USAGE : " + prog_name + " [options] <cockpit> where options may be");
    cmd_line_opts.add(other_opts);

    po::options_description all_opts;
    all_opts.add(other_opts).add(cockpit_opts);

    po::variables_map vm;
    po::store(po::wcommand_line_parser(args).options(all_opts).positional(p).run(), vm);
    po::notify(vm);    

    //Init logging
    init_logs();

    //Init GUI components
    init_wtl(_hInstance);

    if (! vm.count("nologo"))
    {
        //Show splash screen
        new gui::splash_wnd(IDB_SPLASH, 3000, NULL);
    }

    if (vm.count("help") || cockpit_file.empty())
    {
        std::cout << cmd_line_opts << "\n";
        return 0;
    }

    if (vm.count("version"))
    {
        //TODO Print version
        return 0;
    }

    if (vm.count("log-level"))
        LINFO_ << "Log level was set to " << log_level << ".";
    else
        LINFO_ << "Log level was not set.";

    try
    {
        typedef boost::shared_ptr<void> handle_t;
        handle_t hid_event = win::create_event(TRUE); //Manual reset
        handle_t simconn_event = win::create_event();

        // Open USB expansion
        iocards::usb_expansion_card usb;
        if (! usb.open(1, 2, hid_event.get()))
        {
            LERR_ << "Failed to open hardware!";
            throw "Failed to open hardware!";
        }
        
        LINFO_ << "Connected to HID!";

        std::vector<boost::uint8_t> input_buffer(usb.input_size());
        std::vector<boost::uint8_t> output_buffer(usb.output_size());

        //Open SimConnect
        fsx::sim_connect sc;
        if (!SUCCEEDED(sc.open("Client Event", NULL, 0, simconn_event.get(), 0)))
        {
            LERR_ << "Failed to connect to Flight Simulator!";
            throw "Failed to connect to Flight Simulator!";
        }

        LINFO_ << "Connected to Flight Simulator!";

        //Start the python interpreter
        Py_Initialize();

        LINFO_ << "Python is initialized!";

        py::object main_module = py::import("__main__");
        py::object main_namespace = main_module.attr("__dict__");

        py::object ignored = py::exec("hello = file('hello.txt', 'w')\n"
                              "hello.write('Hello world!')\n"
                              "hello.close()",
                              main_namespace);

/*
        // Get capabilities of the HW
        HIDP_CAPS capabilities = hid.capabilities();
    	
        boost::shared_array<unsigned char> input_buffer(new (unsigned char[capabilities.InputReportByteLength]));

        std::pair<BOOL, DWORD> res = hid.read(input_buffer.get(), capabilities.InputReportByteLength);
        if (res.first)
            DWORD dwError = GetLastError();

        //boost::shared_array<unsigned char> output_buffer(new (unsigned char[capabilities.OutputReportByteLength]));
        //unsigned char output_buffer[] = {0x00, 0x3D, 0x00, 0x3A, 0x01, 0x39, 0x02, 0xFF, 0xFF};
        //unsigned char output_buffer[] = {0x00, 0x3D, 0x00, 0x3A, 0x01, 0x39, 0x00, 0xFF, 0xFF};
        unsigned char config[] = {0x00, 0x3D, 0x00, 0x39, 0x00, 0x3A, 0x01, 0xFF, 0xFF};
        res = hid.write(output_buffer.get(), capabilities.InputReportByteLength);
        if (res.first)
            DWORD dwError = GetLastError();
*/
        // Initiate a read from the USB device
        usb.read(input_buffer);

        // Initialize cockpit
        cockpit ckpt(boost::to_utf8(cockpit_file));
        ckpt.initialize(sc);

		// Set a notification when the simulation starts so we can get initial value
		// and set output accordingly.
        HRESULT hr = sc.subscribe_to_system_event(0, "SimStart");

        //Transmit event
        //hr = sc.transmit_client_event(SIMCONNECT_OBJECT_ID_USER, EVENT_AUTOPILOT_HEADING_BUG_SET, 333, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
        //hr = sc.transmit_client_event(SIMCONNECT_OBJECT_ID_USER, EVENT_DME_SELECT, 2, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);

        //double ThrottlePercent = 80.;
        //hr = sc.set_data_on_sim_object(DEFINITION_THROTTLE, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(ThrottlePercent), &ThrottlePercent);

        enum EVENT {EVENT_SIMCONNECT, EVENT_HID};
        boost::array<HANDLE, 2> events;
        events[EVENT_SIMCONNECT] = simconn_event.get();
		events[EVENT_HID] = hid_event.get();

        bool quit = false;
        context<cockpit> ctxt(quit, ckpt);
        while (!quit)
        {
            DWORD EvtId = ::WaitForMultipleObjects(events.size(), events.c_array(), FALSE, INFINITE);

            switch (EvtId)
            {
            case WAIT_OBJECT_0 + EVENT_SIMCONNECT:
                sc.call_dispatch(dispatch_proc<cockpit>, &quit);
                break;

            case WAIT_ABANDONED_0 + EVENT_HID:
            case WAIT_TIMEOUT:
                usb.cancel_io();
                break;

            case WAIT_OBJECT_0 + EVENT_HID:
                LDBG_ << std::setbase(std::ios_base::hex)
                    << (int) input_buffer[0] << " "
                    << (int) input_buffer[1] << " " 
                    << (int) input_buffer[2] << " " 
                    << (int) input_buffer[3] << " " 
                    << (int) input_buffer[4] << " " 
                    << (int) input_buffer[5] << " " 
                    << (int) input_buffer[6] << " " 
                    << (int) input_buffer[7] << " " 
                    << (int) input_buffer[8];
                {
                DWORD nb_bytes_transferred = usb.overlapped_result();

                //TODO: Parse HID buffer...
                //boost::dynamic_bitset<> bitset(res.second * 8);
                //hid_to_event_mapping bitset

                // Initiate a read
                DWORD nb_bytes_read = usb.read(input_buffer);

                //hr = sc.transmit_client_event(SIMCONNECT_OBJECT_ID_USER, EVENT_MIXTURE_SET, input_buffer[2] * 0x40, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                }
                break;

            default :
                LERR_ << "unexpected event: " << EvtId;

            }
        }
    }
    catch(char* _ex)
    {
        std::cerr << _ex << std::endl;    
    }
    catch (win::runtime_error& _ex)
    {
        LERR_ << "Caught: " << _ex.what();
        LERR_ << "Type: " << typeid(_ex).name();
        LERR_ << "Last Error: " << _ex.last_error();
    }
    catch (std::exception& _ex)
    {
        LERR_ << "Caught: " << _ex.what();
        LERR_ << "Type: " << typeid(_ex).name();
    }
    catch(py::error_already_set const &)
    {
        // TODO redirect stderr to LERR_.
        PyErr_Print();
    };

    uninit_wtl();

    return 0;
}


void init_wtl(HINSTANCE _hInstance)
{
    HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ATLASSERT(SUCCEEDED(hRes));

    // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
    ::DefWindowProc(NULL, 0, 0, 0L);

    AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

    hRes = _Module.Init(NULL, _hInstance);
    ATLASSERT(SUCCEEDED(hRes));

    AtlAxWinInit();
}


void uninit_wtl()
{
    _Module.Term();
    ::CoUninitialize();
}