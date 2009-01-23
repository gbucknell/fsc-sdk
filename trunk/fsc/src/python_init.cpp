//  Copyright Joel de Guzman 2002-2004. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//  Hello World Example from the tutorial
//  [Joel de Guzman 10/9/2002]


#include "precompiled_header.h"
#include <boost/python.hpp>

#include <cockpit.hpp>
#include <python/init.hpp>


namespace py = boost::python;


void python::init()
{
    //Initialize python
    Py_Initialize();
    atexit(Py_Finalize);

    py::object main_module = py::import("__main__");
    py::object main_namespace = main_module.attr("__dict__");

    //Redirect error messages to StringIO
    boost::python::exec("import cStringIO\n"
                        "import sys\n"
                        "sys.stderr = cStringIO.StringIO()"
                      , main_namespace
                      , py::dict());
}


std::string python::get_error()
{
    PyErr_Print();
    boost::python::object sys(boost::python::handle<>(PyImport_ImportModule("sys")));
    boost::python::object err = sys.attr("stderr");
    return boost::python::extract<std::string>(err.attr("getvalue")());
}
