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

typedef gboolean (*def_callback_t)(void);
typedef gboolean (*check_callback_t)(void);

class GtkMainWindow {
public:
	GtkMainWindow(GApplication *app);
	virtual ~GtkMainWindow();


private:
	GtkWidget * gtk_window;
	GtkWidget * gtk_hpaned_container;
	GtkWidget * gtk_text_viewer;
	GtkWidget * gtk_notebook;

	GtkWidget * gtk_notebook_page1;
	GtkWidget * gtk_combo_port;
	GtkWidget * gtk_btn_get_serialport;
	GtkWidget * gtk_txt_baudrate;
	GtkWidget * gtk_btn_open_serial;
	GtkWidget * gtk_btn_close_serial;

	GtkWidget * gtk_notebook_page2;

};

#endif /* SRC_GTKMAINWINDOW_H_ */
