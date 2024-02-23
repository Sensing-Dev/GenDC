/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2024  <<user@hostname.org>>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

 /**
  * SECTION:element-gendcparse
  *
  * FIXME:Describe gendcparse here.
  *
  * <refsect2>
  * <title>Example launch line</title>
  * |[
  * gst-launch -v -m fakesrc ! gendcparse ! fakesink silent=TRUE
  * ]|
  * </refsect2>
  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gst/gst.h>
#include <glib-object.h>

#include "gstgendcparse.h"

GST_DEBUG_CATEGORY_STATIC(gendcparse_debug);
#define GST_CAT_DEFAULT gendcparse_debug

static void gst_gendcparse_dispose(GObject* object);

static gboolean gst_gendcparse_sink_activate(GstPad* sinkpad, GstObject* parent);
static gboolean gst_gendcparse_sink_activate(GstPad* pad, GstObject* parent, GstEvent* event);
static gboolean gst_gendcparse_sink_activate_mode(GstPad* sinkpad, GstObject* parent, GstPadMode mode, gboolean active);
static gboolean gst_gendcparse_send_event(GstElement* element, GstEvent* event);
static GstStateChangeReturn gst_gendcparse_change_state(GstElement* element, GstStateChange transition);

static gboolean gst_gendcparse_pad_query(GstPad* pad, GstObject* parent, GstQuery* query);
static gboolean gst_gendcparse_pad_convert(GstPad* pad, GstFormat src_format, gint64 src_value, GstFormat* dest_format, gint64* dest_value);

static GstFlowReturn gst_gendcparse_chain(GstPad* pad, GstObject* parent, GstBuffer* buf);
static gboolean gst_gendcparse_sink_event(GstPad* pad, GstObject* parent, GstEvent* event);
static void gst_gendcparse_loop(GstPad* pad);
static gboolean gst_gendcparse_srcpad_event(GstPad* pad, GstObject* parent, GstEvent* event);

static void gst_gendcparse_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec);
static void gst_gendcparse_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec);

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_SILENT,
  PROP_IGNORE_LENGTH,
};

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE("sink",
  GST_PAD_SINK,
  GST_PAD_ALWAYS,
  GST_STATIC_CAPS("ANY") // TODO
);

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE("src",
  GST_PAD_SRC,
  GST_PAD_ALWAYS,
  GST_STATIC_CAPS("ANY") // TODO
);

#define DEBUG_INIT \
  GST_DEBUG_CATEGORY_INIT(gendcparse_debug, "gendcparse", 0, "GenDC data parser");

#define gst_gendcparse_parent_class parent_class
G_DEFINE_TYPE(GstGenDCParse, gst_gendcparse, GST_TYPE_ELEMENT);

GST_ELEMENT_REGISTER_DEFINE(gendcparse, "gendc-parse", GST_RANK_NONE, GST_TYPE_GENDCPARSE);

/* initialize the gendcparse's class */
static void
gst_gendcparse_class_init(GstGenDCParseClass* klass)
{
  GstElementClass* gstelement_class;
  GObjectClass* gobject_class;

  gstelement_class = (GstElementClass*)klass;
  gobject_class = (GObjectClass*)klass;

  parent_class = g_type_class_peek_parent(klass);

  gobject_class->dispose = gst_gendcparse_dispose;

  gobject_class->set_property = gst_gendcparse_set_property;
  gobject_class->get_property = gst_gendcparse_get_property;

  // g_object_class_install_property(gobject_class, PROP_SILENT,
  //                                 g_param_spec_boolean("silent",
  //                                                      "Silent",
  //                                                      "Produce verbose output ?",
  //                                                      FALSE, G_PARAM_READWRITE));
  // g_object_class_install_property(object_class, PROP_IGNORE_LENGTH,
  //                                 g_param_spec_boolean("ignore-length",
  //                                                      "Ignore length",
  //                                                      "Ignore length from the gendc header",
  //                                                      DEFAULT_IGNORE_LENGTH, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  gstelement_class->change_state = gst_gendcparse_change_state;
  gstelement_class->send_event = gst_gendcparse_send_event;

  gst_element_class_add_pad_template(gstelement_class, gst_static_pad_template_get(&src_factory));
  gst_element_class_add_pad_template(gstelement_class, gst_static_pad_template_get(&sink_factory));

  gst_element_class_set_static_metadata(gstelement_class,
    "GenDC data Parser",
    "USB3 Camera/GenDC Data",
    "Parse gendc data in components",
    "your name <your.name@your.isp>");
}

static void
gst_gendcparse_reset(GstGenDCParse* gendc)
{
}

