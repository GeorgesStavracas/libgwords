words_headers_dir = join_paths(words_prefix, get_option('includedir'), 'words')

deps = [
  glib_dep,
  gio_dep,
]

word_headers = [
  'words.h',
  'words-dictionary.h',
  'words-group.h',
  'words-language.h',
  'words-radix-tree.h',
  'words-segmenter.h',
]

word_sources = files(
  'words-dictionary.c',
  'words-group.c',
  'words-language.c',
  'words-radix-tree.c',
  'words-segmenter.c',
)


###########
# Version #
###########

version_data = configuration_data()
version_data.set('VERSION', words_version)
version_data.set('MAJOR_VERSION', words_major_version)
version_data.set('MINOR_VERSION', words_minor_version)
version_data.set('MICRO_VERSION', words_micro_version)

configure_file(
          input: 'words-version.h.in',
         output: 'words-version.h',
    install_dir: join_paths(get_option('prefix'), get_option('includedir'), 'words'),
  configuration: version_data
)


############
# libwords #
############

libwords = shared_library(
              'words-' + words_major_version,
              sources: word_sources,
            soversion: words_major_version,
              version: words_major_version,
          install_dir: words_libdir,
        install_rpath: words_libdir_abs,
  include_directories: include_directories('.'),
         dependencies: deps,
              install: true
)

libwords_dep = declare_dependency(
            link_with: libwords,
  include_directories: include_directories('.'),
         dependencies: [ glib_dep, gio_dep ],
)

install_headers(word_headers, install_dir: words_headers_dir)


##############
# pkg-config #
##############

pkgg = import('pkgconfig')

pkgg.generate(
         name: 'libwords',
  description: 'Natural Language Text Management for GNOME',
     filebase: 'words-' + words_major_version,
      version: words_version,
    libraries: libwords,
     requires: ['glib-2.0', 'gobject-2.0', 'gio-2.0'],
  install_dir: join_paths(words_libdir, 'pkgconfig')
)
