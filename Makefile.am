ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

SUBDIRS = . doc

DISTCHECK_CONFIGURE_FLAGS =
MAINTAINERCLEANFILES =
EXTRA_DIST =
CLEANFILES =
noinst_PROGRAMS =

lib_LTLIBRARIES = libuber-1.0.la

uberdir = $(pkgincludedir)/uber
uber_HEADERS = $(INST_H_FILES)

INST_H_FILES =				\
	uber/uber.h			\
	uber/uber-graph.h		\
	uber/uber-frame-source.h	\
	uber/uber-heat-map.h		\
	uber/uber-line-graph.h		\
	uber/uber-label.h		\
	uber/uber-range.h		\
	uber/uber-scale.h		\
	uber/uber-scatter.h		\
	uber/uber-timeout-interval.h

NOINST_H_FILES =			\
	uber/uber-window.h		\
	uber/g-ring.h

libuber_1_0_la_SOURCES = 		\
	$(INST_H_FILES)			\
	$(NOINST_H_FILES)		\
	uber/uber-graph.c		\
	uber/uber-frame-source.c	\
	uber/uber-heat-map.c		\
	uber/uber-line-graph.c		\
	uber/uber-label.c		\
	uber/uber-range.c		\
	uber/uber-scale.c		\
	uber/uber-scatter.c		\
	uber/uber-timeout-interval.c	\
	uber/uber-window.c		\
	uber/g-ring.c

libuber_1_0_la_CFLAGS =
libuber_1_0_la_CFLAGS += $(GTK_CFLAGS)

libuber_1_0_la_LIBADD =
libuber_1_0_la_LIBADD += $(GTK_LIBS)

# Introspection
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --warn-all
INTROSPECTION_COMPILER_ARGS =
DISTCHECK_CONFIGURE_FLAGS += --enable-introspection
EXTRA_DIST += introspection.m4

if HAVE_INTROSPECTION
Uber-1.0.gir: libuber-1.0.la
Uber_1_0_gir_INCLUDES = Gtk-3.0
Uber_1_0_gir_CFLAGS = $(GTK_CFLAGS) $(libuber_1_0_la_CPPFLAGS)
Uber_1_0_gir_LIBS = libuber-1.0.la
Uber_1_0_gir_FILES = $(libuber_1_0_la_SOURCES) $(INST_H_FILES)
Uber_1_0_gir_NAMESPACE = Uber
Uber_1_0_gir_EXPORT_PACKAGES = uber-1.0
Uber_1_0_gir_SCANNERFLAGS = 	\
	--nsversion=1.0 		\
	--symbol-prefix=uber 	\
	--c-include="uber/uber.h"

INTROSPECTION_GIRS += Uber-1.0.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)

DISTCHECK_CONFIGURE_FLAGS += --enable-introspection
EXTRA_DIST += introspection.m4
endif

noinst_PROGRAMS += demos/c/simple demos/c/sysmon

demos_c_simple_SOURCES = 	\
	demos/c/simple.c

demos_c_simple_CPPFLAGS = 	\
	-I$(top_srcdir)/ 	\
	-I$(top_srcdir)/uber	\
	$(AM_CPPFLAGS)

demos_c_simple_CFLAGS = 	\
	$(GTK_CFLAGS) 		\
	$(AM_CFLAGS)

demos_c_simple_LDADD = 	\
	$(top_builddir)/libuber-1.0.la \
	$(GTK_LIBS) 		\
	$(AM_LDADD)

demos_c_sysmon_SOURCES = 	\
	demos/c/blktrace.h \
	demos/c/sysmon.h \
	demos/c/blktrace.c \
	demos/c/sysmon.c \
	demos/c/system-monitor.c

demos_c_sysmon_CPPFLAGS = 	\
	-I$(top_srcdir)/ 	\
	-I$(top_srcdir)/uber	\
	$(AM_CPPFLAGS)

demos_c_sysmon_CFLAGS = 	\
	$(GTK_CFLAGS) 		\
	$(AM_CFLAGS)

demos_c_sysmon_LDADD = 	\
	$(top_builddir)/libuber-1.0.la \
	$(GTK_LIBS) 		\
    -ldl \
	$(AM_LDADD)


