
gendcparse_src = [
  'gstgendcparse.c',
  'gstpartparse.c',
  'gstcomponentparse.c',
]

gendcparse_headers = [
  'gstgendcparse.h',
  'gstpartparse.h',
  'gstcomponentparse.h',
]

# gendc_dep = declare_dependency(link_with : gendc,
#   include_directories : [include, separator]
#   )

gstgendcparse = library('gstgendcparse',
  gendcparse_src,gendcparse_headers,
  c_args : gst_plugins_args,
  include_directories : [include,configinc, separator],
  dependencies : [gstbase_dep,gendc_dep],
  install : true,
  install_dir : plugins_install_dir,
)
plugins += [gstgendcparse]