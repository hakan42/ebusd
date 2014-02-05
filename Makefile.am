AUTOMAKE_OPTIONS = subdir-objects

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

AM_CPPFLAGS = $(LIBEBUS_CFLAGS) -Wall -Wextra

bin_PROGRAMS = ebusd

ebusd_SOURCES = src/main.cpp \
		utils/wqueue.hpp \
		utils/thread.hpp \
		utils/thread.cpp \		
		utils/logger.hpp \
		utils/logger.cpp

ebusd_LDADD = $(LIBEBUS_LIBS) \
		-lpthread

dist_noinst_SCRIPTS = autogen.sh
