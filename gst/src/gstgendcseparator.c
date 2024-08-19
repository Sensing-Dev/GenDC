/**
 * SECTION:element-gendcseparator
 *
 * FIXME:Describe gendcseparator here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! gendcseparator ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gst/gst.h>

#include "gstgendcseparator.h"
#include<stdio.h>
#include<stdlib.h>

GST_DEBUG_CATEGORY_STATIC (gst_gendc_separator_debug);
#define GST_CAT_DEFAULT gst_gendc_separator_debug

#define COMPONENT_COUNT_OFFSET 52
#define COMPONENT_COUNT_SIZE 4
#define COMPONENT_OFFSET_OFFSET 56

#define DESCRIPTOR_SIZE_OFFSET 48
#define DESCRIPTOR_SIZE_SIZE 4

#define GST_GENDC_SRPARATOR_DEBUG 1

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_SILENT
};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

// Two types of src pad: descriptor and components
static GstStaticPadTemplate defalut_src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

static GstStaticPadTemplate component_src_factory = GST_STATIC_PAD_TEMPLATE ("component_src%u",
    GST_PAD_SRC,
    GST_PAD_SOMETIMES,
    GST_STATIC_CAPS ("ANY")
    );

#define gst_gendc_separator_parent_class parent_class
G_DEFINE_TYPE (GstGenDCSeparator, gst_gendc_separator, GST_TYPE_ELEMENT);

GST_ELEMENT_REGISTER_DEFINE (gendc_separator, "gendcseparator", GST_RANK_NONE,
    GST_TYPE_GENDCSEPARATOR);

static void gst_gendc_separator_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_gendc_separator_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);

static gboolean gst_gendc_separator_sink_event (GstPad * pad,
    GstObject * parent, GstEvent * event);
static GstFlowReturn gst_gendc_separator_chain (GstPad * pad,
    GstObject * parent, GstBuffer * buf);

/* GObject vmethod implementations */

/* initialize the gendcseparator's class */
static void
gst_gendc_separator_class_init (GstGenDCSeparatorClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_gendc_separator_set_property;
  gobject_class->get_property = gst_gendc_separator_get_property;

  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
      FALSE, G_PARAM_READWRITE));

  gst_element_class_set_details_simple (gstelement_class,
      "GenDCSeparator",
      "Demuxer",
      "Parse GenDC binary data and split it into each component", "momoko.kono@fixstars.com");

  gst_element_class_add_pad_template (gstelement_class, gst_static_pad_template_get (&defalut_src_factory));
  gst_element_class_add_pad_template (gstelement_class, gst_static_pad_template_get (&component_src_factory));
  gst_element_class_add_pad_template (gstelement_class, gst_static_pad_template_get (&sink_factory));
}

static GstPad*
gst_gendc_separator_init_component_src_pad(GstGenDCSeparator * filter, const gchar* component_pad_name){
  GstPad* pad = gst_element_get_static_pad(GST_ELEMENT(filter), component_pad_name);

  if (!pad){
    GST_DEBUG("%s does not exist\n", component_pad_name);

    pad = gst_pad_new_from_static_template (&component_src_factory, component_pad_name);
    GST_PAD_SET_PROXY_CAPS (pad);
    gst_element_add_pad (GST_ELEMENT (filter), pad);
    filter->component_src_pads = g_list_append(filter->component_src_pads, pad);
  } else {
    GST_DEBUG("%s already exists\n", component_pad_name);
  }
  return pad;
}

static void
gst_gendc_separator_init (GstGenDCSeparator * filter)
{
  filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
  gst_pad_set_event_function (filter->sinkpad, GST_DEBUG_FUNCPTR (gst_gendc_separator_sink_event));
  gst_pad_set_chain_function (filter->sinkpad, GST_DEBUG_FUNCPTR (gst_gendc_separator_chain));
  GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

  filter->srcpad = gst_pad_new_from_static_template (&defalut_src_factory, "src");
  GST_PAD_SET_PROXY_CAPS (filter->srcpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

  filter->component_src_pads=NULL;
  filter->isGenDC = FALSE;
  
  filter->framecount = 0;
  filter->silent = TRUE;
  filter->head = TRUE;

  filter->component_info = NULL;
  filter->current_cmp_info = NULL;

}

static void
gst_gendc_separator_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec)
{
  GstGenDCSeparator *filter = GST_GENDCSEPARATOR (object);

  switch (prop_id) {
    case PROP_SILENT:
      filter->silent = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_gendc_separator_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec)
{
  GstGenDCSeparator *filter = GST_GENDCSEPARATOR (object);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* this function handles sink events */
static gboolean
gst_gendc_separator_sink_event (GstPad * pad, GstObject * parent,
    GstEvent * event)
{
  GstGenDCSeparator *filter;
  gboolean ret;

  filter = GST_GENDCSEPARATOR (parent);

  GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,
      GST_EVENT_TYPE_NAME (event), event);

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
    {
      GstCaps *caps;

      gst_event_parse_caps (event, &caps);
      /* do something with the caps */

      /* and forward */
      ret = gst_pad_event_default (pad, parent, event);
      break;
    }
    default:
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }
  return ret;
}

