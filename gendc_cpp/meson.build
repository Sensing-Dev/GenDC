project('gendc', 'c',
  version : '1.10',
  meson_version : '>= 1.1',
  license : 'MIT',
  default_options : [ 'warning_level=1',
                      'buildtype=debugoptimized' ])

gst_version = meson.project_version()
api_version = '1.0'

have_cxx = add_languages('cpp', native: false, required: false)
if have_cxx
  cxx = meson.get_compiler('cpp')
endif

plugins_install_dir = join_paths(get_option('libdir'), 'gstreamer-1.0')
plugins = []
gendc_dep = []
static_build = get_option('default_library') == 'static'

cc = meson.get_compiler('c')
host_system = host_machine.system()

message('Compiler :', cc.get_id())
if cc.get_id() == 'msvc'
  msvc_args = [
      # Ignore several spurious warnings for things gstreamer does very commonly
      # If a warning is completely useless and spammy, use '/wdXXXX' to suppress it
      # If a warning is harmless but hard to fix, use '/woXXXX' so it's shown once
      # NOTE: Only add warnings here if you are sure they're spurious
      '/wd4018', # implicit signed/unsigned conversion
      '/wd4146', # unary minus on unsigned (beware INT_MIN)
      '/wd4244', # lossy type conversion (e.g. double -> int)
      '/wd4305', # truncating type conversion (e.g. double -> float)
      cc.get_supported_arguments(['/utf-8']), # set the input encoding to utf-8
  ]
  if have_cxx
    add_project_arguments(msvc_args, language: ['c', 'cpp'])
  else
    add_project_arguments(msvc_args, language: 'c')
  endif
endif


if cc.has_link_argument('-Wl,-Bsymbolic-functions')
  add_project_link_arguments('-Wl,-Bsymbolic-functions', language : 'c')
endif
if have_cxx and cxx.has_link_argument('-Wl,-Bsymbolic-functions')
  add_project_link_arguments('-Wl,-Bsymbolic-functions', language : 'cpp')
endif
# glib doesn't support unloading, which means that unloading and reloading
# any library that registers static types will fail
if cc.has_link_argument('-Wl,-z,nodelete')
  add_project_link_arguments('-Wl,-z,nodelete', language: 'c')
endif
if have_cxx and cxx.has_link_argument('-Wl,-z,nodelete')
  add_project_link_arguments('-Wl,-z,nodelete', language: 'cpp')
endif

# Symbol visibility
if cc.has_argument('-fvisibility=hidden')
  add_project_arguments('-fvisibility=hidden', language: 'c')
endif

# Disable strict aliasing
if cc.has_argument('-fno-strict-aliasing')
  add_project_arguments('-fno-strict-aliasing', language: 'c')
endif

gendcseparator_dir = get_option('gendcseparator-dir')

gst_plugins_args = ['-DHAVE_CONFIG_H']
configinc = include_directories('.')
separator = include_directories(gendcseparator_dir)
include = include_directories('include')


cdata = configuration_data()
cdata.set_quoted('VERSION', gst_version)
cdata.set_quoted('PACKAGE_VERSION', gst_version)
cdata.set_quoted('PACKAGE', 'gst-gendc-plugin')
cdata.set_quoted('GST_LICENSE', 'MIT')
cdata.set_quoted('GST_API_VERSION', api_version)
cdata.set_quoted('GST_PACKAGE_NAME', 'GenDC Plug-ins')
cdata.set_quoted('GST_PACKAGE_ORIGIN', 'https://gstreamer.freedesktop.org')
configure_file(output : 'config.h', configuration : cdata)


warning_flags = [
  '-Wmissing-declarations',
  '-Wredundant-decls',
  '-Wwrite-strings',
  '-Winit-self',
  '-Wmissing-include-dirs',
  '-Wno-multichar',
  '-Wvla',
  '-Wpointer-arith',
]
warning_c_flags = [
  '-Wmissing-prototypes',
  '-Wold-style-definition',
  '-Waggregate-return',
]
foreach extra_arg : warning_flags
  if cc.has_argument (extra_arg)
    add_project_arguments([extra_arg], language: 'c')
  endif
endforeach
foreach extra_arg : warning_c_flags
  if cc.has_argument (extra_arg)
    add_project_arguments([extra_arg], language: 'c')
  endif
endforeach

gst_dep = dependency('gstreamer-1.0', version : '>=1.19',
    required : true, fallback : ['gstreamer', 'gst_dep'])
gstbase_dep = dependency('gstreamer-base-1.0', version : '>=1.19',
  fallback : ['gstreamer', 'gst_base_dep'])
gstcheck_dep = dependency('gstreamer-check-1.0', version : '>=1.19',
  required : get_option('tests'),
  fallback : ['gstreamer', 'gst_check_dep'])
libm = cc.find_library('m', required : false)


subdir('gendc')

gendc_dep = declare_dependency(link_with : gendc,
  include_directories : [include, separator]
  )

subdir('gst')

## TODO
## Find Check using dependency function
check_dep = dependency('check', required : false)
if not check_dep.found()
   check_dep = cc.find_library('check', required : true)
endif
subdir('tests')

plugin_names = []
gst_plugins = []
pkgconfig = import('pkgconfig')
plugins_pkgconfig_install_dir = join_paths(plugins_install_dir, 'pkgconfig')

foreach plugin: plugins
  pkgconfig.generate(plugin, install_dir: plugins_pkgconfig_install_dir)
  dep = declare_dependency(link_with: plugin, variables: {'full_path': plugin.full_path()})
  meson.override_dependency(plugin.name(), dep)
  gst_plugins += [dep]
  if plugin.name().startswith('gst')
    plugin_names += [plugin.name().substring(3)]
  else
    plugin_names += [plugin.name()]
  endif
endforeach

# pkgconfig.generate(gendc_dep, install_dir: plugins_pkgconfig_install_dir)

summary({
    'Plugins': plugin_names,
}, list_sep: ', ')
