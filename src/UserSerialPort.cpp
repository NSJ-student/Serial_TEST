/*
 * UserSerialPort.cpp
 *
 *  Created on: 2022. 9. 16.
 *      Author: Sujin
 */

#include "UserSerialPort.h"

static gpointer serial_rx_process(gpointer user_data);

/**********************************************/
//	Class Methods
/**********************************************/

UserSerialPort::UserSerialPort() : hSerial(INVALID_HANDLE_VALUE)
{
	thread_data.user_data = 0;
	thread_data.fp_print_rx = 0;
	thread_data.running = true;
	thread_data.rx_processing = false;
	thread_data.handle = hSerial;
	thread_data.data_mutex = g_mutex_new();
	thread_data.serial_mutex = g_mutex_new();

	p_thread_serial_rx = g_thread_new("serial_rx", serial_rx_process, &thread_data);
	if(p_thread_serial_rx == 0)
	{
		g_printerr("Error on pthread_create\n");
	}

}

UserSerialPort::~UserSerialPort()
{
	close_serial_port();

	if(thread_data.rx_processing)
	{
		thread_data.rx_processing = false;
		g_thread_join (p_thread_serial_rx);
	}

	g_mutex_free(thread_data.data_mutex);
	g_mutex_free(thread_data.serial_mutex);
}

void UserSerialPort::set_serial_rx_handler(void *parent, void (*fp)(void * user_data))
{
	thread_data.fp_print_rx = fp;
	thread_data.user_data = parent;
}

gboolean UserSerialPort::get_serial_ports(std::vector<std::string> &ports)
{
//    GUID*    guidDev  = (GUID*) &GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR;
    GUID*    guidDev  = (GUID*) &GUID_DEVINTERFACE_COMPORT;
    HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;

    SP_DEVICE_INTERFACE_DETAIL_DATA* pDetData = NULL;

    ports.clear();
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
//                char * strDevPath = pDetData->DevicePath;

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
                	std::string  wstrFname((char *)fname);
                	std::string::size_type postkf = wstrFname.find_last_of("(");
                    std::string::size_type postke = wstrFname.find_last_of(")");
                    std::string::size_type lentk  = postke - postkf - 1;
                    std::string portn = wstrFname.substr( postkf + 1, lentk );

                    if(!portn.compare("COM1"))
                    {
                    	continue;
                    }
                    ports.push_back(portn);
                    std::cout << portn << std::endl;
                	g_print( "%s[%03d] %s (%s) USB(%s)\n",
                             "COM Port",
                             cnt,
							 (wchar_t*)wstrFname.c_str(),
                             (wchar_t*)desc,
							 bUsbDevice?"O":"X");

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

	return true;
}

gboolean UserSerialPort::open_serial_port(const char *port, gint baudrate)
{
    DCB dcbSerialParams = { 0 };  // Initializing DCB structure
    COMMTIMEOUTS timeouts = { 0 };  //Initializing timeouts structure
    char PortNo[20] = { 0 }; //contain friendly name
    gboolean   Status; // Status

    sprintf_s(PortNo, "\\\\.\\%s", port);
    //Open the serial com port
    hSerial = CreateFile(PortNo,			//friendly name
                       GENERIC_READ | GENERIC_WRITE,	// Read/Write Access
                       0,				// No Sharing, ports cant be shared
                       NULL,			// No Security
                       OPEN_EXISTING,	// Open existing port only
                       0,				// Non Overlapped I/O
                       NULL);			// Null for Comm Devices
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        g_printerr("Port can't be opened\n");
        return false;
    }

    //Setting the Parameters for the UserSerialPort
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    Status = GetCommState(hSerial, &dcbSerialParams); //retreives  the current settings
    if (Status == FALSE)
    {
        CloseHandle(hSerial);//Closing the Serial Port
    	g_printerr("Error to Get the Com state\n");
        return false;
    }
    dcbSerialParams.BaudRate = baudrate;	//BaudRate
    dcbSerialParams.ByteSize = 8;			//ByteSize = 8
    dcbSerialParams.StopBits = ONESTOPBIT;	//StopBits = 1
    dcbSerialParams.Parity = NOPARITY;		//Parity = None
    Status = SetCommState(hSerial, &dcbSerialParams);
    if (Status == FALSE)
    {
        CloseHandle(hSerial);//Closing the Serial Port
    	g_printerr("Error to Setting DCB Structure\n");
    	return false;
    }
    //Setting Timeouts
    timeouts.ReadIntervalTimeout = 1;
    timeouts.ReadTotalTimeoutConstant = 1;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 1;
    if (SetCommTimeouts(hSerial, &timeouts) == FALSE)
    {
        CloseHandle(hSerial);//Closing the Serial Port
    	g_printerr("Error to Setting Time outs");
    	return false;
    }

    //Setting Receive Mask
    Status = SetCommMask(hSerial, EV_RXCHAR|EV_BREAK|EV_ERR);
    if (Status == FALSE)
    {
    	g_printerr("Error to in Setting CommMask\n");
        return false;
    }

	thread_data.handle = hSerial;
	thread_data.rx_processing = true;
    g_print("open %s\n", PortNo);
    return true;
}

