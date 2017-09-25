unitex_pb_sources = files(
  'Alphabet.c',
  'DELA.c',
  'FileName.c',
  'lexicon-bib.c',
  'String_hash.c',
  'unicode.c',
)

unitex_pb = static_library(
      'unitex-pt-br',
       sources: unitex_pb_sources,
           pic: true,
)

unitex_pb_dep = declare_dependency(
            link_with: unitex_pb,
  include_directories: include_directories('.'),
)
