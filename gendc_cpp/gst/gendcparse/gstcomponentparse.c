/**
 * SECTION:element-componentparse
 *
 * FIXME:Describe componentparse here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! componentparse ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib-object.h>
#include <gst/gst.h>

#include "gendc.h"
#include "gstcomponentparse.h"

GST_DEBUG_CATEGORY_STATIC(componentparse_debug);
#define GST_CAT_DEFAULT componentparse_debug

static void gst_componentparse_dispose(GObject* object);

// Sink events
static gboolean gst_componentparse_sink_activate(GstPad* sinkpad, GstObject* parent);
static gboolean gst_componentparse_sink_activate_mode(GstPad* sinkpad, GstObject* parent, GstPadMode mode, gboolean active);

static gboolean gst_componentparse_send_event(GstElement* element, GstEvent* event);
static GstStateChangeReturn gst_componentparse_change_state(GstElement* element, GstStateChange transition);

static gboolean gst_componentparse_pad_query(GstPad* pad, GstObject* parent, GstQuery* query);
static gboolean gst_componentparse_pad_convert(GstPad* pad, GstFormat src_format, gint64 src_value, GstFormat* dest_format, gint64* dest_value);

static GstFlowReturn gst_componentparse_chain(GstPad* pad, GstObject* parent, GstBuffer* buf);
static gboolean gst_componentparse_sink_event(GstPad* pad, GstObject* parent, GstEvent* event);
static gboolean gst_componentparse_srcpad_event(GstPad* pad, GstObject* parent, GstEvent* event);

static void gst_componentparse_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec);
static void gst_componentparse_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec);

/* Filter signals and args */
enum {
  /* FILL ME */
  LAST_SIGNAL
};

enum {
  PROP_0
};
//TODO : create cap for component headers and component data
// GstCaps* gst_genparse_create_part_caps() {
//   GstCaps* part_caps = gst_caps_new_simple("application/x-gendc-part",
//                                                 "header", G_TYPE_POINTER, 0,
//                                                 "data", G_TYPE_POINTER, 0,
//                                                 "data-size", G_TYPE_UINT64, 0,
//                                                 "header-size", G_TYPE_UINT64, 0,
//                                                 NULL);
//   return part_caps;
// }

static GstStaticPadTemplate sink_descriptor_factory = GST_STATIC_PAD_TEMPLATE("sink_header",
                                                                   GST_PAD_SINK,
                                                                   GST_PAD_ALWAYS,
                                                                   GST_STATIC_CAPS("ANY") // TODO
);
static GstStaticPadTemplate sink_data_factory = GST_STATIC_PAD_TEMPLATE("sink_data",
                                                                   GST_PAD_SINK,
                                                                   GST_PAD_ALWAYS,
                                                                   GST_STATIC_CAPS("ANY") // TODO
);
static GstStaticPadTemplate src_header_factory = GST_STATIC_PAD_TEMPLATE("src_header",
                                                                  GST_PAD_SRC,
                                                                  GST_PAD_ALWAYS,
                                                                  GST_STATIC_CAPS("ANY") // TODO
);
static GstStaticPadTemplate src_data_factory = GST_STATIC_PAD_TEMPLATE("src_data",
                                                                  GST_PAD_SRC,
                                                                  GST_PAD_ALWAYS,
                                                                  GST_STATIC_CAPS("ANY") // TODO
);

#define DEBUG_INIT \
  GST_DEBUG_CATEGORY_INIT(componentparse_debug, "componentparse", 0, "Gendc Component data parser");

#define gst_componentparse_parent_class parent_class
G_DEFINE_TYPE(GstComponentParse, gst_componentparse, GST_TYPE_ELEMENT);

GST_ELEMENT_REGISTER_DEFINE(componentparse, "component-parse", GST_RANK_NONE, GST_TYPE_COMPONENTPARSE);

