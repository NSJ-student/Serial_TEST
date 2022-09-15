/*
 * main.cpp
 *
 *  Created on: 2022. 9. 14.
 *      Author: Sujin
 */

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkwin32.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>


GtkWidget * window;
GtkWidget * hpaned_container;
GtkWidget * camera_view;
GtkWidget * notebook;

GtkWidget * page1;
GtkWidget * txt_video_path;
GtkWidget * btn_play_video;
GtkWidget * page2;
GtkWidget * btn_play_camera;

GstElement * playbin;
GstElement * camerabin;
GstElement * video_sink;
GstElement * camera_sink;

/* playbin flags */
typedef enum {
  GST_PLAY_FLAG_VIDEO         = (1 << 0), /* We want video output */
  GST_PLAY_FLAG_AUDIO         = (1 << 1), /* We want audio output */
  GST_PLAY_FLAG_TEXT          = (1 << 2)  /* We want subtitle output */
} GstPlayFlags;

static GstBusSyncReply
_on_bus_message (GstBus * bus, GstMessage * message, void * data)
{
	GstObject *src = GST_MESSAGE_SRC (message);

	switch (GST_MESSAGE_TYPE (message)) {
	case GST_MESSAGE_ERROR:
	{
		GError *err = NULL;
		gchar *name = gst_object_get_path_string (GST_MESSAGE_SRC (message));
		gst_message_parse_error (message, &err, NULL);

		g_printerr ("ERROR: from element %s: %s\n", name, err->message);
		g_error_free (err);
		g_free (name);
		break;
	}
    case GST_MESSAGE_EOS:
		g_print ("EOS ! Stopping \n");
		break;
	case GST_MESSAGE_STREAM_COLLECTION:
	{
		break;
	}
	case GST_MESSAGE_STREAMS_SELECTED:
	{
		GstStreamCollection *collection = NULL;
		gst_message_parse_streams_selected (message, &collection);
		if (collection)
		{
			guint i, len;
			g_print ("Got a STREAMS_SELECTED message from %s (seqnum:%"
				G_GUINT32_FORMAT "):\n", src ? GST_OBJECT_NAME (src) : "unknown",
				GST_MESSAGE_SEQNUM (message));
			len = gst_message_streams_selected_get_size (message);
			for (i = 0; i < len; i++)
			{
				GstStream *stream = gst_message_streams_selected_get_stream (message, i);
				g_print ("  Stream #%d : %s\n", i, gst_stream_get_stream_id (stream));
				gst_object_unref (stream);
			}
			gst_object_unref (collection);
		}
		break;
	}
	default:
		break;
	}

  return GST_BUS_PASS;
}

static void init_gst_elements()
{
	GstBus *bus;
	GdkWindow *video_window_xwindow;
//	gulong embed_xid;
	HGDIOBJ embed_xid;

	g_print("Make playbin\n");
	playbin = gst_element_factory_make ("playbin", NULL);
	if(playbin == 0)
	{
		g_printerr ("Unable to make playbin.\n");
		return;
	}

	g_print("Make camerabin\n");
	camerabin = gst_element_factory_make ("camerabin", NULL);
	if(camerabin == 0)
	{
		g_printerr ("Unable to make camerabin.\n");
		return;
	}

	video_sink = gst_element_factory_make ("glimagesink", NULL);
	if(video_sink == 0)
	{
		g_printerr ("Unable to make video_sink.\n");
		return;
	}

	camera_sink = gst_element_factory_make ("glimagesink", NULL);
	if(camera_sink == 0)
	{
		g_printerr ("Unable to make camera_sink.\n");
		return;
	}

	video_window_xwindow = gtk_widget_get_window (camera_view);
//	embed_xid = gdk_x11_drawable_get_xid (video_window_xwindow);
	embed_xid = gdk_win32_window_get_handle (video_window_xwindow);
	gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(video_sink), (guintptr)embed_xid);
	gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(camera_sink), (guintptr)embed_xid);

	// Set the URI to play
	g_print("Set playbin sink\n");
//	g_object_set (playbin, "uri", "https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_cropped_multilingual.webm", NULL);
	g_object_set (playbin, "uri", "file:///D:\\Flower.mp4", NULL);
	g_object_set (playbin, "video-sink", video_sink, NULL);

	// Set the URI to play
	g_print("Set camerabin sink\n");
	g_object_set (camerabin, "viewfinder-sink", camera_sink, NULL);

	bus = gst_pipeline_get_bus (GST_PIPELINE (playbin));
	gst_bus_set_sync_handler (bus, (GstBusSyncHandler) _on_bus_message, NULL, NULL);


	g_print("End\n");
}

static void start_stop_playbin()
{
	GstStateChangeReturn ret;
    GstState cur_state;
    GstState camera_state;

    gst_element_get_state(playbin, &cur_state, NULL, GST_CLOCK_TIME_NONE);
    if(cur_state == GST_STATE_PLAYING)
    {
    	g_print("Stop Video\n");
    	ret = gst_element_set_state (playbin, GST_STATE_NULL);
    	if (ret == GST_STATE_CHANGE_FAILURE)
    	{
    		g_printerr ("Unable to set the playbin to the NULL state.\n");
    		return;
    	}

        gtk_button_set_label(GTK_BUTTON(btn_play_video), "Play");
    }
    else
    {
        gst_element_get_state(camerabin, &camera_state, NULL, GST_CLOCK_TIME_NONE);
        if(camera_state == GST_STATE_PLAYING)
        {
        	ret = gst_element_set_state (camerabin, GST_STATE_NULL);
        	if (ret == GST_STATE_CHANGE_FAILURE)
        	{
        		g_printerr ("Unable to set the camerabin to the NULL state.\n");
        		return;
        	}
        }

        char * text_input = gtk_entry_get_text(GTK_ENTRY(txt_video_path));

    	g_print("Start Video - %s\n", text_input);
    	ret = gst_element_set_state (playbin, GST_STATE_PLAYING);
    	if (ret == GST_STATE_CHANGE_FAILURE)
    	{
    		g_printerr ("Unable to set the playbin to the playing state.\n");
    		return;
    	}

        gtk_button_set_label(GTK_BUTTON(btn_play_video), "Stop");
    }
}

