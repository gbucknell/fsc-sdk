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
#include <worker_thread.hpp>
#include <cockpit.hpp>
#include <win/single_instance.hpp>
#include <win/event.hpp>
#include <win/exception.hpp>
#include <fsx/sim_connect.hpp>
#include <iocards/usb_expansion_card.hpp>


extern "C"
{
#include <hidsdi.h>
#include <hidpi.h>
}

#include <vector>
#include <string>
#include <boost/shared_array.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/program_options.hpp>
#include <boost/thread.hpp>
#include <boost/python.hpp>

#include <gui/init.hpp>
#include <gui/splash_wnd.hpp>
#include <gui/taskbar_notify_icon.hpp>

#include <python.hpp>

namespace po = boost::program_options;
namespace py = boost::python;
namespace lg = boost::logging;


//Global ATL module
CAppModule _Module;


//int _tmain(int argc, _TCHAR* argv[])
int WINAPI _tWinMain(HINSTANCE _hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR _lpstrCmdLine, int _nCmdShow)
{
    //Initialize GUI components (prior to any call to ATL/WTL)
    gui::init(_hInstance);

    //Limit applications to one instance
    win::single_instance instance(L"D3001DBE-4718-43e0-8C70-47802C927165");
    if (instance.is_already_running())
    {
        AtlMessageBox(NULL, IDS_SINGLE_INSTANCE, IDS_SINGLE_INSTANCE_TITLE);
        //::MessageBox(NULL, L"FSC is limited to a single instance.", L"Already running", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    //Split command line
    std::vector<std::wstring> args = po::split_winmain(_lpstrCmdLine);

    std::string prog_name(args[0].begin(), args[0].end());
    std::wstring cockpit_file;

    // Declare the supported options.
    po::options_description debug_opts("Debug options");
    debug_opts.add_options()
        ("no-sim", "do not connect to fsx")
        ("no-hw", "do not connect to human interface devices")
        ;

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
    all_opts.add(debug_opts).add(other_opts).add(cockpit_opts);

    po::variables_map vm;
    po::store(po::wcommand_line_parser(args).options(all_opts).positional(p).run(), vm);
    po::notify(vm);    

    //Init logging
    init_logs();

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
        win::event_t hid_event = win::create_event(TRUE); //Manual reset
        win::event_t simconn_event = win::create_event();
        win::event_t exit_event = win::create_event();

        // Open USB expansion
        iocards::usb_expansion_card hw;
        if (! vm.count("no-hw"))
            if (! hw.open(1, 2, hid_event.get()))
                throw "Failed to open hardware!";
        
        LINFO_ << "Connected to HID!";

        //Open SimConnect
        fsx::sim_connect sim;
        if (! vm.count("no-sim"))
            if (!SUCCEEDED(sim.open("Client Event", NULL, 0, simconn_event.get(), 0)))
                throw "Failed to connect to Flight Simulator!";

        LINFO_ << "Connected to Flight Simulator!";

        // Register the module with the interpreter
        if (PyImport_AppendInittab("fsc", initfsc) == -1)
            throw std::runtime_error("Failed to add fsc module to the python interpreter");

        //Start the python interpreter
        python::init();

        LINFO_ << "Python is initialized!";

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
        // Initialize cockpit (load python module)
        py::object cokpit_mod = py::import(boost::to_utf8(cockpit_file).c_str());
	    py::object cockpit_class = cokpit_mod.attr("cockpit");
    	
        //Construct the (derived) python cockpit
	    py::object py_cockpit = cockpit_class(sim, hw, "test.cfg");
        basic_cockpit& cockpit = py::extract<basic_cockpit&>(py_cockpit);

        //basic_cockpit& cockpit = python::make_cockpit(boost::to_utf8(cockpit_file), "test.cfg", sim, hw);

        //Call the initialize member function
	    cockpit.initialize();

        // Initiate a read from the USB device
        hw.read();

		// Set a notification when the simulation starts so we can get initial value
		// and set output accordingly.
        HRESULT hr = sim.subscribe_to_system_event(0, "SimStart");

        //Run the worker thread
        work w(cockpit, hid_event, simconn_event, exit_event);
        boost::thread worker(w);

        //Finally run GUI components
        gui::run();

        //Transmit event
        //hr = sim.transmit_client_event(SIMCONNECT_OBJECT_ID_USER, EVENT_AUTOPILOT_HEADING_BUG_SET, 333, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
        //hr = sim.transmit_client_event(SIMCONNECT_OBJECT_ID_USER, EVENT_DME_SELECT, 2, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);

        //double ThrottlePercent = 80.;
        //hr = sim.set_data_on_sim_object(DEFINITION_THROTTLE, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(ThrottlePercent), &ThrottlePercent);
/*
        enum EVENT {EVENT_SIMCONNECT, EVENT_HID, EVENT_EXIT};
        boost::array<HANDLE, 3> events;
        events[EVENT_SIMCONNECT] = simconn_event.get();
		events[EVENT_HID] = hid_event.get();
        events[EVENT_EXIT] = exit_event.get();

        bool quit = false;
        context<basic_cockpit> ctxt(quit, sim, cockpit);
        while (!quit)
        {
            DWORD EvtId = ::WaitForMultipleObjects(events.size(), events.c_array(), FALSE, INFINITE);

            switch (EvtId)
            {
            case WAIT_OBJECT_0 + EVENT_SIMCONNECT:
                sim.call_dispatch(dispatch_proc<basic_cockpit>, &quit);
                break;

            case WAIT_ABANDONED_0 + EVENT_HID:
            case WAIT_TIMEOUT:
                hw.cancel_io();
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
                DWORD nb_bytes_transferred = hw.overlapped_result();

                //TODO: Parse HID buffer...
                //boost::dynamic_bitset<> bitset(res.second * 8);
                //hid_to_event_mapping bitset

                // Initiate a read
                DWORD nb_bytes_read = hw.read(input_buffer);

                //hr = sim.transmit_client_event(SIMCONNECT_OBJECT_ID_USER, EVENT_MIXTURE_SET, input_buffer[2] * 0x40, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                }
                break;

            case WAIT_OBJECT_0 + EVENT_EXIT:
                quit = true;
                break;

            default :
                LERR_ << "unexpected event: " << EvtId;
            }
        }
*/
        //End the worker thread
        win::set(exit_event);
        worker.join();
    }
    catch(char* _ex)
    {
        LERR_ << _ex << std::endl;    
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
        LERR_ << python::get_error();
    };
    
    gui::uninit();

    return 0;
}