static void
gst_gendcparse_dispose(GObject* object)
{
  GstGenDCParse* gendc = GST_GENDCPARSE(object);

  GST_DEBUG_OBJECT(gendc, "GenDC: Dispose");
  gst_gendcparse_reset(gendc);

  G_OBJECT_CLASS(parent_class)->dispose(object);
}
static gboolean gst_gendcparse_pad_query(GstPad* pad, GstObject* parent, GstQuery* query) {
  gboolean ret = FALSE;
  return ret;
}
static gboolean
gst_gendcparse_send_event(GstElement* element, GstEvent* event) {
  gboolean ret = FALSE;
  return ret;
}
static gboolean
gst_gendcparse_srcpad_event(GstPad* pad, GstObject* parent, GstEvent* event) {
  gboolean ret = FALSE;

  return ret;
}
static GstStateChangeReturn
gst_gendcparse_change_state(GstElement* element, GstStateChange transition) {
  GstStateChangeReturn ret;
  return ret;
}
static void
gst_gendcparse_init(GstGenDCParse* gendcparse)
{
  gendcparse->sinkpad = gst_pad_new_from_static_template(&sink_factory, "sink");
  gst_pad_set_event_function(gendcparse->sinkpad, GST_DEBUG_FUNCPTR(gst_gendcparse_sink_activate));
  gst_pad_set_chain_function(gendcparse->sinkpad, GST_DEBUG_FUNCPTR(gst_gendcparse_chain));
  GST_PAD_SET_PROXY_CAPS(gendcparse->sinkpad);
  gst_element_add_pad(GST_ELEMENT(gendcparse), gendcparse->sinkpad);

  gendcparse->srcpad = gst_pad_new_from_template(gst_element_class_get_pad_template(GST_ELEMENT_GET_CLASS(gendcparse), "src"), "src");
  gst_pad_use_fixed_caps(gendcparse->srcpad);
  gst_pad_set_query_function(gendcparse->srcpad, GST_DEBUG_FUNCPTR(gst_gendcparse_pad_query));
  gst_pad_set_event_function(gendcparse->srcpad, GST_DEBUG_FUNCPTR(gst_gendcparse_srcpad_event));
  GST_PAD_SET_PROXY_CAPS(gendcparse->srcpad);

  gst_element_add_pad(GST_ELEMENT_CAST(gendcparse), gendcparse->srcpad);

  // gendcparse->srcpad = gst_pad_new_from_static_template(&src_factory, "src");
  // GST_PAD_SET_PROXY_CAPS(gendcparse->srcpad);
  // gst_element_add_pad(GST_ELEMENT(gendcparse), gendcparse->srcpad);

  gendcparse->silent = FALSE;
}

static gboolean
gst_gendcparse_parse_file_header(GstElement* element, GstBuffer* buf)
{

  // Check if valid genDC data

  gboolean valid = FALSE;
  guint32 type;

  if (!valid) // TODO
    return FALSE;

  if (type) // TODO
    goto not_gendc;

  return TRUE;

  /* ERRORS */
not_gendc:
  {
    GST_ELEMENT_ERROR(element, STREAM, WRONG_TYPE, (NULL),
      ("Data is not a GenDC format file: 0x%" G_GINT32_MODIFIER "x", type));
    return FALSE;
  }
}

static GstFlowReturn
gst_gendcparse_stream_init(GstGenDCParse* gendcparse)
{
  GstFlowReturn res;
  GstBuffer* buf = NULL;

  if ((res = gst_pad_pull_range(gendcparse->sinkpad,
    gendcparse->offset, 12, &buf)) != GST_FLOW_OK) // TODO
    return res;
  else if (!gst_gendcparse_parse_file_header(GST_ELEMENT_CAST(gendcparse), buf))
    return GST_FLOW_ERROR;

  gendcparse->offset += 12; // TODO

  return GST_FLOW_OK;
}

static void
gst_gendcparse_set_property(GObject* object, guint prop_id,
  const GValue* value, GParamSpec* pspec)
{
  GstGenDCParse* self;

  g_return_if_fail(GST_IS_GENDCPARSE(object));
  self = GST_GENDCPARSE(object);

  switch (prop_id)
  {
  case PROP_SILENT:
    self->silent = g_value_get_boolean(value);
    break;
  case PROP_IGNORE_LENGTH:
    self->ignore_length = g_value_get_boolean(value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void
gst_gendcparse_get_property(GObject* object, guint prop_id,
  GValue* value, GParamSpec* pspec)
{
  GstGenDCParse* self;

  g_return_if_fail(GST_IS_GENDCPARSE(object));
  self = GST_GENDCPARSE(object);

  switch (prop_id)
  {
  case PROP_SILENT:
    // g_value_set_boolean(value, self->silent);
    break;
  case PROP_IGNORE_LENGTH:
    g_value_set_boolean(value, self->ignore_length);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean
gst_gendcparse_sink_activate(GstPad* pad, GstObject* parent, GstEvent* event)
{
  GstGenDCParse* gendcparse;
  gboolean ret;

  gendcparse = GST_GENDCPARSE(parent);

  GST_LOG_OBJECT(gendcparse, "Received %s event: %" GST_PTR_FORMAT,
    GST_EVENT_TYPE_NAME(event), event);

  switch (GST_EVENT_TYPE(event))
  {
  case GST_EVENT_CAPS:
  {
    GstCaps* caps;

    gst_event_parse_caps(event, &caps);
    /* do something with the caps */

    /* and forward */
    ret = gst_pad_event_default(pad, parent, event);
    break;
  }
  default:
    ret = gst_pad_event_default(pad, parent, event);
    break;
  }
  return ret;
}

/* chain function
 * this function does the actual processing
 */
static GstFlowReturn
gst_gendcparse_chain(GstPad* pad, GstObject* parent, GstBuffer* buf)
{
  GstGenDCParse* gendcparse;

  gendcparse = GST_GENDCPARSE(parent);

  if (gendcparse->silent == FALSE)
    g_print("I'm plugged, therefore I'm in.\n");

  /* just push out the incoming buffer without touching it */
  return gst_pad_push(gendcparse->srcpad, buf);
}

static gboolean
gendcparse_init(GstPlugin* plugin)
{

  GST_DEBUG_CATEGORY_INIT(gendcparse_debug, "gendcparse",
    0, "Parse GenDC data");

  return GST_ELEMENT_REGISTER(gendcparse, plugin);
}

GST_PLUGIN_DEFINE(
  GST_VERSION_MAJOR,
  GST_VERSION_MINOR,
  gendcparse,
  "Parse gendc data to decriptor and componets",
  gendcparse_init,
  PACKAGE_VERSION,
  GST_LICENSE,
  GST_PACKAGE_NAME,
  GST_PACKAGE_ORIGIN)
