AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

bin_PROGRAMS = \
	par2query

par2query_SOURCES = \
	src/par2query.cpp

dist_noinst_SCRIPTS = autogen.sh

LDADD = -lstdc++ $(SIGC_LIBS)
INCLUDES = $(SIGC_CFLAGS)
AM_CXXFLAGS = -Wall -Wextra -Werror

