
lib_LTLIBRARIES += src/lib/libegueb_js_sm.la

installed_headersdir = $(pkgincludedir)-$(VMAJ)
dist_installed_headers_DATA = \
src/lib/Egueb_Js_Sm.h

src_lib_libegueb_js_sm_la_SOURCES = \
src/lib/egueb_js_sm_private.h \
src/lib/egueb_js_sm.c

src_lib_libegueb_js_sm_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
@EGUEB_JS_SM_CFLAGS@

src_lib_libegueb_js_sm_la_LIBADD = \
@EGUEB_JS_SM_LIBS@

src_lib_libegueb_js_sm_la_LDFLAGS = -no-undefined -version-info @version_info@
