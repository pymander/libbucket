# Makefile.am for libbucket source code.
# Run automake on this file to create Makefile.in

lib_LTLIBRARIES = libbucket.la
pkginclude_HEADERS = bucket.h strlib.h
libbucket_la_SOURCES = bucket.c strlib.c
libbucket_la_LDFLAGS = -version-info 1:4

# Special install directory.
pkgincludedir = $(includedir)/bucket