gboolean
_is_gendc (GstMapInfo map, GstGenDCSeparator* filter){
  if (*((guint32 *)map.data) == 0x43444E47){
    return TRUE;
  }
  return FALSE;
}

void
_get_valid_component_offset(GstMapInfo map, GstGenDCSeparator* filter, GstBuffer * buf){
  guint32 component_count = *((guint32 *)(map.data + COMPONENT_COUNT_OFFSET));
  for (guint i=0; i < component_count; ++i){
    guint64 ith_component_offset = *((guint64 *)(map.data + COMPONENT_OFFSET_OFFSET + sizeof(guint64) * i));
    gushort ith_component_flag  = *((gushort *)(map.data + ith_component_offset + 2));
    if (ith_component_flag & 0x0001){
      // invalid component
    }else{
      struct _ComponentInfo *this_component = g_new(struct _ComponentInfo, 1);
      this_component->ith_valid_component = i;
      this_component->partcount = *((guint16 *)(map.data + ith_component_offset + 46));
      this_component->partinfo = NULL;
      this_component->current_prt_info = NULL;

      for (int pc=0; pc < this_component->partcount; pc++){
        struct _PartInfo *jth_part = g_new(struct _PartInfo, 1);
        gint jth_partoffset = *((guint64 *)(map.data + ith_component_offset+ 48 + 8 * pc));
        jth_part->dataoffset = *((guint64 *)(map.data + jth_partoffset+ 32));
        jth_part->datasize = *((guint64 *)(map.data + jth_partoffset+ 24));
        this_component->partinfo = g_list_append(this_component->partinfo, jth_part);
      }

      if (!this_component->current_prt_info){
        this_component->current_prt_info = this_component->partinfo;
      }

      this_component->ith_comp_index = i;

      filter->component_info = g_list_append(filter->component_info, this_component);
      if (!filter->current_cmp_info){
        filter->current_cmp_info = filter->component_info;
      }

      gchar* pad_name = g_strdup_printf("component_src%u", i); 
      GstPad* comp_pad = gst_gendc_separator_init_component_src_pad(filter, pad_name);
      // gst_pad_push_data:<gendcseparator0:component_src0> Got data flow before stream-start event
      GstEvent *event = gst_event_new_stream_start (pad_name);
      gst_pad_push_event (comp_pad, gst_event_ref (event));
      
      // gst_pad_push_data:<gendcseparator0:component_src0> Got data flow before segment event
      GstSegment segment;
      GstClockTime start_time = GST_BUFFER_PTS(buf);
      GstClockTime duration = GST_BUFFER_DURATION(buf);
      gst_segment_init(&segment, GST_FORMAT_TIME);
      segment.start = start_time;
      segment.duration = duration;
      segment.flags = GST_SEGMENT_FLAG_NONE;
      GstEvent *segment_event = gst_event_new_segment(&segment);
      gst_pad_push_event(comp_pad, segment_event);
    }
  }
}


GstBuffer * _generate_descriptor_buffer(GstMapInfo map, GstGenDCSeparator* filter){
  filter->descriptor_size = *(guint32 *)(map.data + 48);
  filter->container_size = (guint64)(filter->descriptor_size) + *(guint64 *)(map.data + 32);

  if (map.size >= filter->descriptor_size){
    filter->isDescriptor = FALSE;
  }else{
    filter->descriptor_size = filter->descriptor_size - map.size;
  }

  GstBuffer *descriptor_buf = gst_buffer_new_allocate (NULL, filter->descriptor_size, NULL);
  gst_buffer_fill (descriptor_buf, 0, map.data, filter->descriptor_size);
  return descriptor_buf;
}


