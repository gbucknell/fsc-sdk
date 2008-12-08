//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//	$Revision: $
//	$History: $


/// \file master_card.hpp
/// HID c++ API


#if !defined(__IOCARDS_MASTER_CARD_HPP__)
#define __IOCARDS_MASTER_CARD_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace iocards {


/// Master card
class master_card
{
public:
    master_card();
    ~master_card();

    /// Map hardware to IO
    void map_hw_to_io();
};


} //namespace iocards


#endif //__IOCARDS_MASTER_CARD_HPP__