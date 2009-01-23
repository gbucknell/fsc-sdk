//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//	$Revision: $
//	$History: $


#include "precompiled_header.h"
#include <iocards/usb_expansion_card.hpp>


using namespace iocards;

/*
usb_expansion::usb_expansion()
{
}
*/


BOOL usb_expansion_card::open(boost::uint8_t _nb_of_master_cards, boost::uint8_t _nb_of_ad_converters, HANDLE _evt)
{
    nb_of_master_cards_ = _nb_of_master_cards;
    nb_of_ad_converters_ = _nb_of_ad_converters;

    // Open device
    BOOL res = hid_.open(0x0000, 0x0000, _evt);

    if (res)
    {
        // Get capabilities of the HW
        HIDP_CAPS capabilities = hid_.capabilities();
        size_in_ = capabilities.InputReportByteLength;
        size_out_ = capabilities.OutputReportByteLength;

        //Resize buffers accordingly
        input_buffer_.resize(size_in_);
        output_buffer_.resize(size_out_);

        // Write configuration
        unsigned char config[] = {0x00, 0x3D, 0x00, 0x3A, 0x00, 0x39, 0x00, 0xFF, 0xFF};
        config[4] = _nb_of_master_cards;
        config[6] = _nb_of_ad_converters;
        hid_.write(config, sizeof(config));
    }

    return res;
}

usb_expansion_card::~usb_expansion_card()
{
}
