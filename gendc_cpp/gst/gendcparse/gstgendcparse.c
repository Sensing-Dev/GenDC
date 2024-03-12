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

#include <glib-object.h>
#include <gst/gst.h>

#include "gendc.h"
#include "gstgendcparse.h"

GST_DEBUG_CATEGORY_STATIC(gendcparse_debug);
#define GST_CAT_DEFAULT gendcparse_debug

static void gst_gendcparse_dispose(GObject* object);

static GstFlowReturn gst_gendcparse_chain(GstPad* pad, GstObject* parent, GstBuffer* buf);
static gboolean gst_gendcparse_sink_event(GstPad* pad, GstObject* parent, GstEvent* event);
static gboolean gst_gendcparse_srcpad_event(GstPad* pad, GstObject* parent, GstEvent* event);

static void gst_gendcparse_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec);
static void gst_gendcparse_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec);

/* Filter signals and args */
enum {
  /* FILL ME */
  LAST_SIGNAL
};

enum {
  PROP_0
};
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE("sink",
                                                                   GST_PAD_SINK,
                                                                   GST_PAD_ALWAYS,
                                                                   GST_STATIC_CAPS("ANY"));
// TODO create caps for src_descriptor and data
static GstStaticPadTemplate src_descriptor_factory = GST_STATIC_PAD_TEMPLATE("src_descriptor",
                                                                             GST_PAD_SRC,
                                                                             GST_PAD_ALWAYS,
                                                                             GST_STATIC_CAPS("ANY"));
static GstStaticPadTemplate src_component_factory  = GST_STATIC_PAD_TEMPLATE("src_component_%u",
                                                                             GST_PAD_SRC,
                                                                             GST_PAD_REQUEST, // pad availability (on request)
                                                                             GST_STATIC_CAPS("ANY"));

#define DEBUG_INIT \
  GST_DEBUG_CATEGORY_INIT(gendcparse_debug, "gendcparse", 0, "GenDC data parser");

#define gst_gendcparse_parent_class parent_class
G_DEFINE_TYPE(GstGenDCParse, gst_gendcparse, GST_TYPE_ELEMENT);

GST_ELEMENT_REGISTER_DEFINE(gendcparse, "gendc-parse", GST_RANK_NONE, GST_TYPE_GENDCPARSE);

/* initialize the gendcparse's class */
static void
gst_gendcparse_class_init(GstGenDCParseClass* klass) {
  GstElementClass* gstelement_class;
  GObjectClass* gobject_class;

  gstelement_class = (GstElementClass*)klass;
  gobject_class    = (GObjectClass*)klass;

  parent_class = g_type_class_peek_parent(klass);

  gobject_class->dispose = gst_gendcparse_dispose;

  gobject_class->set_property = gst_gendcparse_set_property;
  gobject_class->get_property = gst_gendcparse_get_property;

  // gstelement_class->change_state = gst_gendcparse_change_state;
  // gstelement_class->send_event   = gst_gendcparse_send_event;

  gst_element_class_add_pad_template(gstelement_class, gst_static_pad_template_get(&src_component_factory)); // TODO Need list
  gst_element_class_add_pad_template(gstelement_class, gst_static_pad_template_get(&src_descriptor_factory));
  gst_element_class_add_pad_template(gstelement_class, gst_static_pad_template_get(&sink_factory));

  gst_element_class_set_static_metadata(gstelement_class,
                                        "GenDC data Parser",
                                        "Filter/Converter",
                                        "Parse gendc data in descriptor and components",
                                        "your name <your.name@your.isp>");
}

static GstPad* gst_gendcparse_ensure_src_pad(GstGenDCParse* gendcparse, const gchar* name) {
  GstPad* pad = gst_element_get_static_pad(GST_ELEMENT(gendcparse), name);
  if (!pad) {
    // Pad doesn't exist, create it
    pad                           = gst_pad_new_from_static_template(&src_component_factory, name);
    gendcparse->src_component_pad = g_list_append(gendcparse->src_component_pad, pad);
    gst_pad_set_active(pad, TRUE);
    gst_element_add_pad(GST_ELEMENT(gendcparse), pad);
    // You might want to connect signal handlers here, e.g., for pad queries or events
  }
  return pad; // Remember to unref this outside this function if you're done with it
}
static gboolean is_dynamically_created_pad(GstPad* pad) {
  const gchar* name = GST_PAD_NAME(pad);
  // dynamic pads start with "src_component_"
  return g_str_has_prefix(name, "src_component_");
}

static void gst_gendcparse_cleanup_dynamic_pads(GstGenDCParse* gendcparse) {
  GList *pads, *pad;

  pads = GST_ELEMENT_PADS(gendcparse);
  for (pad = pads; pad; pad = pad->next) {
    GstPad* current_pad = GST_PAD(pad->data);
    if (is_dynamically_created_pad(current_pad)) {
      gst_pad_set_active(current_pad, FALSE);
      gst_element_remove_pad(GST_ELEMENT(gendcparse), current_pad);
    }
  }
  g_list_free(pads);
}

