#############################################
# HACK!                                     #
# Copy the test data files to the build dir #
#############################################

configure_file(
          input: 'english-words.txt',
         output: 'english-words.txt',
  configuration: configuration_data()
)


##############
# Dictionary #
##############

test_dictionary = executable(
  'test-dictionary',
  'test-dictionary.c',
  dependencies: libwords_dep,
)

test('test-dictionary', test_dictionary)


##############
# Radix tree #
##############

test_radix_tree = executable(
  'test-radix-tree',
  'test-radix-tree.c',
  dependencies: libwords_dep,
)

test('test-radix-tree', test_radix_tree)
