libgwords_headers_dir = join_paths(gw_prefix, get_option('includedir'), 'words')

deps = [
  glib_dep,
  gio_dep,
]

libgwords_headers = [
  'languages/C/gw-segmenter-fallback.h',
  'gwords.h',
  'gw-dictionary.h',
  'gw-group.h',
  'gw-init.h',
  'gw-language.h',
  'gw-radix-tree.h',
  'gw-segmenter.h',
  'gw-string.h',
  'gw-types.h',
  'gw-utils.h',
]

libgwords_sources = files(
  'languages/C/gw-segmenter-fallback.c',
  'gw-dictionary.c',
  'gw-group.c',
  'gw-init.c',
  'gw-language.c',
  'gw-radix-tree.c',
  'gw-segmenter.c',
  'gw-string.c',
  'gw-utils.c',
)


###########
# Version #
###########

version_data = configuration_data()
version_data.set('VERSION', gw_version)
version_data.set('MAJOR_VERSION', gw_major_version)
version_data.set('MINOR_VERSION', gw_minor_version)
version_data.set('MICRO_VERSION', gw_micro_version)

configure_file(
          input: 'gw-version.h.in',
         output: 'gw-version.h',
    install_dir: join_paths(get_option('prefix'), get_option('includedir'), 'gwords'),
  configuration: version_data
)


#############
# libgwords #
#############

libgwords = shared_library(
              'words-' + gw_major_version,
              sources: libgwords_sources,
            soversion: gw_major_version,
              version: gw_major_version,
          install_dir: gw_libdir,
        install_rpath: gw_libdir_abs,
  include_directories: include_directories('.'),
         dependencies: deps,
              install: true
)

libgwords_dep = declare_dependency(
            link_with: libgwords,
  include_directories: include_directories('.'),
         dependencies: [ glib_dep, gio_dep ],
)

install_headers(libgwords_headers, install_dir: libgwords_headers_dir)


##############
# pkg-config #
##############

pkgg = import('pkgconfig')

pkgg.generate(
         name: 'libgwords',
  description: 'Natural Language Text Management for GNOME',
     filebase: 'words-' + gw_major_version,
      version: gw_version,
    libraries: libgwords,
     requires: ['glib-2.0', 'gobject-2.0', 'gio-2.0'],
  install_dir: join_paths(gw_libdir, 'pkgconfig')
)
