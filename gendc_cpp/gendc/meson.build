
src = [
  'gendc.cpp'
]

gendc = static_library('gendc',
  src,
  c_args : gst_plugins_args,
  include_directories : [include, separator],
#   dependencies : [gstbase_dep],
  install : true,
  install_dir : plugins_install_dir,
)
# pkgconfig.generate(gendc, install_dir : plugins_pkgconfig_install_dir)

gendc_dep += [gendc]