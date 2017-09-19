#############################################
# HACK!                                     #
# Copy the test data files to the build dir #
#############################################

configure_file(
          input: 'english-words.txt',
         output: 'english-words.txt',
  configuration: configuration_data()
)

#################
# test-config.h #
#################

test_conf = configuration_data()
test_conf.set_quoted('TESTDIR', join_paths(meson.build_root(), 'tests'))

configure_file(
         output: 'test-config.h',
  configuration: test_conf
)


##############
# Dictionary #
##############

test_dictionary = executable(
    'test-dictionary',
  'test-dictionary.c',
         dependencies: libgwords_dep,
  include_directories: gw_include_dirs,
)

test('test-dictionary', test_dictionary)


############
# Document #
############

test_document = executable(
  'test-document',
  [ 'stub-modifier.c', 'test-document.c' ],
         dependencies: libgwords_dep,
  include_directories: gw_include_dirs,
)

test('test-document', test_document)

############
# Language #
############

test_language = executable(
  'test-language',
  'test-language.c',
         dependencies: libgwords_dep,
  include_directories: gw_include_dirs,
)

test('test-language', test_language)


##############
# Radix tree #
##############

test_radix_tree = executable(
  'test-radix-tree',
  'test-radix-tree.c',
         dependencies: libgwords_dep,
  include_directories: gw_include_dirs,
)

test('test-radix-tree', test_radix_tree, timeout: 60)


##############
# Segmenters #
##############

test_segmenters = executable(
  'test-segmenters',
  'test-segmenters.c',
         dependencies: libgwords_dep,
  include_directories: gw_include_dirs,
)

test('test-segmenters', test_segmenters)


##########
# String #
##########

test_string = executable(
  'test-string',
  'test-string.c',
         dependencies: libgwords_dep,
  include_directories: gw_include_dirs,
)

test('test-string', test_string)


#####################
# String comparator #
#####################

test_string_comparator = executable(
  'test-string-comparator',
  'test-string-comparator.c',
         dependencies: libgwords_dep,
  include_directories: gw_include_dirs,
)

test('test-string-comparator', test_string_comparator, timeout: 60)


#################
# String editor #
#################

test_string_editor = executable(
  'test-string-editor',
  'test-string-editor.c',
            dependencies: libgwords_dep,
     include_directories: gw_include_dirs,
)

test('test-string-editor', test_string_editor, timeout: 60)
