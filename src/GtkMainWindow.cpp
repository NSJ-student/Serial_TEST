/*
 * GtkMainWindow.cpp
 *
 *  Created on: 2022. 9. 16.
 *      Author: Sujin
 */

#include "GtkMainWindow.h"

static void on_serialport_refresh_clicked(GtkButton *btn, gpointer user_data);
static void on_serial_open_clicked(GtkButton *btn, gpointer user_data);
static void on_serial_close_clicked(GtkButton *btn, gpointer user_data);

/**********************************************/
//	Class Methods
/**********************************************/

GtkMainWindow::GtkMainWindow(GApplication *app)
{
    //////////////////////////////
    /////  main window
    //////////////////////////////
	gtk_window = gtk_application_window_new(GTK_APPLICATION(app));
	gtk_hpaned_container = gtk_hpaned_new();
	gtk_text_viewer = gtk_text_view_new ();
	gtk_notebook  = gtk_notebook_new();

	// set text viewer
    gtk_widget_set_size_request(GTK_WIDGET(gtk_text_viewer), 640, 480);
    // set control notebook
    gtk_notebook_set_tab_pos (GTK_NOTEBOOK(gtk_notebook), GTK_POS_TOP);

    // add to container
    gtk_container_add (GTK_CONTAINER (GTK_WIDGET(gtk_window)), GTK_WIDGET(gtk_hpaned_container));
    gtk_paned_add1 (GTK_PANED(gtk_hpaned_container), gtk_text_viewer);
    gtk_paned_add2 (GTK_PANED(gtk_hpaned_container), gtk_notebook);

//    g_signal_connect (gtk_hpaned_container, "button-release-event", G_CALLBACK(on_paned_handle_mouse_released), NULL);

    //////////////////////////////
    /////  page 1 (serial port)
    //////////////////////////////
    gtk_notebook_page1 = gtk_fixed_new();
	GtkWidget * label = gtk_label_new ("Serial");
    gtk_notebook_append_page (GTK_NOTEBOOK(gtk_notebook), gtk_notebook_page1, label);

    gtk_combo_port = gtk_combo_box_new();
    gtk_txt_baudrate = gtk_entry_new();
    gtk_btn_get_serialport = gtk_button_new_with_label("Refresh");


    gtk_btn_open_serial = gtk_button_new_with_label("Open");
    gtk_btn_close_serial = gtk_button_new_with_label("Close");

    g_signal_connect(gtk_btn_get_serialport, "clicked", G_CALLBACK(on_serialport_refresh_clicked), this);
    g_signal_connect(gtk_btn_open_serial, "clicked", G_CALLBACK(on_serial_open_clicked), this);
    g_signal_connect(gtk_btn_close_serial, "clicked", G_CALLBACK(on_serial_close_clicked), this);

    gtk_fixed_put(GTK_FIXED(gtk_notebook_page1), GTK_WIDGET(gtk_combo_port), 10, 10);
    gtk_fixed_put(GTK_FIXED(gtk_notebook_page1), GTK_WIDGET(gtk_btn_get_serialport), 80, 10);
    gtk_fixed_put(GTK_FIXED(gtk_notebook_page1), GTK_WIDGET(gtk_txt_baudrate), 10, 50);
    gtk_fixed_put(GTK_FIXED(gtk_notebook_page1), GTK_WIDGET(gtk_btn_open_serial), 10, 90);
    gtk_fixed_put(GTK_FIXED(gtk_notebook_page1), GTK_WIDGET(gtk_btn_close_serial), 80, 90);

    //////////////////////////////
    /////  page 2 (test)
    //////////////////////////////
    gtk_notebook_page2 = gtk_fixed_new();
	GtkWidget * label2 = gtk_label_new ("Test");
    gtk_notebook_append_page (GTK_NOTEBOOK(gtk_notebook), gtk_notebook_page2, label2);


    //////////////////////////////
    /////  display the gtk_window
    //////////////////////////////
    gtk_widget_show_all(GTK_WIDGET(gtk_window));
}

GtkMainWindow::~GtkMainWindow()
{
	// TODO Auto-generated destructor stub
}





/**********************************************/
//	callback functions
/**********************************************/

static void on_serialport_refresh_clicked(GtkButton *btn, gpointer user_data)
{
	GtkMainWindow * p_main = (GtkMainWindow *)user_data;

	if(p_main == NULL)
	{
		return;
	}

}

static void on_serial_open_clicked(GtkButton *btn, gpointer user_data)
{
	GtkMainWindow * p_main = (GtkMainWindow *)user_data;

	if(p_main == NULL)
	{
		return;
	}

}

static void on_serial_close_clicked(GtkButton *btn, gpointer user_data)
{
	GtkMainWindow * p_main = (GtkMainWindow *)user_data;

	if(p_main == NULL)
	{
		return;
	}

}