static void
gst_gendcparse_reset(GstGenDCParse* gendc) {
  gendc->state = GST_GENDCPARSE_START;

  if (gendc->container_descriptor) {
    destroy_container_descriptor(gendc->container_descriptor);
  }
  gendc->container_descriptor      = NULL;
  gendc->container_descriptor_size = 0;
  gendc->container_data_size       = 0;

  for (GList* l = gendc->components; l != NULL; l = l->next) {
    destroy_component_header(l->data);
  }
  g_list_free(gendc->components);
  gendc->component_count = 0;
}

static void
gst_gendcparse_dispose(GObject* object) {
  GstGenDCParse* gendc = GST_GENDCPARSE(object);

  GST_DEBUG_OBJECT(gendc, "GenDC: Dispose");
  gst_gendcparse_reset(gendc);

  G_OBJECT_CLASS(parent_class)->dispose(object);
}

gboolean gst_gendcparse_sink_event(GstPad* pad, GstObject* parent, GstEvent* event) {
  GstGenDCParse* gendcparse;
  gboolean ret = FALSE;
  gendcparse   = GST_GENDCPARSE(parent);

  GST_LOG_OBJECT(gendcparse, "Received %s event: %" GST_PTR_FORMAT, GST_EVENT_TYPE_NAME(event), event);

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

static void
gst_gendcparse_init(GstGenDCParse* gendcparse) {
  gendcparse->sinkpad = gst_pad_new_from_static_template(&sink_factory, "sink");
  gst_pad_set_chain_function(gendcparse->sinkpad, GST_DEBUG_FUNCPTR(gst_gendcparse_chain));
  gst_pad_set_event_function(gendcparse->sinkpad, GST_DEBUG_FUNCPTR(gst_gendcparse_sink_event));
  GST_PAD_SET_PROXY_CAPS(gendcparse->sinkpad);
  gst_element_add_pad(GST_ELEMENT(gendcparse), gendcparse->sinkpad);

  gendcparse->src_descriptor_pad = gst_pad_new_from_static_template(&src_descriptor_factory, "src_descriptor");

  gst_pad_use_fixed_caps(gendcparse->src_descriptor_pad);
  // gst_pad_set_query_function(gendcparse->src_descriptor_pad, GST_DEBUG_FUNCPTR(gst_gendcparse_pad_query));
  // gst_pad_set_event_function(gendcparse->src_descriptor_pad, GST_DEBUG_FUNCPTR(gst_gendcparse_srcpad_event));
  GST_PAD_SET_PROXY_CAPS(gendcparse->src_descriptor_pad);
  gst_element_add_pad(GST_ELEMENT_CAST(gendcparse), gendcparse->src_descriptor_pad);

  // ToDO Need to create List
  // gendcparse->src_component_pad = gst_pad_new_from_template(&src_component_factory, "src_component");
  // gst_pad_use_fixed_caps(gendcparse->src_component_pad);
  // // gst_pad_set_query_function(gendcparse->src_component_pad, GST_DEBUG_FUNCPTR(gst_gendcparse_pad_query));
  // // gst_pad_set_event_function(gendcparse->src_component_pad, GST_DEBUG_FUNCPTR(gst_gendcparse_srcpad_event));
  // GST_PAD_SET_PROXY_CAPS(gendcparse->src_component_pad);
  // gst_element_add_pad(GST_ELEMENT_CAST(gendcparse), gendcparse->src_component_pad);
}

static gboolean
gst_gendcparse_validate_input(GstGenDCParse* element, GstBuffer* buf, guint64 min_size) {
  // Check if valid genDC data

  // 1. Should Have signature Signature = “GNDC”
  // 2. A GenDC Container must always contain at least one Component Header
  gboolean valid = FALSE;
  guint32 type   = 0;

  GstMapInfo info;

  g_return_val_if_fail(buf != NULL, FALSE);

  gst_buffer_map(buf, &info, GST_MAP_READ);

  if (info.size < min_size)
    goto too_small;

  if (!is_gendc_format(info.data))
    goto not_gendc;

  if (!is_valid_container(info.data))
    goto not_gendc;

  return TRUE;

  /* ERRORS */
too_small : {
  GST_ELEMENT_ERROR(element, STREAM, WRONG_TYPE, (NULL),
                    ("Not enough data to parse GENDC header (%" G_GSIZE_FORMAT " available,"
                     " %d needed)",
                     info.size, min_size));
  gst_buffer_unmap(buf, &info);
  gst_buffer_unref(buf);
  return FALSE;
}
not_gendc : {
  GST_ELEMENT_ERROR(element, STREAM, WRONG_TYPE, (NULL),
                    ("Data is not a GenDC format : 0x%" G_GINT32_MODIFIER "x", type));
  gst_buffer_unmap(buf, &info);
  gst_buffer_unref(buf);
  return FALSE;
}
}

static gboolean
gst_gendcparse_validate_component(GstGenDCParse* element, gpointer* buf, guint64 min_size) {
  // Check if valid genDC data

  // 1. Should Have  HeaderType = “GDC_COMPONENT_HEADER”
  // 2. A GenDC Component must contain at least one Part Header.
  gboolean valid = FALSE;
  guint32 type   = 0;

  GstMapInfo info;

  g_return_val_if_fail(buf != NULL, FALSE);

  if (!is_component_format(buf))
    goto not_gendc;

  if (!is_valid_component(buf))
    goto not_gendc;

  return TRUE;

not_gendc : {
  GST_ELEMENT_ERROR(element, STREAM, WRONG_TYPE, (NULL),
                    ("Data is not a GenDC component format : 0x%" G_GINT32_MODIFIER "x", type));
  return FALSE;
}
}

static void
gst_gendcparse_set_property(GObject* object, guint prop_id,
                            const GValue* value, GParamSpec* pspec) {
  GstGenDCParse* self;

  g_return_if_fail(GST_IS_GENDCPARSE(object));
  self = GST_GENDCPARSE(object);

  switch (prop_id) {
    case PROP_0:
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void
gst_gendcparse_get_property(GObject* object, guint prop_id,
                            GValue* value, GParamSpec* pspec) {
  GstGenDCParse* self;

  g_return_if_fail(GST_IS_GENDCPARSE(object));
  self = GST_GENDCPARSE(object);

  switch (prop_id) {
    case PROP_0:
      // g_value_set_boolean(value, self->silent);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static GstFlowReturn
gst_gendcparse_chain(GstPad* pad, GstObject* parent, GstBuffer* buf) {
  GstGenDCParse* gendcparse;

  gendcparse = GST_GENDCPARSE(parent);

  guint min_valid_components = 1;
  guint min_size             = 56 + 8 * min_valid_components; // ToDO
  guint64 offset             = 0;                             // We are assuming file input is complete, not segment

  guint min_valid_parts    = 1;
  guint min_component_size = 48 + 8 * min_valid_parts; // ToDO

  if (!gst_gendcparse_validate_input(gendcparse, buf, min_size)) {
    return GST_FLOW_ERROR;
  }

  // Divide buffer into container descriptor (ptr,size) and container data (ptr,size)

  // Descriptor
  GstMapInfo info;
  gst_buffer_map(buf, &info, GST_MAP_READ);

  gendcparse->container_descriptor      = create_container_descriptor(info.data);
  gendcparse->container_descriptor_size = get_descriptor_size(gendcparse->container_descriptor);

  gpointer container_data         = info.data + gendcparse->container_descriptor_size;
  gendcparse->container_data_size = get_data_size(gendcparse->container_descriptor);

  // Create and push a new buffer for the container descriptor
  GstBuffer* descriptor_buffer = gst_buffer_new_wrapped(info.data, gendcparse->container_descriptor_size);
  GstMapInfo descriptor_info;
  gst_buffer_map(descriptor_buffer, &descriptor_info, GST_MAP_WRITE);
  memcpy(descriptor_info.data, info.data, gendcparse->container_descriptor_size);
  gst_buffer_unmap(descriptor_buffer, &descriptor_info);
  gst_pad_push(gendcparse->src_descriptor_pad, descriptor_buffer);

  // Divide container data into components

  gendcparse->component_count   = get_component_count(gendcparse->container_descriptor);
  gpointer component_data       = container_data;
  guint64 component_data_offset = 0;
  guint64 prev_offset           = gendcparse->container_descriptor_size;
  for (guint64 i = 0; i < gendcparse->component_count; i++) {
    gpointer component_header = get_component_header(gendcparse->container_descriptor, i);
    gendcparse->components    = g_list_append(gendcparse->components, component_header);

    if (!gst_gendcparse_validate_component(gendcparse, component_header, min_component_size)) {
      return GST_FLOW_ERROR;
    }

    component_data              = info.data + prev_offset;
    component_data_offset       = get_component_data_offset(component_header);
    guint64 component_data_size = get_component_data_size(component_header);
    prev_offset                 = prev_offset + component_data_size;

    // create pad
    gchar* pad_name       = g_strdup_printf("src_component_%u", i); // Create a unique pad name
    GstPad* component_pad = gst_gendcparse_ensure_src_pad(gendcparse, pad_name);
    g_free(pad_name);

    // Create and push a new buffer for the components
    GstBuffer* component_buffer = gst_buffer_new_wrapped(component_data, component_data_size);
    GstMapInfo component_info;
    gst_buffer_map(component_buffer, &component_info, GST_MAP_WRITE);
    memcpy(component_info.data, info.data, component_data_size);
    gst_buffer_unmap(component_buffer, &component_info);
    gst_pad_push(component_pad, component_buffer);
    // gst_object_unref(component_pad);
  }

  return GST_FLOW_OK;
}

static gboolean
gendcparse_init(GstPlugin* plugin) {
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
