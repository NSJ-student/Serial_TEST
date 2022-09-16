/*
 * SerialPort.h
 *
 *  Created on: 2022. 9. 16.
 *      Author: Sujin
 */

#ifndef SRC_SERIALPORT_H_
#define SRC_SERIALPORT_H_

#include <gtk/gtk.h>
#include <iostream>
#include <locale>
#include <codecvt>
#include <windows.h>
#include <string>

#ifdef WIN32
#include "termiWin/termios.h"
#include <setupapi.h>
#include <initguid.h>
#include <winioctl.h>

#define DEVICE_INFO_SZ 	100
#else
#include <termios.h>
#endif

class SerialPort {
public:
	SerialPort();
	virtual ~SerialPort();

	bool get_serial_ports();
};

#endif /* SRC_SERIALPORT_H_ */
