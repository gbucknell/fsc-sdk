//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


/// \file log.hpp
/// HID c++ API


#if !defined(__LOG_HPP__)
#define __LOG_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <boost/logging/format_fwd.hpp>

// Step 1: Optimize : use a cache string, to make formatting the message faster
BOOST_LOG_FORMAT_MSG( optimize::cache_string_one_str<> ) 

#ifndef BOOST_LOG_COMPILE_FAST
#include <boost/logging/format.hpp>
#include <boost/logging/writer/ts_write.hpp>
#endif

// Step 3 : Specify your logging class(es)
typedef boost::logging::logger_format_write< > log_type;

// Step 4: declare which filters and loggers you'll use
BOOST_DECLARE_LOG_FILTER(g_l_filter, boost::logging::level::holder)
BOOST_DECLARE_LOG(g_l, log_type)

// Step 5: define the macros through which you'll log
#define LDBG_ BOOST_LOG_USE_LOG_IF_LEVEL(g_l(), g_l_filter(), debug ) << " [dbg] "
#define LERR_ BOOST_LOG_USE_LOG_IF_LEVEL(g_l(), g_l_filter(), error ) << " [ERR] "
#define LINFO_ BOOST_LOG_USE_LOG_IF_LEVEL(g_l(), g_l_filter(), info ) << " "

void init_logs();

#endif //__LOG_HPP__