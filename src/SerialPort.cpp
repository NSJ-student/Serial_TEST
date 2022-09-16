/*
 * SerialPort.cpp
 *
 *  Created on: 2022. 9. 16.
 *      Author: Sujin
 */

#include "SerialPort.h"

SerialPort::SerialPort() {
	// TODO Auto-generated constructor stub

}

SerialPort::~SerialPort() {
	// TODO Auto-generated destructor stub
}

bool SerialPort::get_serial_ports()
{
//    GUID*    guidDev  = (GUID*) &GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR;
    GUID*    guidDev  = (GUID*) &GUID_DEVINTERFACE_COMPORT;
    HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;

    SP_DEVICE_INTERFACE_DETAIL_DATA* pDetData = NULL;

    hDevInfo = SetupDiGetClassDevs( guidDev,
                                    NULL,
                                    NULL,
                                    DIGCF_PRESENT | DIGCF_DEVICEINTERFACE );

    if( hDevInfo == INVALID_HANDLE_VALUE )
    {
        g_printerr("SetupDiGetClassDevs return INVALID_HANDLE_VALUE\n");
        return false;
    }

    BOOL                        bOk = TRUE;
    SP_DEVICE_INTERFACE_DATA    ifcData;
    DWORD                       dwDetDataSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + DEVICE_INFO_SZ;

    pDetData = (SP_DEVICE_INTERFACE_DETAIL_DATA*)new char[dwDetDataSize];

    if ( pDetData == NULL )
        return false;

    ifcData.cbSize   = sizeof(SP_DEVICE_INTERFACE_DATA);
    pDetData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

    for( int cnt=0; cnt<256; cnt++ )
    {
        bOk = SetupDiEnumDeviceInterfaces(hDevInfo, NULL, guidDev, cnt, &ifcData);
        if ( bOk == TRUE )
        {
            // Got a device. Get the details.
            SP_DEVINFO_DATA devdata = {sizeof(SP_DEVINFO_DATA)};

            bOk = SetupDiGetDeviceInterfaceDetail( hDevInfo,
                                                   &ifcData,
                                                   pDetData,
                                                   dwDetDataSize,
                                                   NULL,
                                                   &devdata );
            if ( bOk == TRUE )
            {
                char * strDevPath = pDetData->DevicePath;

                wchar_t fname[256] = {0};
                wchar_t desc[256] = {0};

                BOOL bSuccess = SetupDiGetDeviceRegistryProperty( hDevInfo,
                                                                  &devdata,
                                                                  SPDRP_FRIENDLYNAME,
                                                                  NULL,
                                                                  (PBYTE)fname,
                                                                  256,
                                                                  NULL );
                bSuccess = bSuccess &&
                           SetupDiGetDeviceRegistryProperty( hDevInfo,
                                                             &devdata,
                                                             SPDRP_DEVICEDESC,
                                                             NULL,
                                                             (PBYTE)desc,
                                                             256,
                                                             NULL );
                bool bUsbDevice = false;

                if ( ( wcsstr( desc, L"USB" ) != NULL ) || ( wcsstr( fname, L"USB" ) != NULL ) )
                {
                    bUsbDevice = true;
                }

                if ( bSuccess == TRUE )
                {
                	/*
                    using convert_typeX = std::codecvt_utf8<wchar_t>;
                    std::wstring_convert<convert_typeX, wchar_t> converterX;
                	std::string  strFname = converterX.to_bytes(fname);
                    std::cout << strFname << std::endl;
					*/

                	std::wstring  wstrFname1(L"TEST(COM32)TEST");
                	std::wstring  wstrFname(fname);
                	std::wstring::size_type postkf = wstrFname1.find(L"(");
                    std::wstring::size_type postke = wstrFname1.find_last_of(L")");
                    std::wstring::size_type lentk  = postke - postkf - 1;
                    std::wstring portn = wstrFname.substr( postkf + 1, lentk );

                    //std::wcout << wstrFname << std::endl;
                    //std::wcout << portn << postkf << postke << lentk << std::endl;

                	g_print( "%s[%03d] %s (%s) %d %d %d\n",
                             "COM Port",
                             cnt,
							 (wchar_t*)wstrFname.c_str(),
                             (wchar_t*)desc,
							 postkf,
							 postke,
							 wstrFname.length());


/*
                    serialportinfo si;

                    si.isUSBdev = bUsbDevice;
                    si.devpath  = ConvertFromUnicode( strDevPath.c_str() );
                    si.friendlyname = ConvertFromUnicode( fname );
                    si.description = ConvertFromUnicode( desc );

                    string::size_type postkf = si.friendlyname.find("(");
                    string::size_type postke = si.friendlyname.find_last_of(")");
                    string::size_type lentk  = postke - postkf - 1;
                    string portn = si.friendlyname.substr( postkf + 1, lentk );

                    si.portname = portn;

                    serialports.push_back( si );
                    */
                }

            }
            else
            {
                DWORD err = GetLastError();
            	g_printerr("SetupDiGetDeviceInterfaceDetail failed. err=%d\n", err);
                return false;
            }
        }
        else
        {
            DWORD err = GetLastError();

            if (err != ERROR_NO_MORE_ITEMS)
            {
            	g_printerr("SetupDiEnumDeviceInterfaces failed.\n");
                return false;
            }
            else
            {
            	break;
            }
        }
    }

	if (pDetData != NULL)
		delete [] (char*)pDetData;

	if (hDevInfo != INVALID_HANDLE_VALUE)
		SetupDiDestroyDeviceInfoList(hDevInfo);
}
