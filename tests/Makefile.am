check_PROGRAMS =

common_cflags = \
	$(AM_CFLAGS) \
	$(WORDS_CFLAGS) \
	-I$(top_builddir)/src

common_ldadd = \
	$(CONTRIB_LDADD) \
	$(top_builddir)/src/libwords-@API_VERSION@.la

# Dictionary
check_PROGRAMS += test-dictionary
test_dictionary_CPPFLAGS = $(common_cppflags)
test_dictionary_CFLAGS = $(common_cflags)
test_dictionary_LDFLAGS = $(common_libs)
test_dictionary_LDADD = $(common_ldadd)
test_dictionary_SOURCES = test-dictionary.c

# Radix tree
check_PROGRAMS += test-radix-tree
test_radix_tree_CPPFLAGS = $(common_cppflags)
test_radix_tree_CFLAGS = $(common_cflags)
test_radix_tree_LDFLAGS = $(common_libs)
test_radix_tree_LDADD = $(common_ldadd)
test_radix_tree_SOURCES = test-radix-tree.c

TESTS = $(check_PROGRAMS)

EXTRA_DIST = \
	english-words.txt

-include $(top_srcdir)/git.mk
