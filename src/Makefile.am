lib_LTLIBRARIES = libwords-@API_VERSION@.la

libwords_@API_VERSION@_la_SOURCES = \
	words-dictionary.c \
	words-dictionary.h \
	words-group.c \
	words-group.h \
	words-radix-tree.c \
	words-radix-tree.h \
	words-version.h \
	words.h \
	$(NULL)

libwords_@API_VERSION@_la_CFLAGS = \
	$(WORDS_CFLAGS) \
	$(WARN_CFLAGS) \
	-Wno-sign-compare \
	$(NULL)

libwords_@API_VERSION@_la_LIBADD = \
	$(WORDS_LIBS) \
	$(NULL)

libwords_@API_VERSION@_la_LDFLAGS = \
	$(WARN_LDFLAGS) \
	$(NULL)

-include $(top_srcdir)/git.mk
