//  Copyright Joel de Guzman 2002-2004. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//  Hello World Example from the tutorial
//  [Joel de Guzman 10/9/2002]


#include "precompiled_header.h"
#include <boost/python.hpp>

#include <cockpit.hpp>


namespace py = boost::python;


//char const* greet()
//{
//   return "hello, world";
//}

//class basic_cockpit
//{
//public:
//    const char* greet() const
//    {
//       return "hello, world";
//    }
//
//    virtual int on_rev_sim_connect() const = 0;
//
//private:
//
//};
/*
basic_cockpit& make_cockpit(
    std::string _cockpit_file,
    std::string _config_path,
    fsx::sim_connect& _sim,
    iocards::usb_expansion_card& _hw)
{
    // Initialize cockpit (load python module)
    py::object cokpit_mod = py::import(_cockpit_file.c_str());
    py::object cockpit_class = cokpit_mod.attr("cockpit");

    //Construct the (derived) python cockpit
    py::object cockpit = cockpit_class(_config_path, _sim, _hw);
    return py::extract<basic_cockpit&>(cockpit);
}
*/

struct basic_cockpit_wrapper
    : basic_cockpit
    , boost::python::wrapper<basic_cockpit>
{
    basic_cockpit_wrapper(
        const std::string& _path,
        fsx::sim_connect& _sim,
        iocards::usb_expansion_card& _hw) :
        basic_cockpit(_path, _sim, _hw) {}

    virtual int on_rcv_sim_data()
    {
        return this->get_override("on_rcv_sim_data")();
    }
};


void export_cockpit()
{
    using namespace boost::python;

    class_<basic_cockpit_wrapper, boost::noncopyable>("basic_cockpit", init<std::string, fsx::sim_connect&, iocards::usb_expansion_card&>())
        .def("initialize", &basic_cockpit::initialize)
        .def("on_rcv_sim_data", pure_virtual(&basic_cockpit::on_rcv_sim_data))
        ;
}
