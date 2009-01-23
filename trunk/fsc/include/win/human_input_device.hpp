//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


/// \file human_input_device.hpp
/// HID c++ API


#if !defined(__WIN_HUMAN_INPUT_DEVICE_HPP__)
#define __WIN_HUMAN_INPUT_DEVICE_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <windows.h>
extern "C" {
#include <hidsdi.h>
}

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include <win/exception.hpp>
#include <assert.hpp>


#define HID_PRECONDITION(x) _ASSERT((x))


namespace win {


/// HID object. Encapuslate a Human Input Device handle.
class human_input_device //: public boost::noncopyable
{
    typedef boost::shared_ptr<void> handle_t;

public:
    human_input_device() {memset(&overlapped_, 0, sizeof(OVERLAPPED));}

    /// Open an HID with a given vendor id and product id.
    BOOL open(USHORT _VendorId, USHORT _ProductId, HANDLE _hOverlapped);

    /// Close the HID
    BOOL close() {hid_.reset(); }

    /// Return true is the human_input_device object is currently opened, false otherwise.
    bool is_open() {return hid_;}

    /// Return information about a top-level collection's capability.
    HIDP_CAPS capabilities() const;

    /// Set up device notification (i.e. plug or unplug)
    boost::shared_ptr<void>  register_notification(HWND _hWnd) const;

    /// Cancels all pending input and output (I/O) operations that are issued by the calling thread.
    BOOL cancel_io() const
    {
        HID_PRECONDITION(hid_ != NULL);
        return ::CancelIo(handle());
    }

    /// Reads data from the device.
    DWORD read(LPVOID _lpBuffer, DWORD _dwNumberOfBytesToRead)
    {
        DWORD number_of_bytes_read = 0;
        if (! ::ReadFile(handle(), _lpBuffer, _dwNumberOfBytesToRead, &number_of_bytes_read, &overlapped_))
        {
            DWORD err = ::GetLastError();
            if (err != ERROR_IO_PENDING)
                throw runtime_error("ReadFile failed", err);
        }
        return number_of_bytes_read;
    }

    /// Write data to the device.
    DWORD write(LPVOID _lpBuffer, DWORD _dwNumberOfBytesToWrite)
    {
        DWORD number_of_bytes_written = 0;
        if (! ::WriteFile(handle(), _lpBuffer, _dwNumberOfBytesToWrite, &number_of_bytes_written, &overlapped_))
        {
            DWORD err = ::GetLastError();
            if (err != ERROR_IO_PENDING)
                throw runtime_error("WriteFile failed", err);
        }
        return number_of_bytes_written;
    }

    DWORD overlapped_result(BOOL _bWait = FALSE)
    {
        DWORD number_of_bytes_transferred = 0;
        if (! ::GetOverlappedResult(handle(), &overlapped_, &number_of_bytes_transferred, _bWait))
            throw runtime_error("GetOverlappedResult failed");
        return number_of_bytes_transferred;
    }


private:
    handle_t hid_;
    OVERLAPPED overlapped_;

    HANDLE handle() const
    {
        FSC_PRECONDITION((bool) hid_);
        return hid_.get();
    }
};


} //namespace win


#endif //__WIN_HUMAN_INPUT_DEVICE_HPP__