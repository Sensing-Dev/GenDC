
gendcparse_src = [
  'gstgendcparse.c'
]

gendcparse_headers = [
  'gstgendcparse.h'
]

gstgendcparse = library('gstgendcparse',
  gendcparse_src,gendcparse_headers,
  c_args : gst_plugins_args,
  include_directories : [configinc, separator],
  dependencies : [gstbase_dep],
  install : true,
  install_dir : plugins_install_dir,
)
plugins += [gstgendcparse]