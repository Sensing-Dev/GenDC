plugin_c_args = ['-DHAVE_CONFIG_H']

cdata = configuration_data()
cdata.set_quoted('PACKAGE_VERSION', gst_version)
cdata.set_quoted('PACKAGE', 'gst-gendc-parser-plugin')
cdata.set_quoted('GST_LICENSE', 'LGPL')
cdata.set_quoted('GST_API_VERSION', api_version)
cdata.set_quoted('GST_PACKAGE_NAME', 'GenDC Parser Plug-ins')
cdata.set_quoted('GST_PACKAGE_ORIGIN', 'https://gstreamer.freedesktop.org')
configure_file(output : 'config.h', configuration : cdata)

gstaudio_dep = dependency('gstreamer-audio-1.0',
    fallback: ['gst-plugins-base', 'audio_dep'])

# Plugin 1
plugin_sources = [
  'src/gstplugin.c'
  ]

gstpluginexample = library('gstplugin',
  plugin_sources,
  c_args: plugin_c_args,
  dependencies : [gst_dep],
  install : true,
  install_dir : plugins_install_dir,
)


# The gendc_parser Plugin
 gstgendc_parser_sources = [
  'src/gstgendc_parser.c',
  ]

gstgendc_parserexample = library('gstgendc_parser',
  gstgendc_parser_sources,
  c_args: plugin_c_args,
  dependencies : [gst_dep, gstbase_dep],
  install : true,
  install_dir : plugins_install_dir,
)