gboolean UserSerialPort::close_serial_port()
{
	if(hSerial == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	thread_data.rx_processing = false;

	//Closing the Serial Port
	g_mutex_lock(thread_data.serial_mutex);
	bool result = CloseHandle(hSerial);
	hSerial = INVALID_HANDLE_VALUE;
	thread_data.data_queue = std::queue<char>();
    g_mutex_unlock(thread_data.serial_mutex);

    return result;
}

gboolean UserSerialPort::is_serial_port_open()
{
	if (hSerial == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	return true;
}

gint UserSerialPort::get_rx_size()
{
	if(!is_serial_port_open())
	{
		return 0;
	}

	return thread_data.data_queue.size();
}

char UserSerialPort::get_rx_data()
{
	if(get_rx_size() == 0)
	{
		return 0;
	}

	char data = thread_data.data_queue.front();
	thread_data.data_queue.pop();

	return data;
}

gboolean UserSerialPort::read_data(char *read_buff, gint read_size, gint *bytes_read)
{
	gint size = get_rx_size();

	if (hSerial == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	if (read_size == 0)
	{
		return false;
	}

	if(size < read_size)
	{
		int i = 0;
		g_mutex_lock(thread_data.data_mutex);
		while(get_rx_size() > 0)
		{
			read_buff[i] = get_rx_data();
			i++;
		}

		if(bytes_read)	*bytes_read = i;

		g_mutex_unlock(thread_data.data_mutex);
		return true;
	}
	else
	{
		int i = 0;
		g_mutex_lock(thread_data.data_mutex);
		while(get_rx_size() > 0)
		{
			read_buff[i] = get_rx_data();
			i++;
			if(i >= read_size)
			{
				break;
			}
		}

		if(bytes_read)	*bytes_read = i;

		g_mutex_unlock(thread_data.data_mutex);
		return true;
	}
}

gboolean UserSerialPort::write_data(const char *write_buff, gint write_size, gint *bytes_written)
{
    gboolean   Status;
    DWORD written;

	if (hSerial == INVALID_HANDLE_VALUE)
	{
		return false;
	}

    //Writing data to Serial Port
	g_mutex_lock(thread_data.serial_mutex);
    Status = WriteFile(hSerial,// Handle to the Serialport
    					write_buff,            // Data to be written to the port
						write_size,   // No of bytes to write into the port
						&written,  // No of bytes written to the port
						NULL);
    g_mutex_unlock(thread_data.serial_mutex);
    if (Status == FALSE)
    {
        printf_s("Fail to Written");
        return false;
    }

    *bytes_written = written;
    return true;
}

/**********************************************/
//	callback functions
/**********************************************/

static gpointer serial_rx_process(gpointer user_data)
{
	serialMsg_t * p_msg = (serialMsg_t *)user_data;
	gboolean   Status;
	gint debug_count = 0;
//	DWORD dwEventMask;     // Event mask to trigger
	char  ReadData[256];        // temperory Character
	DWORD NoBytesRead;     // Bytes read by ReadFile()

	while(p_msg->running)
	{
		debug_count++;

		if(p_msg->rx_processing)
		{
		    /*
			g_print("WaitCommEvent\n");
		    //Setting WaitComm() Event
			g_mutex_lock(p_msg->serial_mutex);
			g_print("WaitCommEvent start\n");
		    Status = WaitCommEvent(p_msg->handle, &dwEventMask, NULL); //Wait for the character to be received
			g_print("WaitCommEvent end\n");
		    g_mutex_unlock(p_msg->serial_mutex);
		    if (Status == FALSE)
		    {
				g_print("WaitCommEvent fail\n");
		        continue;
		    }

			g_print("event = 0x%X\n", dwEventMask);
		    if ((dwEventMask & EV_RXCHAR) != EV_RXCHAR)
		    {
		    	continue;
		    }
			 */
		    do
		    {
			    if (p_msg->handle == INVALID_HANDLE_VALUE)
			    {
					break;
			    }

			    NoBytesRead = 0;
				g_mutex_lock(p_msg->serial_mutex);
		        Status = ReadFile(p_msg->handle, ReadData, sizeof(ReadData), &NoBytesRead, NULL);
			    g_mutex_unlock(p_msg->serial_mutex);
			    if(Status==FALSE)
			    {
			    	break;
			    }

			    if(NoBytesRead > 0)
			    {
					g_mutex_lock(p_msg->data_mutex);
					for(int i=0; i<NoBytesRead; i++)
					{
				        p_msg->data_queue.push(ReadData[i]);
					}
				    g_mutex_unlock(p_msg->data_mutex);
			    }
		    } while (NoBytesRead > 0);

			g_usleep (100);
		    /*
			if(p_msg->fp_print_rx && p_msg->user_data)
			{
				//g_print("Print\n");
				p_msg->fp_print_rx(p_msg->user_data);
			}
			*/
		}

		g_usleep (1000);
	}

	g_print("thread finish\n");
	return 0;
}
