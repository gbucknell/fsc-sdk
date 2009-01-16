//  Copyright Joel de Guzman 2002-2004. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//  Hello World Example from the tutorial
//  [Joel de Guzman 10/9/2002]


#include "precompiled_header.h"
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

#include <cockpit.hpp>

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


struct basic_cockpit_wrapper
    : basic_cockpit
    , boost::python::wrapper<basic_cockpit>
{
    basic_cockpit_wrapper(const std::string& _path) : basic_cockpit(_path) {}

    virtual int on_rcv_sim_data()
    {
        return this->get_override("on_rcv_sim_data")();
    }
};



BOOST_PYTHON_MODULE(fsc)
{
    using namespace boost::python;

    class_<basic_cockpit_wrapper, boost::noncopyable>("basic_cockpit", init<std::string>())
            .def("initialize", &basic_cockpit::initialize)
            .def("on_rcv_sim_data", pure_virtual(&basic_cockpit::on_rcv_sim_data))
        ;
}
