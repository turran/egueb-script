AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4 -I common/m4

MAINTAINERCLEANFILES = \
Makefile.in \
aclocal.m4 \
build-stamp \
compile \
config.guess \
config.h \
config.h.in \
config.h.in~ \
config.sub \
configure \
configure-stamp \
depcomp \
install-sh \
ltconfig \
ltmain.sh \
missing \
mkinstalldirs \
stamp-h.in \
$(PACKAGE)-$(VERSION).tar.bz2 \
$(PACKAGE)-$(VERSION).tar.gz \
m4/libtool.m4 \
m4/lt~obsolete.m4 \
m4/ltoptions.m4 \
m4/ltsugar.m4 \
m4/ltversion.m4

lib_LTLIBRARIES =
check_PROGRAMS =
EXTRA_DIST =

### Library

include src/lib/Makefile.mk

EXTRA_DIST += \
AUTHORS \
COPYING \
README.md \
egueb-script.pc.in \
m4/efl_dlfcn.m4 \
m4/ens_doxygen.m4

sed_process = \
$(SED) \
-e 's,@VERSION\@,$(VERSION),g' \
-e 's,@VMAJ\@,$(VMAJ),g' \
-e 's,@prefix\@,$(prefix),g' \
-e 's,@exec_prefix\@,$(exec_prefix),g' \
-e 's,@libdir\@,$(libdir),g' \
-e 's,@includedir\@,$(includedir),g' \
-e 's,@pkgincludedir\@,$(pkgincludedir),g' \
-e 's,@pkglibdir\@,$(pkglibdir),g' \
-e 's,@egueb_script_requirements_private\@,$(egueb_script_requirements_private),g' \
-e 's,@egueb_script_requirements\@,$(egueb_script_requirements),g' \
< $< > $@ || rm $@

pc_verbose = $(pc_verbose_@AM_V@)
pc_verbose_ = $(pc_verbose_@AM_DEFAULT_V@)
pc_verbose_0 = @echo "  PC      " $@;

%.pc: %.pc.in Makefile
	@rm -f $@
	$(pc_verbose)$(sed_process)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA =

pkgconfig_DATA += egueb-script.pc