/* initialize the componentparse's class */
static void
gst_componentparse_class_init(GstComponentParseClass* klass) {
  GstElementClass* gstelement_class;
  GObjectClass* gobject_class;

  gstelement_class = (GstElementClass*)klass;
  gobject_class    = (GObjectClass*)klass;

  parent_class = g_type_class_peek_parent(klass);

  gobject_class->dispose = gst_componentparse_dispose;

  gobject_class->set_property = gst_componentparse_set_property;
  gobject_class->get_property = gst_componentparse_get_property;

  gstelement_class->change_state = gst_componentparse_change_state;
  gstelement_class->send_event   = gst_componentparse_send_event;

  gst_element_class_add_pad_template(gstelement_class, gst_static_pad_template_get(&src_header_factory));
  gst_element_class_add_pad_template(gstelement_class, gst_static_pad_template_get(&src_data_factory));
  gst_element_class_add_pad_template(gstelement_class, gst_static_pad_template_get(&sink_descriptor_factory));
  gst_element_class_add_pad_template(gstelement_class, gst_static_pad_template_get(&sink_data_factory));

  gst_element_class_set_static_metadata(gstelement_class,
                                        "Component data Parser",
                                        "Filter/Converter",
                                        "Parse component data in components",
                                        "your name <your.name@your.isp>");
}


static void
gst_componentparse_reset(GstComponentParse* component) { // TODO
  component->state = GST_COMPONENTPARSE_START;
  component->container_header = NULL;
  component->container_header_size = 0;

  component->component_count = 0;
  g_ptr_array_unref(component->components);
}

