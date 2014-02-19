AUTOMAKE_OPTIONS = subdir-objects

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

AM_CPPFLAGS = $(LIBEBUS_CFLAGS) -I$(top_srcdir)/utils -Wall -Wextra

bin_PROGRAMS = ebusd

ebusd_SOURCES = src/main.cpp \
		utils/thread.cpp \
		utils/logger.cpp \
		utils/socket.cpp \
		utils/daemon.cpp \
		utils/appl.cpp

ebusd_LDADD = $(LIBEBUS_LIBS) \
		-lpthread

dist_noinst_SCRIPTS = autogen.sh