static GstFlowReturn
gst_gendc_separator_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
  GstGenDCSeparator *filter  = GST_GENDCSEPARATOR (parent);
  GstMapInfo map;

  // copy to map
  if (!gst_buffer_map(buf, &map, GST_MAP_READ)){
    return GST_FLOW_ERROR;
  }

  // min requires first 4 byte to check signature
  if (map.size < sizeof(guint32)){
    gst_buffer_unmap(buf, &map);
    return GST_FLOW_ERROR;
  }

  // Check GenDC; TODO: replace it with GenDC separator API
  if (filter->head && _is_gendc(map, filter)){
    filter->accum_cursor = 0;
    filter->isGenDC = TRUE;
    filter->isDescriptor = TRUE;

    _get_valid_component_offset(map, filter, buf); 

    // get descriptor
    GstBuffer *descriptor_buf = _generate_descriptor_buffer(map, filter);
    gst_pad_push (filter->srcpad, descriptor_buf);

    filter->isDescriptor = FALSE;
    filter->head = FALSE;

  } else if ( !filter->isGenDC){
    gst_buffer_unmap(buf, &map);
    return gst_pad_push (filter->srcpad, buf); 
  }

  struct _ComponentInfo *info = NULL;

  while(filter->current_cmp_info){
    info = (struct _ComponentInfo *)  filter->current_cmp_info->data;
    gchar* pad_name = g_strdup_printf("component_src%u", info->ith_comp_index); 
    GstPad* comp_pad = gst_gendc_separator_init_component_src_pad(filter, pad_name);
    g_free(pad_name);

    struct _PartInfo *jth_part_info = (struct _PartInfo *)  info->current_prt_info->data;
    jth_part_info->dataoffset = jth_part_info->dataoffset > filter->accum_cursor ? jth_part_info->dataoffset - filter->accum_cursor : 0;
    while (info->current_prt_info){
      if (map.size < jth_part_info->dataoffset + jth_part_info->datasize){
        guint32 size_of_copy = map.size - jth_part_info->dataoffset;

        GstBuffer *this_comp_buffer = gst_buffer_new_allocate (NULL, size_of_copy, NULL);
        gst_buffer_fill (this_comp_buffer, 0, map.data + jth_part_info->dataoffset, size_of_copy);
        gst_pad_push (comp_pad, this_comp_buffer);

        jth_part_info->dataoffset = jth_part_info->dataoffset > filter->accum_cursor ? jth_part_info->dataoffset - filter->accum_cursor : 0;
        jth_part_info->datasize -= size_of_copy;

        filter->head = FALSE;

        filter->accum_cursor += map.size;
        gst_buffer_unmap(buf, &map);

        return GST_FLOW_OK;

      }else{
        guint32 size_of_copy = jth_part_info->datasize;

        GstBuffer *this_comp_buffer = gst_buffer_new_allocate (NULL, size_of_copy, NULL);
        gst_buffer_fill (this_comp_buffer, 0, map.data + jth_part_info->dataoffset, size_of_copy);
        gst_pad_push (comp_pad, this_comp_buffer);

        // shift to the next part
        info->current_prt_info = info->current_prt_info->next;
        if (info->current_prt_info){
          jth_part_info = (struct _PartInfo *)info->current_prt_info->data;
          jth_part_info->dataoffset = jth_part_info->dataoffset > filter->accum_cursor ? jth_part_info->dataoffset - filter->accum_cursor : 0;
        }else{
          break;
        }

      }
    }

    filter->current_cmp_info = filter->current_cmp_info->next;

  }
  filter->accum_cursor += map.size;

  if (filter->accum_cursor >= filter->container_size){
    filter->head = TRUE;
    filter->framecount += 1;
    filter->accum_cursor = 0;
  } 
  gst_buffer_unmap(buf, &map);
  
  return GST_FLOW_OK;
}

static gboolean
gendcseparator_init (GstPlugin * gendcseparator)
{
  GST_DEBUG_CATEGORY_INIT (gst_gendc_separator_debug, "gendcseparator", 0, "Template gendcseparator");
  // return GST_ELEMENT_REGISTER (gendc_separator, gendcseparator);
  return gst_element_register (gendcseparator, "gendcseparator", GST_RANK_NONE, GST_TYPE_GENDCSEPARATOR);
}

#ifndef PACKAGE
#define PACKAGE "gendcseparator"
#endif

/* gstreamer looks for this structure to register gendcseparators
 *
 * exchange the string 'Template gendcseparator' with your gendcseparator description
 */
GST_PLUGIN_DEFINE (
  GST_VERSION_MAJOR,
  GST_VERSION_MINOR,
  gendcseparator,       //plugin's symbol name
  "GenDC Separator",    //plugin's name
  gendcseparator_init,  //plugin's init funtion's name
  PACKAGE_VERSION, 
  GST_LICENSE, 
  GST_PACKAGE_NAME, 
  GST_PACKAGE_ORIGIN)
