//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


/// \file assert.hpp
/// A few assertion macros


#if !defined(__ASSERT_HPP__)
#define __ASSERT_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if !defined(NDEBUG)
    #define FSC_PRECONDITION(x) _ASSERT((x))
    #define FSC_ASSERT(x) _ASSERT((x))
#else
    #define FSC_PRECONDITION(x)
    #define FSC_ASSERT(x)
#endif


#endif //__ASSERT_HPP__