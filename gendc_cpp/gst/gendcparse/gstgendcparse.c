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
#include "gendc.h"

GST_DEBUG_CATEGORY_STATIC(gendcparse_debug);
#define GST_CAT_DEFAULT gendcparse_debug

static void gst_gendcparse_dispose(GObject* object);

// Sink events
static gboolean gst_gendcparse_sink_activate(GstPad* sinkpad, GstObject* parent);
static gboolean gst_gendcparse_sink_activate_mode(GstPad* sinkpad, GstObject* parent, GstPadMode mode, gboolean active);
static gboolean gst_gendcparse_sink_event(GstPad* pad, GstObject* parent, GstEvent* event);

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
    "Filter/Converter",
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
gboolean gst_gendcparse_sink_activate_mode(GstPad* sinkpad, GstObject* parent, GstPadMode mode, gboolean active)
{
  gboolean ret = FALSE;
  return ret;
}
gboolean gst_gendcparse_sink_event(GstPad* pad, GstObject* parent, GstEvent* event)
{
  GstGenDCParse* gendcparse;
  gboolean ret = FALSE;
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
  gst_pad_set_activate_function(gendcparse->sinkpad, GST_DEBUG_FUNCPTR(gst_gendcparse_sink_activate));
  gst_pad_set_activatemode_function(gendcparse->sinkpad, GST_DEBUG_FUNCPTR(gst_gendcparse_sink_activate_mode));
  gst_pad_set_chain_function(gendcparse->sinkpad, GST_DEBUG_FUNCPTR(gst_gendcparse_chain));
  gst_pad_set_event_function(gendcparse->sinkpad, GST_DEBUG_FUNCPTR(gst_gendcparse_sink_event));
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

  //gendcparse->silent = FALSE;
}

static gboolean
gst_gendcparse_parse_header(GstElement* element, GstBuffer* buf)
{

  // Check if valid genDC data

  // 1. Should Have signature Signature = “GNDC” 
  // 2. A GenDC Container must always contain at least one Component Header
  gboolean valid = FALSE;
  guint32 type = 0;

  GstMapInfo info;


  g_return_val_if_fail(buf != NULL, FALSE);

  gst_buffer_map(buf, &info, GST_MAP_READ);

  if (info.size < 12)
    goto too_small;

  if (!is_gendc_format(info.data))
    goto not_gendc; 
  
  if (!is_valid_gendc(info.data))
    goto not_gendc;

  return TRUE;

  /* ERRORS */
too_small:
  {
    GST_ELEMENT_ERROR(element, STREAM, WRONG_TYPE, (NULL),
      ("Not enough data to parse GENDC header (%" G_GSIZE_FORMAT " available,"
        " %d needed)", info.size, 12));
    gst_buffer_unmap(buf, &info);
    gst_buffer_unref(buf);
    return FALSE;
  }
not_gendc:
  {
    GST_ELEMENT_ERROR(element, STREAM, WRONG_TYPE, (NULL),
      ("Data is not a GenDC format : 0x%" G_GINT32_MODIFIER "x", type));
    gst_buffer_unmap(buf, &info);
    gst_buffer_unref(buf);
    return FALSE;
  }
}

static GstFlowReturn
gst_gendcparse_stream_init(GstGenDCParse* gendcparse)
{
  GstFlowReturn res;
  GstBuffer* buf = NULL;

  guint size = 12; // ToDO

  if ((res = gst_pad_pull_range(gendcparse->sinkpad, gendcparse->offset, 12, &buf)) != GST_FLOW_OK) // TODO
    return res;
  else if (!gst_gendcparse_parse_header(GST_ELEMENT_CAST(gendcparse), buf))
    return GST_FLOW_ERROR;

  //gendcparse->offset += 12; // TODO

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


/* this function handles sink events */
static gboolean
gst_gendcparse_sink_activate(GstPad* sinkpad, GstObject* parent)
{
  gboolean ret;

  
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
