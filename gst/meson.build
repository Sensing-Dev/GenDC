project('gendc', 'c',
  version : '0.1.1',
  meson_version : '>= 1.1',
  license : 'MIT')

gst_dep = dependency('gstreamer-1.0')

plugin_c_args = ['-DHAVE_CONFIG_H']

cdata = configuration_data()
cdata.set_quoted('PACKAGE_VERSION', '0.1.1')
cdata.set_quoted('PACKAGE', 'gendc-separator')
cdata.set_quoted('GST_LICENSE', 'MIT')
cdata.set_quoted('GST_PACKAGE_NAME', 'GenDC Separator')
cdata.set_quoted('GST_PACKAGE_ORIGIN', 'https://github.com/Sensing-Dev/')
configure_file(output : 'config.h', configuration : cdata)

# The gendcseparator Plugin
 gstgendcseparator_sources = [
  'src/gstgendcseparator.c',
  ]

gstgendcseparatorexample = library('gstgendcseparator',
  gstgendcseparator_sources,
  c_args: plugin_c_args,
  dependencies : [gst_dep],
  install : true,
  install_dir : get_option('prefix'),
)
