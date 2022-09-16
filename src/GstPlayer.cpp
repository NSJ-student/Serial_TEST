/*
 * GstPlayer.cpp
 *
 *  Created on: 2022. 9. 15.
 *      Author: Sujin
 */

#include "GstPlayer.h"

static GstBusSyncReply _on_bus_message (GstBus * bus, GstMessage * message, void * data);

/**********************************************/
//	Class Methods
/**********************************************/

GstPlayer::GstPlayer()
{
	GstBus *bus;

	g_print("Make gst_playbin\n");
	gst_playbin = gst_element_factory_make ("playbin", NULL);
	if(gst_playbin == 0)
	{
		g_printerr ("Unable to make playbin.\n");
		return;
	}

	g_print("Make gst_camerabin\n");
	gst_camerabin = gst_element_factory_make ("camerabin", NULL);
	if(gst_camerabin == 0)
	{
		g_printerr ("Unable to make camerabin.\n");
		return;
	}

	gst_video_sink = gst_element_factory_make ("glimagesink", NULL);
	if(gst_video_sink == 0)
	{
		g_printerr ("Unable to make gst_video_sink.\n");
		return;
	}

	gst_camera_sink = gst_element_factory_make ("glimagesink", NULL);
	if(gst_camera_sink == 0)
	{
		g_printerr ("Unable to make gst_camera_sink.\n");
		return;
	}

	// Set the URI to play
	g_print("Set gst_playbin sink\n");
//	g_object_set (gst_playbin, "uri", "https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_cropped_multilingual.webm", NULL);
	g_object_set (gst_playbin, "uri", "file:///D:\\Flower.mp4", NULL);
	g_object_set (gst_playbin, "video-sink", gst_video_sink, NULL);

	// Set the URI to play
	g_print("Set gst_camerabin sink\n");
	g_object_set (gst_camerabin, "viewfinder-sink", gst_camera_sink, NULL);

	bus = gst_pipeline_get_bus (GST_PIPELINE (gst_playbin));
	gst_bus_set_sync_handler (bus, (GstBusSyncHandler) _on_bus_message, NULL, NULL);


	g_print("End\n");

}

GstPlayer::~GstPlayer() {
	// TODO Auto-generated destructor stub
}

void GstPlayer::set_video_sink(GdkWindow *sink)
{
//	gulong embed_xid;
	HGDIOBJ embed_xid;

	embed_xid = gdk_win32_window_get_handle (sink);
	gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(gst_video_sink), (guintptr)embed_xid);
}

void GstPlayer::set_camera_sink(GdkWindow *sink)
{
//	gulong embed_xid;
	HGDIOBJ embed_xid;

	embed_xid = gdk_win32_window_get_handle (sink);
	gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(gst_camera_sink), (guintptr)embed_xid);
}

gboolean GstPlayer::is_video_playing()
{
    GstState cur_state;

    gst_element_get_state(gst_playbin, &cur_state, NULL, GST_CLOCK_TIME_NONE);
    if(cur_state == GST_STATE_PLAYING)
    {
    	return true;
    }
    else
    {
    	return false;
    }
}

gboolean GstPlayer::is_camera_playing()
{
    GstState cur_state;

    gst_element_get_state(gst_camerabin, &cur_state, NULL, GST_CLOCK_TIME_NONE);
    if(cur_state == GST_STATE_PLAYING)
    {
    	return true;
    }
    else
    {
    	return false;
    }
}

gboolean GstPlayer::play_video(const char * path)
{
	GstStateChangeReturn ret;
	gboolean ret2;

	if(is_video_playing())
	{
		ret2 = stop_video();
		if(!ret2) { return false; }
	}
	if(is_camera_playing())
	{
		ret2 = stop_camera();
		if(!ret2) { return false; }
	}

	g_object_set (gst_playbin, "uri", path, NULL);
	ret = gst_element_set_state (gst_playbin, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE)
	{
		g_printerr ("Unable to set the gst_playbin to the playing state.\n");
		return false;
	}

	return true;
}

gboolean GstPlayer::stop_video()
{
	GstStateChangeReturn ret;

	ret = gst_element_set_state (gst_playbin, GST_STATE_NULL);
	if (ret == GST_STATE_CHANGE_FAILURE)
	{
		g_printerr ("Unable to set the gst_playbin to the NULL state.\n");
		return false;
	}

	return true;
}

gboolean GstPlayer::play_camera()
{
	GstStateChangeReturn ret;
	gboolean ret2;

	if(is_video_playing())
	{
		ret2 = stop_video();
		if(!ret2) { return false; }
	}
	if(is_camera_playing())
	{
		ret2 = stop_camera();
		if(!ret2) { return false; }
	}

	ret = gst_element_set_state (gst_camerabin, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE)
	{
		g_printerr ("Unable to set the gst_camerabin to the playing state.\n");
		return false;
	}

	return true;
}
gboolean GstPlayer::stop_camera()
{
	GstStateChangeReturn ret;

	ret = gst_element_set_state (gst_camerabin, GST_STATE_NULL);
	if (ret == GST_STATE_CHANGE_FAILURE)
	{
		g_printerr ("Unable to set the gst_camerabin to the NULL state.\n");
		return false;
	}

	return true;
}


/**********************************************/
//	callback functions
/**********************************************/

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
