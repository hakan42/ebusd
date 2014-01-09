AUTOMAKE_OPTIONS = subdir-objects

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

AM_CPPFLAGS = $(LIBEBUS_CFLAGS)

bin_PROGRAMS = ebusd

ebusd_SOURCES = src/main.cpp

ebusd_LDADD = $(LIBEBUS_LIBS)

dist_noinst_SCRIPTS = autogen.sh
