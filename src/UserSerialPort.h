/*
 * UserSerialPort.h
 *
 *  Created on: 2022. 9. 16.
 *      Author: Sujin
 */

#ifndef SRC_USERSERIALPORT_H_
#define SRC_USERSERIALPORT_H_

#include <gtk/gtk.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <queue>

#ifdef WIN32
#include <winsock2.h>
#include <locale>
#include <codecvt>
#include <Windows.h>
#include <setupapi.h>
#include <initguid.h>
#include <winioctl.h>

#define DEVICE_INFO_SZ 	100
#else
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h>
#include <unistd.h>

#define INVALID_HANDLE_VALUE	-1
#endif

typedef struct _SerialMsg
{
	GMutex *data_mutex;
	GMutex *serial_mutex;
#ifdef WIN32
	HANDLE handle;
#else
    int handle;
#endif
	bool running;
	bool rx_processing;
	std::queue<char> data_queue;

	void * user_data;
	void (*fp_print_rx)(gpointer user_data);
} serialMsg_t;

class UserSerialPort {
public:
	UserSerialPort();
	virtual ~UserSerialPort();

	void set_serial_rx_handler(void * parent, void (*fp)(gpointer user_data));
	gboolean get_serial_ports(std::vector<std::string> &ports);
	gboolean open_serial_port(const char *port, gint baudrate);
	gboolean close_serial_port();
	gboolean is_serial_port_open();
	gint get_rx_size();
	gboolean read_data(char * read_buff, gint read_size, gint * bytes_read);
	gboolean write_data(const char * write_buff, gint write_size, gint * bytes_written);

private:
#ifdef WIN32
    HANDLE hSerial;  // Handle to the Serial port
#else
    int hSerial;
    struct termios oldConf;
#endif

    GThread * p_thread_serial_rx;
    serialMsg_t thread_data;

	char get_rx_data();
#ifndef WIN32
	guint baudrate_to_c_cflag(guint baudrate);
#endif
};

#endif /* SRC_USERSERIALPORT_H_ */