static void
gst_componentparse_dispose(GObject* object) { // TODO
  GstComponentParse* component = GST_COMPONENTPARSE(object);

  GST_DEBUG_OBJECT(component, "Component: Dispose");
  gst_componentparse_reset(component);

  G_OBJECT_CLASS(parent_class)->dispose(object);
}
static gboolean gst_componentparse_pad_query(GstPad* pad, GstObject* parent, GstQuery* query) {  // TODO
  gboolean ret = FALSE;
  return ret;
}
gboolean gst_componentparse_sink_activate_mode(GstPad* sinkpad, GstObject* parent, GstPadMode mode, gboolean active) {  // TODO
  gboolean ret = FALSE;
  return ret;
}
gboolean gst_componentparse_sink_event(GstPad* pad, GstObject* parent, GstEvent* event) {  // TODO
  GstComponentParse* componentparse;
  gboolean ret = FALSE;
  componentparse   = GST_COMPONENTPARSE(parent);

  GST_LOG_OBJECT(componentparse, "Received %s event: %" GST_PTR_FORMAT,
                 GST_EVENT_TYPE_NAME(event), event);

  switch (GST_EVENT_TYPE(event)) {
    case GST_EVENT_CAPS: {
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
gst_componentparse_send_event(GstElement* element, GstEvent* event) {  // TODO
  gboolean ret = FALSE;
  return ret;
}
static gboolean
gst_componentparse_srcpad_event(GstPad* pad, GstObject* parent, GstEvent* event) {  // TODO
  gboolean ret = FALSE;

  return ret;
}
static GstStateChangeReturn
gst_componentparse_change_state(GstElement* element, GstStateChange transition) {  // TODO
  GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
  return ret;
}

static void
gst_componentparse_init(GstComponentParse* componentparse) {
  componentparse->sink_descriptor_pad = gst_pad_new_from_static_template(&sink_descriptor_factory, "sink_header");
  // gst_pad_set_activate_function(componentparse->sink_descriptor_pad, GST_DEBUG_FUNCPTR(gst_componentparse_sink_activate));
  // gst_pad_set_activatemode_function(componentparse->sink_descriptor_pad, GST_DEBUG_FUNCPTR(gst_componentparse_sink_activate_mode));
  gst_pad_set_chain_function(componentparse->sink_descriptor_pad, GST_DEBUG_FUNCPTR(gst_componentparse_chain));
  gst_pad_set_event_function(componentparse->sink_descriptor_pad, GST_DEBUG_FUNCPTR(gst_componentparse_sink_event));
  GST_PAD_SET_PROXY_CAPS(componentparse->sink_descriptor_pad);
  gst_element_add_pad(GST_ELEMENT(componentparse), componentparse->sink_descriptor_pad);

  componentparse->sink_data_pad = gst_pad_new_from_static_template(&sink_data_factory, "sink_data");
  gst_pad_set_chain_function(componentparse->sink_data_pad, GST_DEBUG_FUNCPTR(gst_componentparse_chain));
  gst_pad_set_event_function(componentparse->sink_data_pad, GST_DEBUG_FUNCPTR(gst_componentparse_sink_event));
  GST_PAD_SET_PROXY_CAPS(componentparse->sink_data_pad);
  gst_element_add_pad(GST_ELEMENT(componentparse), componentparse->sink_data_pad);


  componentparse->src_header_pad = gst_pad_new_from_template(gst_element_class_get_pad_template(GST_ELEMENT_GET_CLASS(componentparse), "src_header"), "src_header");
  componentparse->src_data_pad = gst_pad_new_from_template(gst_element_class_get_pad_template(GST_ELEMENT_GET_CLASS(componentparse), "src_data"), "src_data");
  
  
  gst_pad_use_fixed_caps(componentparse->src_header_pad);
  gst_pad_set_query_function(componentparse->src_header_pad, GST_DEBUG_FUNCPTR(gst_componentparse_pad_query));
  gst_pad_set_event_function(componentparse->src_header_pad, GST_DEBUG_FUNCPTR(gst_componentparse_srcpad_event));
  GST_PAD_SET_PROXY_CAPS(componentparse->src_header_pad);

  gst_pad_use_fixed_caps(componentparse->src_data_pad);
  gst_pad_set_query_function(componentparse->src_data_pad, GST_DEBUG_FUNCPTR(gst_componentparse_pad_query));
  gst_pad_set_event_function(componentparse->src_data_pad, GST_DEBUG_FUNCPTR(gst_componentparse_srcpad_event));
  GST_PAD_SET_PROXY_CAPS(componentparse->src_data_pad);

  gst_element_add_pad(GST_ELEMENT_CAST(componentparse), componentparse->src_header_pad);
  gst_element_add_pad(GST_ELEMENT_CAST(componentparse), componentparse->src_data_pad);

}

static gboolean
gst_componentparse_validate_input(GstComponentParse* element, GstBuffer* buf, guint64 min_size) {
  // Check if valid genDC data

  // 1. Should Have signature Signature = “GNDC”
  // 2. A Component Container must always contain at least one Component Header
  gboolean valid = FALSE;
  guint32 type   = 0;

  GstMapInfo info;

  g_return_val_if_fail(buf != NULL, FALSE);

  gst_buffer_map(buf, &info, GST_MAP_READ);

  if (info.size < min_size)
    goto too_small;

  if (!is_component_format(info.data))
    goto not_component;

  if (!is_valid_container(info.data))
    goto not_component;

  return TRUE;

  /* ERRORS */
too_small : {
  GST_ELEMENT_ERROR(element, STREAM, WRONG_TYPE, (NULL),
                    ("Not enough data to parse COMPONENT header (%" G_GSIZE_FORMAT " available,"
                     " %d needed)",
                     info.size, 12));
  gst_buffer_unmap(buf, &info);
  gst_buffer_unref(buf);
  return FALSE;
}
not_component : {
  GST_ELEMENT_ERROR(element, STREAM, WRONG_TYPE, (NULL),
                    ("Data is not a Component format : 0x%" G_GINT32_MODIFIER "x", type));
  gst_buffer_unmap(buf, &info);
  gst_buffer_unref(buf);
  return FALSE;
}
}

static void
gst_componentparse_set_property(GObject* object, guint prop_id,
                            const GValue* value, GParamSpec* pspec) {
  GstComponentParse* self;

  g_return_if_fail(GST_IS_COMPONENTPARSE(object));
  self = GST_COMPONENTPARSE(object);

  switch (prop_id) {
    case PROP_0:
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void
gst_componentparse_get_property(GObject* object, guint prop_id,
                            GValue* value, GParamSpec* pspec) {
  GstComponentParse* self;

  g_return_if_fail(GST_IS_COMPONENTPARSE(object));
  self = GST_COMPONENTPARSE(object);

  switch (prop_id) {
    case PROP_0:
      // g_value_set_boolean(value, self->silent);
      break;
    
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

/* this function handles sink events */
static gboolean
gst_componentparse_sink_activate(GstPad* sinkpad, GstObject* parent) {
  gboolean ret;

  return ret;
}

/* chain function
 * this function does the actual processing
 */
static GstFlowReturn
gst_componentparse_chain(GstPad* pad, GstObject* parent, GstBuffer* buf) {
  GstComponentParse* componentparse;

  componentparse = GST_COMPONENTPARSE(parent);

  guint min_valid_components = 1;
  guint min_size             = 56 + 8 * min_valid_components; // ToDO
  guint64 offset             = 0;                             // We are assuming file input is complete, not segment


  if (!gst_componentparse_validate_input(componentparse, buf, min_size)) {
    return GST_FLOW_ERROR;
  }

  // Divide buffer into container descriptor (ptr,size) and container data (ptr,size)

  // Descriptor
  GstMapInfo info;
  gst_buffer_map(buf, &info, GST_MAP_READ);

  gpointer container_descriptor     = create_container_descriptor(info.data);
  guint64 container_descriptor_size = get_descriptor_size(container_descriptor);

  gpointer container_data     = info.data + container_descriptor_size;
  guint64 container_data_size = get_data_size(container_descriptor);

//  static GstStaticPadTemplate src_data_factory = GST_STATIC_PAD_TEMPLATE("src_data",
//                                                                  GST_PAD_SRC,
//                                                                  GST_PAD_ALWAYS,
//                                                                  GST_STATIC_CAPS("video/x-raw") // TODO
//);


   // Create and push a new buffer for the container descriptor
  GstBuffer* descriptor_buffer = gst_buffer_new_wrapped(container_descriptor, container_descriptor_size);
  GstMapInfo descriptor_info;
  gst_buffer_map(descriptor_buffer, &descriptor_info, GST_MAP_WRITE);
  memcpy(descriptor_info.data, container_descriptor, container_descriptor_size);
  gst_buffer_unmap(descriptor_buffer, &descriptor_info);
  gst_pad_push(componentparse->srcpad, descriptor_buffer);
  gst_pad_push(componentparse->src_header_pad, descriptor_buffer);

 // Create and push a new buffer for the container data
  GstBuffer* data_buffer = gst_buffer_new_allocate(NULL, container_data_size, NULL);
  GstMapInfo data_info;
  gst_buffer_map(data_buffer, &data_info, GST_MAP_WRITE);
  memcpy(data_info.data, container_data, container_data_size);
  gst_buffer_unmap(data_buffer, &data_info);
  gst_pad_push(componentparse->srcpad, data_buffer);
  gst_pad_push(componentparse->src_data_pad, data_buffer);

  // Unmap and unref the original buffer
  gst_buffer_unmap(buf, &info);
  gst_buffer_unref(buf);

  /* just push out the incoming buffer without touching it */
  return GST_FLOW_OK;
}

static gboolean
componentparse_init(GstPlugin* plugin) {
  GST_DEBUG_CATEGORY_INIT(componentparse_debug, "componentparse",
                          0, "Parse Component data");

  return GST_ELEMENT_REGISTER(componentparse, plugin);
}

GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    componentparse,
    "Parse component data to decriptor and componets",
    componentparse_init,
    PACKAGE_VERSION,
    GST_LICENSE,
    GST_PACKAGE_NAME,
    GST_PACKAGE_ORIGIN)
