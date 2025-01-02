project('gendc', 'c',
  version : run_command('git', 'describe', '--tags', '--abbrev=0').stdout().strip() + '-' + run_command('git', 'rev-parse',  '--short', 'HEAD').stdout().strip(),
  meson_version : '>= 1.1',
  license : 'MIT')

full_version =  run_command('git', 'describe', '--tags', '--abbrev=0').stdout().strip().strip('v')
version_parts = full_version.split('.')
gendc_api_version = version_parts[0] + '.' + version_parts[1]
message( 'API version: ' + gendc_api_version )
message( 'PACKAGE_VERSION: ' + full_version )

header_lib_install_dir = get_option('includedir')
gst_plugin_dir = get_option('libdir')/'gstreamer-1.0'

summary (
  {
  'prefix': get_option('prefix'),
  'GenDCSaparator(header library)':header_lib_install_dir,
  'gstgendcseparator(gst-plugin)':gst_plugin_dir,
  },
  section: 'Directories'
)

gst_enabled = get_option('gst-plugin')
summary (
  {
  'GStreamer plugin': gst_enabled,
  },
  section: 'Options'
)

# header library
install_subdir('gendc_cpp/gendc_separator', install_dir: header_lib_install_dir)
install_subdir('gendc_cpp/genicam', install_dir: header_lib_install_dir)
install_headers('gendc_common.h', install_dir: header_lib_install_dir)

# gst-plugin
gst_dep = [dependency('gstreamer-1.0', required: gst_enabled)]
subdir('gst', if_found: gst_dep)
