//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//	$Revision: $
//	$History: $


/// \file usb_expansion_card.hpp
/// USB expansion card


#if !defined(__IOCARDS_USB_EXPANSION_HPP__)
#define __IOCARDS_USB_EXPANSION_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <boost/shared_array.hpp>
#include <boost/cstdint.hpp>

#include <win/human_input_device.hpp>


namespace iocards {


class master_card;


/// USB expansion card
class usb_expansion_card
{
public:
    usb_expansion_card() : size_in_(0), size_out_(0), nb_of_master_cards_(0), nb_of_ad_converters_(0) {}
    ~usb_expansion_card();

    /// Open an USB expansion card with a given number of connected master cards and used A/D converters.
    BOOL open(boost::uint8_t _nb_of_master_cards, boost::uint8_t _nb_of_ad_converters, HANDLE _evt);

    template <class RandomAccessContainer>
    DWORD read(RandomAccessContainer& _in)
    {
        _ASSERT(_in.size() <= input_size());
        return hid_.read(&_in[0], _in.size());
    }

    template <class RandomAccessContainer>
    DWORD write(const RandomAccessContainer& _out)
    {
        _ASSERT(_out.size() <= output_size());
        return hid_.write(&_out[0], _out.size());
    }

    BOOL cancel_io() const {return hid_.cancel_io();}
    DWORD overlapped_result() {return hid_.overlapped_result();}

    size_t input_size() const {return size_in_;}
    size_t output_size() const {return size_out_;}

    boost::uint8_t nb_of_master_cards() const {return nb_of_master_cards_;}
    boost::uint8_t nb_of_ad_converters() const {return nb_of_ad_converters_;}

private:
	win::human_input_device hid_;
    size_t size_in_;
    size_t size_out_;

    boost::uint8_t nb_of_master_cards_;
    boost::uint8_t nb_of_ad_converters_;
};


} //namespace iocards

#endif //__IOCARDS_USB_EXPANSION_HPP__