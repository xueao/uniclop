

#include "GstVideoInput.hpp"


#include <boost/gil/gil_all.hpp>

#include <stdexcept>

// using C code because gstreamermm was too much paint to install
#include <glib.h>
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>

namespace uniclop
{
namespace devices
{
namespace video
{


program_options::options_description GstVideoInput::get_options_description()
{
    program_options::options_description desc("GstVideoInput options");
    desc.add_options()
    ("width", program_options::value< int >()->default_value(640),
     "Define the input image width to define the input size. Accepted values are 320, 640, 800 (320x240, 640x480 and 800x600 respectivelly)")

    ("video_sink",  program_options::value<string>()->default_value("v4l2src"), "video input  gstreamer module")
    ;

    return desc;
}


void GstVideoInput::parse_options(program_options::variables_map &options)
{


    video_sink_name = "v4l2src";
    if (options.count("video_sink") != 0)
    {
        video_sink_name = options["video_sink"].as<string>();
    }

    width = 640;
    height = 480;

    if (options.count("width") != 0)
    {
        width = options["width"].as<int>();
    }
    switch (width)
    {
    case 320:
        height = 240;
        break;
    case 640:
        height = 480;
        break;
    case 800:
        height = 600;
        break;


    default:
        throw std::runtime_error("GstVideoInput received and unmanaged video width value");
    }


    return;
}

GstVideoInput::GstVideoInput(program_options::variables_map &options)
{

    parse_options(options);

    current_image_p.reset(new GstVideoInput::image_t(width, height));
    current_image_view = boost::gil::const_view(*current_image_p);

    init_gstreamer(video_sink_name);

    return;
}


GstVideoInput::~GstVideoInput()
{
    // nothing to do here
    return;
}

void GstVideoInput::init_gstreamer(const string &video_sink_name)
{


    // the following pipeline works on my laptop
    // gst-launch-0.10 v4l2src ! video/x-raw-yuv, width=640,height=480 ! xvimagesink

    GstElement *camera_source, *tee, *videosink_queue, *fakesink_queue, *videosink, *fakesink;
    //GstPad *pad;
    GstCaps *filter;

    GstPipeline *pipeline;

    gboolean link_ok;

    GMainLoop * loop;
    loop = g_main_loop_new(NULL, FALSE);

    int argc = 0;
    gst_init(&argc, NULL);

    // pipeline element
    pipeline = GST_PIPELINE(gst_pipeline_new("test-camera"));

    // the camera
    camera_source = gst_element_factory_make(video_sink_name.c_str(), "camera_source");

    // tee
    tee = gst_element_factory_make("tee", "tee");

    // queue for video sink
    videosink_queue = gst_element_factory_make("queue", "videosink_queue");

    // queue  for fake sink
    fakesink_queue = gst_element_factory_make("queue", "fakesink_queue");

    // the screen sink
    videosink = gst_element_factory_make("xvimagesink", "videosink");

    // fake sink to capture buffer
    fakesink = gst_element_factory_make("fakesink", "fakesink");

    gst_bin_add_many(GST_BIN(pipeline), camera_source, tee, videosink_queue, fakesink_queue, videosink, fakesink, NULL);

	// FIXME how to get the depth from the image type ?
	//GstVideoInput::image_t::point_t
	//GstVideoInput::image_t::value_t
	const int depth = 24; // 8 bits RGB
    filter = gst_caps_new_simple("video/x-raw-yuv", "width", G_TYPE_INT, width, "height", G_TYPE_INT, height, /*"framerate",
	 GST_TYPE_FRACTION, 15, 1,*/"bpp", G_TYPE_INT, depth, "depth", G_TYPE_INT, depth, NULL);

    // Camera -> Tee
    link_ok = gst_element_link_filtered(camera_source, tee, filter);
    if (!link_ok)
    {
        g_warning("Failed to link elements !");
        throw std::runtime_error("VideoInputApplication::init_video_input failed to link the tee element");
    }


    // Tee -> Queue1 -> Videosink
    link_ok = gst_element_link(tee, videosink_queue);
    link_ok = link_ok && gst_element_link(videosink_queue, videosink);
    if (!link_ok)
    {
        g_warning("Failed to link elements!");
        throw std::runtime_error("VideoInputApplication::init_video_input failed to link the videosink element");
    }

    // Tee -> Queue2 -> Fakesink
    link_ok = gst_element_link(tee, fakesink_queue);
    link_ok = link_ok && gst_element_link(fakesink_queue, fakesink);
    if (!link_ok)
    {
        g_warning("Failed to link elements!");
        throw std::runtime_error("VideoInputApplication::init_video_input failed to link the fakesink element");
    }

    // As soon as screen is exposed, window ID will be advised to the sink
    //g_signal_connect(screen, "expose-event", G_CALLBACK(expose_cb), videosink);

    g_object_set(G_OBJECT(fakesink), "signal-handoffs", TRUE, NULL);
    g_signal_connect(fakesink, "handoff", G_CALLBACK(GstVideoInput::on_new_frame_callback), this);

    const GstStateChangeReturn ret = gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {

        GstMessage * msg;
        g_print("Failed to start up pipeline!\n");
        msg = gst_bus_poll(gst_pipeline_get_bus(pipeline), GST_MESSAGE_ERROR, 0);
        if (msg)
        {
            GError * err = NULL;
            gst_message_parse_error(msg, &err, NULL);
            g_print("ERROR: %s\n", err->message);
            g_error_free(err);
            gst_message_unref(msg);
        }
        throw std::runtime_error("VideoInputApplication::init_video_input failed to start up pipeline!");
    }

    g_main_loop_run(loop);

    g_main_loop_unref(loop);
    gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
    gst_object_unref(pipeline);

    return;
}



void GstVideoInput::on_new_frame_callback(GstElement *element, GstBuffer * buffer, GstPad* pad, gpointer self_p)
{

    ((GstVideoInput *) self_p)->on_new_frame(element, buffer, pad);

    return;
}

void GstVideoInput::on_new_frame(GstElement *element, GstBuffer * buffer, GstPad* pad)
{

    // here goes the video processing

    // unsigned char *data_photo = (unsigned char *) GST_BUFFER_DATA(buffer);


    if (true)
    {
        g_debug("on_new_frame was called");
    }

    return;
}


GstVideoInput::const_view_t &GstVideoInput::get_new_image()
{
    // wait until a new image has arrived

    return  current_image_view;
}


}
}
}