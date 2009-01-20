//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $

/*!
    \mainpage Cockpit SDK for Flight Simulator X and OpenCockpit I/O Cards
    
    \section intro_sec Introduction
    OpenCockpit is an non profit organisation that developps and distributes Copckit componants. This SDK aims to help software developpers to build their own cockpit software based on OpenCockpit hardware. It 

    \subsection rational Rationale
    Why an other developpement when SIOC is already well tested and widely used ?
        - The need for an open source project
        - The need to bypass FSUIPC, since an official API exists for FSX
        - Customization and efficiency, "don't pay for what you don't use"
        - A better debugger
        - For fun
    
    \section install_sec Installation
 
    \subsection prerequis Prerequisites
    I do my best to keep the dependancies to third party libraries as small as possible. All the required tools and libraries given in the next sections are Open Source or available for free.
    \subsection skills Skills
    SDKs
    emphazises some good practicices of the boost community (the anti-chamber of the standard c++ libraries)    
    \subsection tools Tools
        - A c++ compiler. I suggest Visual C++ Express, a free IDE available from Microsoft.
    \subsection libraries Librairies
        - FSX SDK available from http://www.fsinsider.com
        - Windows SDK (formely Platform SDK) available from Microsoft
        - Windows WDK (driver SDK) available from Microsoft
        - Boost libraries available from http://www.boost.org
*/


/*! \namespace fsx
    \brief MS Flight Simulator SDK
 
    TODO.
*/
 
/*! \namespace win
    \brief MS Windows Software and Driver DK
 
    Ecapsulate some of the functionnalities of the platform SDK.
*/

/*! \namespace iocards
    \brief IO cards SDK

    TODO.
*/

/*! \namespace hw
    \brief Hardware components

    TODO.
*/