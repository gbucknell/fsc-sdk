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
#include <win/human_input_device.hpp>

#include <setupapi.h>
#include <dbt.h>

#include <boost/shared_ptr.hpp>
#include <boost/raw_scoped_ptr.hpp>


using namespace win;


BOOL human_input_device::open(USHORT _VendorId, USHORT _ProductId, HANDLE _hOverlapped)
{
    // Get the HID Globally Unique ID from the OS
    static GUID HidGuid;
    ::HidD_GetHidGuid(&HidGuid);

    overlapped_.hEvent = _hOverlapped;
	overlapped_.Offset = 0;
	overlapped_.OffsetHigh = 0;

    // Device interface detailed data
    boost::raw_scoped_ptr<SP_DEVICE_INTERFACE_DETAIL_DATA> detail_data;

    // Get a handle to a device information set that contains all the installed devices that match the specified parameters
    boost::shared_ptr<void> HidDevInfo(
        ::SetupDiGetClassDevs(&HidGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE),
        ::SetupDiDestroyDeviceInfoList);

    // size of the DeviceInterfaceDetail structure 
    DWORD dwDataSize;

    // Information structure for HID devices
    SP_DEVICE_INTERFACE_DATA devInfoData;
    // Set to the size of the structure that will contain the device info data
    devInfoData.cbSize = sizeof(devInfoData);	

    // Step through the attached device list 1 by 1 and examine each of the attached devices.
    // When there are no more entries in the array of structures, the function will return FALSE.
    for (DWORD dwIndex = 0 ; ::SetupDiEnumDeviceInterfaces(HidDevInfo.get(), 0, &HidGuid, dwIndex, &devInfoData) != FALSE ; dwIndex++)
    {
        if (!detail_data)
        {
            // Get the size of the DEVICE_INTERFACE_DETAIL_DATA
            // structure.  The first call will return an error condition ERROR_INSUFFICIENT_BUFFER, 
            // but we'll get the size of the strucure
            ::SetupDiGetDeviceInterfaceDetail(HidDevInfo.get(), &devInfoData, NULL, 0, &dwDataSize, NULL);

            // Allocate memory for the HidDevInfo structure
            detail_data.reset((PSP_DEVICE_INTERFACE_DETAIL_DATA) new (unsigned char[dwDataSize]));

            // Set the size parameter of the structure
            detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        }

        // Now call the function with the correct size parameter.  This function will return
        // data from one of the array members that last call pointed to. This way we can start to identify the
        // attributes of particular HID devices.
        ::SetupDiGetDeviceInterfaceDetail(HidDevInfo.get(), &devInfoData, detail_data.get(), dwDataSize, NULL, NULL);

        // Open a file handle to the device.  Make sure the attibutes specify overlapped transactions or the IN
        // transaction may block the input thread.
        HANDLE hid = ::CreateFile(detail_data->DevicePath, GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

        // Get the Device VID & PID to see if it's the device we want
        if(hid != INVALID_HANDLE_VALUE)
        {
            // HID device attributes
            HIDD_ATTRIBUTES HIDAttrib;
            HIDAttrib.Size = sizeof(HIDAttrib);
            HidD_GetAttributes(hid, &HIDAttrib);

            if((HIDAttrib.VendorID == _VendorId) && (HIDAttrib.ProductID == _ProductId))
            {
                hid_ = handle_t(hid, ::CloseHandle);
                return TRUE;	// found HID device
            }

            // Close the Device Handle because we didn't find the device
            // with the correct VID and PID
            ::CloseHandle(hid);
            hid = INVALID_HANDLE_VALUE;
        }
    }

    return FALSE;
}


HIDP_CAPS human_input_device::capabilities() const
{
    HID_PRECONDITION(hid_ != NULL);

    HIDP_CAPS capabilities;
    PHIDP_PREPARSED_DATA HidParsedData;

    // get a handle to a buffer that describes the device's capabilities.  This
    // line plus the following two lines of code extract the report length the
    // device is claiming to support
    ::HidD_GetPreparsedData(handle(), &HidParsedData);
	
    // Extract the capabilities info
    ::HidP_GetCaps(HidParsedData, &capabilities);
	
    // Free the memory allocated when getting the preparsed data
    ::HidD_FreePreparsedData(HidParsedData);

    return capabilities;
}


boost::shared_ptr<void> human_input_device::register_notification(HWND _hWnd) const
{
    // Get the HID Globally Unique ID from the OS
    static GUID HidGuid;
    HidD_GetHidGuid(&HidGuid);
    
    // Un/plug notification filter
    DEV_BROADCAST_DEVICEINTERFACE filter;
	memset(&filter, 0, sizeof(filter));
				
	// Assign the previously cleared structure with the correct data
	// so that the application is notified of HID device un/plug events
	filter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	filter.dbcc_classguid = HidGuid;

	/* 4) register device notifications for this application */
	//hDevNotify = RegisterDeviceNotification(_hWnd, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);
					
	return boost::shared_ptr<void>(RegisterDeviceNotification(_hWnd, &filter, DEVICE_NOTIFY_WINDOW_HANDLE), UnregisterDeviceNotification);
}