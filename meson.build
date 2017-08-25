project(
  'libgwords',
  'c',
  version : '0.1.0',
  license : 'LGPL3+',
  default_options : ['warning_level=1'],
  meson_version : '>=0.41.2'
)

gw_version = meson.project_version()
varr = gw_version.split('.')
gw_major_version = varr[0]
gw_minor_version = varr[1]
gw_micro_version = varr[2]


###############
# Directories #
###############

gw_prefix = get_option('prefix')
gw_libdir = get_option('libdir')
gw_libdir_abs = join_paths(gw_prefix, gw_libdir)


################
# Dependencies #
################

glib_ver = '>= 2.45.8'
glib_dep = dependency('glib-2.0', version : glib_ver)
gio_dep = dependency('gio-2.0', version : glib_ver)

gnome = import('gnome')
i18n = import('i18n')


############
# config.h #
############

conf = configuration_data()
conf.set('GW_MAJOR_VERSION', gw_major_version)
conf.set('GW_MINOR_VERSION', gw_minor_version)
conf.set('GW_MICRO_VERSION', gw_micro_version)
conf.set_quoted('PACKAGE_VERSION', gw_version)
conf.set_quoted('GETTEXT_PACKAGE', meson.project_name())
conf.set_quoted('LOCALEDIR', get_option('localedir'))
conf.set_quoted('LOCALSTATEDIR', get_option('localstatedir'))

configure_file(
         output: 'config.h',
  configuration: conf
)

add_global_arguments('-DGW_COMPILATION', language : 'c')


###########
# Subdirs #
###########

subdir('data')
subdir('po')
subdir('src')
subdir('tests')


##########
# Output #
##########

output = '\n     Words ' + gw_version + '\n'
output += '      =========================\n\n'
output += '          Source ...........................  ' + meson.source_root() + '\n'
output += '          Prefix ...........................  ' + gw_prefix + '\n'
output += '          Libdir ...........................  ' + gw_libdir_abs + '\n'
output += '          Compiler .........................  ' + meson.get_compiler('c').get_id() + '\n\n'
output += '      Now type "ninja -C ' + meson.build_root() + '" to build ' + meson.project_name() + '\n'

message(output)
