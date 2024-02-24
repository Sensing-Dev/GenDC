
src = [
  'gendc.cpp'
]

gendc = library('gendc',
  src,
  c_args : gst_plugins_args,
  include_directories : [include, separator],
#   dependencies : [gstbase_dep],
  install : true,
  install_dir : plugins_install_dir,
)
plugins += [gendc]