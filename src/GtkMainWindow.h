/*
 * GtkMainWindow.h
 *
 *  Created on: 2022. 9. 16.
 *      Author: Sujin
 */

#ifndef SRC_GTKMAINWINDOW_H_
#define SRC_GTKMAINWINDOW_H_

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkwin32.h>

#include "SerialPort.h"

typedef gboolean (*def_callback_t)(void);
typedef gboolean (*check_callback_t)(void);

class GtkMainWindow {
public:
	GtkMainWindow(GApplication *app);
	virtual ~GtkMainWindow();

	/***********************/
	//	Serial function
	/***********************/
	void set_serial_instance(SerialPort * serial);
	gboolean refresh_serial_ports();
	gboolean open_serial_ports();
	gboolean close_serial_ports();
	gboolean read_serial_data();
	gboolean write_serial_data();

	/***********************/
	//	UI function
	/***********************/
	void gtk_text_view_scroll_to_bottom();
	void gtk_text_view_clear();

private:
	GtkWidget * gtk_window;
	GtkWidget * gtk_hpaned_container;
	GtkWidget * gtk_vbox;
	GtkWidget * gtk_btn_clear_text_viewer;
	GtkWidget * gtk_scroll;
	GtkWidget * gtk_text_viewer;
	GtkWidget * gtk_txt_input;
	GtkWidget * gtk_notebook;

	GtkWidget * gtk_notebook_page1;
    GtkWidget * gtk_grid;
	GtkWidget * gtk_combo_port;
	GtkWidget * gtk_btn_get_serialport;
	GtkWidget * gtk_txt_baudrate;
	GtkWidget * gtk_btn_open_serial;
	GtkWidget * gtk_btn_close_serial;

	GtkWidget * gtk_notebook_page2;

	SerialPort * inst_serial;
	gint serial_rx_timer_id;

};

#endif /* SRC_GTKMAINWINDOW_H_ */
