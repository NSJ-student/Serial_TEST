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
static void on_serial_data_received(gpointer user_data);
static gboolean on_serial_write_input_pressed(GtkWidget *widget, GdkEventKey *event, gpointer user_data);

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
	gtk_vbox = gtk_vbox_new (false, 5);
	gtk_scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_text_viewer = gtk_text_view_new ();
	gtk_txt_input = gtk_entry_new();
	gtk_notebook  = gtk_notebook_new();

	// set text viewer
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (gtk_scroll),
                           GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_widget_set_size_request(GTK_WIDGET(gtk_scroll), 340, 480);
    gtk_container_add (GTK_CONTAINER (gtk_scroll), gtk_text_viewer);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(gtk_text_viewer), false);
    // set vbox
    gtk_box_pack_start (GTK_BOX (gtk_vbox), gtk_scroll, true, true, 0);
    gtk_box_pack_start (GTK_BOX (gtk_vbox), gtk_txt_input, false, false, 0);
    // set control notebook
    gtk_widget_set_size_request(GTK_WIDGET(gtk_notebook), 300, 480);
    gtk_notebook_set_tab_pos (GTK_NOTEBOOK(gtk_notebook), GTK_POS_TOP);

    // add to container
    gtk_container_add (GTK_CONTAINER (GTK_WIDGET(gtk_window)), GTK_WIDGET(gtk_hpaned_container));
    gtk_paned_add1 (GTK_PANED(gtk_hpaned_container), gtk_vbox);
    gtk_paned_add2 (GTK_PANED(gtk_hpaned_container), gtk_notebook);

    g_signal_connect(gtk_txt_input, "key-release-event", G_CALLBACK(on_serial_write_input_pressed), this);
//    g_signal_connect (gtk_hpaned_container, "button-release-event", G_CALLBACK(on_paned_handle_mouse_released), NULL);

    //////////////////////////////
    /////  page 1 (serial port)
    //////////////////////////////
    gtk_notebook_page1 = gtk_fixed_new();
	GtkWidget * label = gtk_label_new ("Serial");
    gtk_notebook_append_page (GTK_NOTEBOOK(gtk_notebook), gtk_notebook_page1, label);

    gtk_combo_port = gtk_combo_box_text_new_with_entry();
    gtk_txt_baudrate = gtk_entry_new();
    gtk_btn_get_serialport = gtk_button_new_with_label("Refresh");

    gtk_btn_open_serial = gtk_button_new_with_label("Open");
    gtk_btn_close_serial = gtk_button_new_with_label("Close");

    g_signal_connect(gtk_btn_get_serialport, "clicked", G_CALLBACK(on_serialport_refresh_clicked), this);
    g_signal_connect(gtk_btn_open_serial, "clicked", G_CALLBACK(on_serial_open_clicked), this);
    g_signal_connect(gtk_btn_close_serial, "clicked", G_CALLBACK(on_serial_close_clicked), this);

    gtk_fixed_put(GTK_FIXED(gtk_notebook_page1), GTK_WIDGET(gtk_combo_port), 10, 20);
//    gtk_widget_set_size_request(GTK_WIDGET(gtk_combo_port), 200, 30);
    gtk_fixed_put(GTK_FIXED(gtk_notebook_page1), GTK_WIDGET(gtk_btn_get_serialport), 220, 20);
    gtk_fixed_put(GTK_FIXED(gtk_notebook_page1), GTK_WIDGET(gtk_txt_baudrate), 10, 60);
    gtk_entry_set_text(GTK_ENTRY(gtk_txt_baudrate), "460800");
//    gtk_widget_set_size_request(GTK_WIDGET(gtk_txt_baudrate), 200, 30);
    gtk_fixed_put(GTK_FIXED(gtk_notebook_page1), GTK_WIDGET(gtk_btn_open_serial), 10, 100);
    gtk_fixed_put(GTK_FIXED(gtk_notebook_page1), GTK_WIDGET(gtk_btn_close_serial), 80, 100);

    gtk_widget_set_sensitive (gtk_btn_close_serial, FALSE);

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


void GtkMainWindow::set_serial_instance(SerialPort * serial)
{
	inst_serial = serial;
	inst_serial->set_serial_rx_handler(this, on_serial_data_received);
}

