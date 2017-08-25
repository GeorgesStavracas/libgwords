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
)

test('test-dictionary', test_dictionary)


############
# Language #
############

test_language = executable(
  'test-language',
  'test-language.c',
  dependencies: libgwords_dep,
)

test('test-language', test_language)


##############
# Radix tree #
##############

test_radix_tree = executable(
  'test-radix-tree',
  'test-radix-tree.c',
  dependencies: libgwords_dep,
)

test('test-radix-tree', test_radix_tree)


##############
# Segmenters #
##############

test_segmenters = executable(
  'test-segmenters',
  'test-segmenters.c',
  dependencies: libgwords_dep,
)

test('test-segmenters', test_segmenters)


##########
# String #
##########

test_string = executable(
  'test-string',
  'test-string.c',
  dependencies: libgwords_dep,
)

test('test-string', test_string)
