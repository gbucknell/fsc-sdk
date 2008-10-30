//  (c) Copyright 2004 - HYDROWIDE - Project LIQUID
//
//  Distributed under the Hydrowide Software License,
//  (See accompanying file license.txt
//  or copy at http://www.hydrowide.com/license.txt)
//
//	$Revision: 1.1 $
//	$History: $


#if !defined(__HYDROWIDE_LIQUID_THIRD_PARTY_PCH_H__)
#define __HYDROWIDE_LIQUID_THIRD_PARTY_PCH_H__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if defined(BOOST_BUILD_PCH_ENABLED)


//IO
#include <iomanip>
#include <iostream>

//STL
#include <algorithm>
#include <cmath>
#include <limits>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <stack>
#include <string>
#include <valarray>
#include <vector>

//Boost
#include <boost/array.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>

//WTL
//// Change these values to use different versions
//#define WINVER		    0x0600
//#define _WIN32_WINNT	0x0600
//#define _WIN32_IE	    0x0501
//#define _RICHEDIT_VER	0x0200

// This project was generated for VC++ 2005 Express and ATL 3.0 from Platform SDK.
// Comment out this line to build the project with different versions of VC++ and ATL.
#define _WTL_SUPPORT_SDK_ATL3

// Support for VS Express & SDK ATL
#ifdef _WTL_SUPPORT_SDK_ATL3
  #pragma conform(forScope, off)
  #pragma comment(linker, "/NODEFAULTLIB:atlthunk.lib")
#endif // _WTL_SUPPORT_SDK_ATL3

#include <atlbase.h> 

// Support for VS200x Express & SDK ATL
#ifdef _WTL_SUPPORT_SDK_ATL3
  namespace ATL
  {
	inline void * __stdcall __AllocStdCallThunk()
	{
		return ::HeapAlloc(::GetProcessHeap(), 0, sizeof(_stdcallthunk));
	}

	inline void __stdcall __FreeStdCallThunk(void *p)
	{
		::HeapFree(::GetProcessHeap(), 0, p);
	}
  };
#endif // _WTL_SUPPORT_SDK_ATL3

#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>
#include <atlcom.h>
#include <atlhost.h>
#include <atlctl.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>

#include <atlcrack.h>
#include <atlmisc.h>

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

//WDK
extern "C" {
#include <hidsdi.h>
}
#include <setupapi.h>


#endif //BOOST_BUILD_PCH_ENABLED

#endif //__HYDROWIDE_LIQUID_THIRD_PARTY_PCH_H__