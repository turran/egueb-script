
lib_LTLIBRARIES += src/lib/libegueb_script.la

installed_headersdir = $(pkgincludedir)-$(VMAJ)
dist_installed_headers_DATA = \
src/lib/Egueb_Script.h

src_lib_libegueb_script_la_SOURCES = \
src/lib/egueb_script_private.h \
src/lib/egueb_script.c

if BUILD_JS_SM
src_lib_libegueb_script_la_SOURCES += \
src/lib/egueb_script_js_sm.c
endif

src_lib_libegueb_script_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
@EGUEB_SCRIPT_CFLAGS@

src_lib_libegueb_script_la_LIBADD = \
@EGUEB_SCRIPT_LIBS@

src_lib_libegueb_script_la_LDFLAGS = -no-undefined -version-info @version_info@
