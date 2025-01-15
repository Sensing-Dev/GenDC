plugin_c_args = ['-DHAVE_CONFIG_H']

cdata = configuration_data()
cdata.set_quoted('PACKAGE_VERSION', full_version)
cdata.set_quoted('PACKAGE', 'gendc-separator')
cdata.set_quoted('GST_LICENSE', 'LGPL')
cdata.set_quoted('GST_PACKAGE_NAME', 'GenDC Separator')
cdata.set_quoted('GST_PACKAGE_ORIGIN', 'https://github.com/Sensing-Dev/')
configure_file(output : 'config.h', configuration : cdata)

# The gendcseparator Plugin
 gstgendcseparator_sources = [
  'src/gstgendcseparator.c',
  ]

gst_plugin_filename = 'gstgendcseparator.@0@'.format (gendc_api_version)

gstgendcseparatorexample = library(gst_plugin_filename,
  gstgendcseparator_sources,
  c_args: plugin_c_args,
  dependencies : [gst_dep],
  install : true,
  install_dir : gst_plugin_dir,
)