gboolean GtkMainWindow::refresh_serial_ports()
{
	if(inst_serial == 0)
	{
		return false;
	}

	std::vector<std::string> serial_ports;
	gboolean result = inst_serial->get_serial_ports(serial_ports);
	if(!result)
	{
		return false;
	}

	gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(gtk_combo_port));
	guint64 port_count = serial_ports.size();
    for (guint64 i=0; i<port_count;i++)
    {
    	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(gtk_combo_port), NULL, serial_ports.back().c_str());
    	serial_ports.pop_back();
    }

	return true;
}

gboolean GtkMainWindow::open_serial_ports()
{
	if(inst_serial == 0)
	{
		return false;
	}

	char * serial_port = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(gtk_combo_port));
	const char * serial_baudrate = gtk_entry_get_text(GTK_ENTRY(gtk_txt_baudrate));

	if(serial_baudrate[0] == 0)
	{
	    g_free (serial_port);
	    g_printerr("baudrate is empty\n");
		return false;
	}

	int baudrate = std::stoi(serial_baudrate);
	if(baudrate == 0)
	{
	    g_free (serial_port);
		g_printerr("baudrate is 0\n");
		return false;
	}

	if(!inst_serial->open_serial_port(serial_port, baudrate))
	{
	    g_free (serial_port);
	    g_printerr("fail to open serial\n");
		return false;
	}

    gtk_widget_set_sensitive (gtk_btn_open_serial, FALSE);
    gtk_widget_set_sensitive (gtk_btn_close_serial, TRUE);

    g_free (serial_port);
	return true;
}

gboolean GtkMainWindow::close_serial_ports()
{
	if(inst_serial == 0)
	{
		return false;
	}

	if(inst_serial->close_serial_port())
	{
	    g_printerr("fail to close serial\n");
		return false;
	}

    gtk_widget_set_sensitive (gtk_btn_open_serial, TRUE);
    gtk_widget_set_sensitive (gtk_btn_close_serial, FALSE);

	return true;
}

gboolean GtkMainWindow::read_serial_data()
{
	if(inst_serial->get_rx_size() == 0)
	{
		return false;
	}

	char temp_buff[256] = {0};
	gint bytes_read;
	gboolean result = inst_serial->read_data(temp_buff, 255, &bytes_read);
	if(!result)
	{
		return false;
	}

//	std::string str_read(temp_buff);
	GtkTextBuffer * gtk_text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_text_viewer));
	GtkTextIter end;
	gtk_text_buffer_get_end_iter (gtk_text_buffer, &end);
	gtk_text_buffer_insert(gtk_text_buffer, &end, temp_buff, bytes_read);

	return true;
}

gboolean GtkMainWindow::write_serial_data()
{
	if(inst_serial == 0)
	{
		return false;
	}

	const char * txt_input = gtk_entry_get_text(GTK_ENTRY(gtk_txt_input));
	if(txt_input == 0)
	{
		g_printerr("text input is empty\n");
		return false;
	}

    char temp_buff[145] = { 0 };

    gint length = sprintf_s(temp_buff, "%s\r\n", txt_input);
	gint written;
	if(inst_serial->write_data(temp_buff, length, &written))
	{
	    g_printerr("fail to write serial\n");
		return false;
	}

	return true;
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

	p_main->refresh_serial_ports();
}

static void on_serial_open_clicked(GtkButton *btn, gpointer user_data)
{
	GtkMainWindow * p_main = (GtkMainWindow *)user_data;

	if(p_main == NULL)
	{
		return;
	}

	p_main->open_serial_ports();
}

static void on_serial_close_clicked(GtkButton *btn, gpointer user_data)
{
	GtkMainWindow * p_main = (GtkMainWindow *)user_data;

	if(p_main == NULL)
	{
		return;
	}

	p_main->close_serial_ports();
}

static void on_serial_data_received(gpointer user_data)
{
	GtkMainWindow * p_main = (GtkMainWindow *)user_data;

	if(p_main == NULL)
	{
		return;
	}

	p_main->read_serial_data();
}

static gboolean on_serial_write_input_pressed(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	GtkMainWindow * p_main = (GtkMainWindow *)user_data;

	if(p_main == NULL)
	{
		return true;
	}

	if(event->keyval == GDK_KEY_Return)
	{
		p_main->write_serial_data();
	}

	return true;
}