static void start_stop_camerabin()
{
	GstStateChangeReturn ret;
    GstState cur_state;
    GstState video_state;

    gst_element_get_state(camerabin, &cur_state, NULL, GST_CLOCK_TIME_NONE);
    if(cur_state == GST_STATE_PLAYING)
    {
    	g_print("Stop Camera\n");
    	ret = gst_element_set_state (camerabin, GST_STATE_NULL);
    	if (ret == GST_STATE_CHANGE_FAILURE)
    	{
    		g_printerr ("Unable to set the camerabin to the playing state.\n");
    		return;
    	}

        gtk_button_set_label(GTK_BUTTON(btn_play_camera), "Play");
    }
    else
    {
        gst_element_get_state(playbin, &video_state, NULL, GST_CLOCK_TIME_NONE);
        if(video_state == GST_STATE_PLAYING)
        {
        	ret = gst_element_set_state (playbin, GST_STATE_NULL);
        	if (ret == GST_STATE_CHANGE_FAILURE)
        	{
        		g_printerr ("Unable to set the playbin to the NULL state.\n");
        		return;
        	}
        }

    	g_print("Start Camera\n");
    	ret = gst_element_set_state (camerabin, GST_STATE_PLAYING);
    	if (ret == GST_STATE_CHANGE_FAILURE)
    	{
    		g_printerr ("Unable to set the camerabin to the playing state.\n");
    		return;
    	}

        gtk_button_set_label(GTK_BUTTON(btn_play_camera), "Stop");
    }
}

// callback function which is called when button is clicked
static void on_playbin_clicked(GtkButton *btn, gpointer data) {
    // change button label when it's clicked
	start_stop_playbin();
}

static void on_camerabin_clicked(GtkButton *btn, gpointer data) {
    // change button label when it's clicked
	start_stop_camerabin();
}

static void init_gui(GApplication * app)
{
    // create a new application window for the application
    // GtkApplication is sub-class of GApplication
    // downcast GApplication* to GtkApplication* with GTK_APPLICATION() macro
	window = gtk_application_window_new(GTK_APPLICATION(app));
	hpaned_container = gtk_hpaned_new();
	camera_view = gtk_drawing_area_new ();
	notebook  = gtk_notebook_new();

    gtk_container_add (GTK_CONTAINER (GTK_WIDGET(window)), GTK_WIDGET(hpaned_container));
    gtk_notebook_set_tab_pos (GTK_NOTEBOOK(notebook), GTK_POS_TOP);

    gtk_paned_add1 (GTK_PANED(hpaned_container), camera_view);
    gtk_paned_add2 (GTK_PANED(hpaned_container), notebook);

    GdkColor		color;
    gdk_color_parse("black", &color);
    gtk_widget_modify_bg(camera_view, GTK_STATE_NORMAL, &color);
    gtk_widget_set_size_request(GTK_WIDGET(camera_view), 640, 480);

    //////////////////////////////
    /////  page 1 (video)
    //////////////////////////////
    page1 = gtk_fixed_new();
	GtkWidget * label = gtk_label_new ("Video");
    gtk_notebook_append_page (GTK_NOTEBOOK(notebook), page1, label);

    txt_video_path = gtk_entry_new();
    btn_play_video = gtk_button_new_with_label("Play");

    g_signal_connect(btn_play_video, "clicked", G_CALLBACK(on_playbin_clicked), NULL);

    gtk_fixed_put(GTK_FIXED(page1), GTK_WIDGET(txt_video_path), 10, 10);
    gtk_fixed_put(GTK_FIXED(page1), GTK_WIDGET(btn_play_video), 10, 50);

    //////////////////////////////
    /////  page 2 (camera)
    //////////////////////////////
    page2 = gtk_fixed_new();
	GtkWidget * label2 = gtk_label_new ("Camera");
    gtk_notebook_append_page (GTK_NOTEBOOK(notebook), page2, label2);

    btn_play_camera = gtk_button_new_with_label("Play");

    g_signal_connect(btn_play_camera, "clicked", G_CALLBACK(on_camerabin_clicked), NULL);

    gtk_fixed_put(GTK_FIXED(page2), GTK_WIDGET(btn_play_camera), 10, 10);

    // display the window
    gtk_widget_show_all(GTK_WIDGET(window));
}

// callback function which is called when application is first started
static void on_app_activate(GApplication *app, gpointer data) {
	g_print("App activated\n");
    init_gui(app);
    init_gst_elements();
	g_print("App activated finish\n");
}

int main(int argc, char *argv[]) {
    // create new GtkApplication with an unique application ID
    GtkApplication *app = gtk_application_new(
        "org.gtkmm.example.libVLC_TEST",
        G_APPLICATION_FLAGS_NONE
    );

    // Initialize GStreamer
    gst_init(NULL, NULL);

    // connect the event-handler for "activate" signal of GApplication
    // G_CALLBACK() macro is used to cast the callback function pointer
    // to generic void pointer
    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);
    // start the application, terminate by closing the window
    // GtkApplication* is upcast to GApplication* with G_APPLICATION() macro
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    // deallocate the application object
    g_object_unref(app);
    return status;
}
