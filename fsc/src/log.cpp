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
#include <log.hpp>
#include <boost/logging/format.hpp>
#include <boost/logging/writer/ts_write.hpp>

using namespace boost::logging;

// Step 6: Define the filters and loggers you'll use
BOOST_DEFINE_LOG(g_l, log_type)
BOOST_DEFINE_LOG_FILTER(g_l_filter, level::holder)


void init_logs()
{
    // Add formatters and destinations
    // That is, how the message is to be formatted...
    g_l()->writer().add_formatter( formatter::idx() );
    g_l()->writer().add_formatter( formatter::time("$hh:$mm.$ss ") );
    g_l()->writer().add_formatter( formatter::append_newline() );

    //        ... and where should it be written to
    //g_l()->writer().add_destination( destination::cout() );
    g_l()->writer().add_destination( destination::dbg_window() );
    g_l()->writer().add_destination( destination::file("log.txt") );
    g_l()->turn_cache_off();
}


void set_level(level::type _level)
{
    g_l_filter()->set_enabled(_level);
}