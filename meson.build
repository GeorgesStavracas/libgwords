project(
  'libwords',
  'c',
  version : '0.1.0',
  license : 'LGPL3+',
  default_options : ['warning_level=1'],
  meson_version : '>=0.41.2'
)

words_version = meson.project_version()
varr = words_version.split('.')
words_major_version = varr[0]
words_minor_version = varr[1]
words_micro_version = varr[2]


###############
# Directories #
###############

words_prefix = get_option('prefix')
words_libdir = get_option('libdir')
words_libdir_abs = join_paths(words_prefix, words_libdir)


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
conf.set('WORDS_MAJOR_VERSION', words_major_version)
conf.set('WORDS_MINOR_VERSION', words_minor_version)
conf.set('WORDS_MICRO_VERSION', words_micro_version)
conf.set_quoted('PACKAGE_VERSION', words_version)
conf.set_quoted('GETTEXT_PACKAGE', meson.project_name())
conf.set_quoted('LOCALEDIR', get_option('localedir'))
conf.set_quoted('LOCALSTATEDIR', get_option('localstatedir'))

configure_file(
         output: 'config.h',
  configuration: conf
)

add_global_arguments('-DWORDS_COMPILATION', language : 'c')


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

output = '\n     Words ' + words_version + '\n'
output += '      =========================\n\n'
output += '          Source ...........................  ' + meson.source_root() + '\n'
output += '          Prefix ...........................  ' + words_prefix + '\n'
output += '          Libdir ...........................  ' + words_libdir_abs + '\n'
output += '          Compiler .........................  ' + meson.get_compiler('c').get_id() + '\n\n'
output += '      Now type "ninja -C ' + meson.build_root() + '" to build ' + meson.project_name() + '\n'

message(output)
