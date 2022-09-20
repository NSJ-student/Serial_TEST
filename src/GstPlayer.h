/*
 * GstPlayer.h
 *
 *  Created on: 2022. 9. 15.
 *      Author: Sujin
 */

#ifndef SRC_GSTPLAYER_H_
#define SRC_GSTPLAYER_H_

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#ifdef WIN32
#include <gdk/gdkwin32.h>
#else
#include <gdk/gdkx.h>
#endif

#include <gst/gst.h>
#include <gst/video/videooverlay.h>

typedef enum {
  GST_PLAY_FLAG_VIDEO         = (1 << 0), /* We want video output */
  GST_PLAY_FLAG_AUDIO         = (1 << 1), /* We want audio output */
  GST_PLAY_FLAG_TEXT          = (1 << 2)  /* We want subtitle output */
} GstPlayFlags;

class GstPlayer {
public:
	GstPlayer();
	virtual ~GstPlayer();

	void set_video_sink(GdkWindow *sink);
	void set_camera_sink(GdkWindow *sink);

	gboolean is_video_playing();
	gboolean is_camera_playing();
	gboolean play_video(const char * path);
	gboolean stop_video();
	gboolean play_camera();
	gboolean stop_camera();

private:
	GstElement * gst_playbin;
	GstElement * gst_camerabin;
	GstElement * gst_video_sink;
	GstElement * gst_camera_sink;
};

#endif /* SRC_GSTPLAYER_H_ */
