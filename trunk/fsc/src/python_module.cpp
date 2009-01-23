//  Copyright Joel de Guzman 2002-2004. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//  Hello World Example from the tutorial
//  [Joel de Guzman 10/9/2002]


#include "precompiled_header.h"
#include <boost/python.hpp>


#include <fsx/sim_connect.hpp>
#include <iocards/usb_expansion_card.hpp>


void export_cockpit();


BOOST_PYTHON_MODULE(fsc)
{
    using namespace boost::python;

    export_cockpit();

    class_<iocards::usb_expansion_card>("usb_expansion_card");
    class_<fsx::sim_connect>("sim_connect");
}
