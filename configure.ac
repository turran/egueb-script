
### Version

m4_define([v_maj], [0])
m4_define([v_min], [0])
m4_define([v_mic], [1])
m4_define([v_ver], [v_maj.v_min.v_mic])

m4_define([lt_cur], m4_eval(v_maj + v_min))
m4_define([lt_rev], v_mic)
m4_define([lt_age], v_min)

AC_INIT([egueb_script], [v_ver], [enesim-devel@googlegroups.com])
AC_PREREQ([2.60])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign 1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_PREREQ([2.2])
LT_INIT([win32-dll])

version_info="lt_cur:lt_rev:lt_age"
AC_SUBST([version_info])

AC_DEFINE_UNQUOTED([VERSION_MAJOR], [v_maj], [Major version])
AC_DEFINE_UNQUOTED([VERSION_MINOR], [v_min], [Minor version])
AC_DEFINE_UNQUOTED([VERSION_MICRO], [v_mic], [Micro version])

VMAJ=v_maj
AC_SUBST([VMAJ])

## Add parameters for aclocal (keep in sync with Makefile.am)
AC_SUBST(ACLOCAL_AMFLAGS, "-I m4 -I common/m4")

### Additional options to configure

### default values

AC_CANONICAL_HOST

have_win32="no"
case "$host_os" in
   mingw*)
      have_win32="yes"
   ;;
esac

AM_CONDITIONAL([HAVE_WIN32], [test "x${have_win32}" = "xyes"])

### Checks for programs

AC_PROG_CC
AM_PROG_CC_C_O
ENS_CHECK_COMPILER_FLAGS([ENDER], [-Wall])

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if ! $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   AC_MSG_ERROR([pkg-config >= 0.22 is required])
fi

# doxygen program for documentation building
ENS_CHECK_DOXYGEN([build_doc="yes"], [build_doc="no"])

LT_SYS_MODULE_EXT

### Core dependencies
egueb_script_requirements="egueb-dom >= 0.0.23"
egueb_script_requirements_private="ender >= 0.0.8"

### Checks for libraries
### JavaScript SpiderMonkey backend
js_sm_requirements="ender-js-sm >= 0.0.1 mozjs185"
ENS_ENABLE_ARG([js_sm], [JavaScript SpiderMonkey support], [yes])
PKG_CHECK_EXISTS([${js_sm_requirements}], [have_js_sm="yes"], [have_js_sm="no"])

build_js_sm="no"
if test "x${have_js_sm}" = "xyes" -a "x${want_js_sm}" = "xyes"; then
  build_js_sm="yes"
  egueb_script_requirements_private="${egueb_script_requirements_private} ${js_sm_requirements}"
  AC_DEFINE([BUILD_JS_SM], [1], [Have support for Js SM])
fi
AM_CONDITIONAL([BUILD_JS_SM], [test "x${build_js_sm}" = "xyes"])



PKG_CHECK_MODULES([REQUIREMENTS], [${egueb_script_requirements} ${egueb_script_requirements_private}])
AC_SUBST([egueb_script_requirements])
AC_SUBST([egueb_script_requirements_private])

EGUEB_SCRIPT_CFLAGS="${EGUEB_SCRIPT_CFLAGS} ${REQUIREMENTS_CFLAGS}"
EGUEB_SCRIPT_LIBS="${REQUIREMENTS_LIBS}"
AC_SUBST(EGUEB_SCRIPT_CFLAGS)
AC_SUBST(EGUEB_SCRIPT_LIBS)

## Make the debug preprocessor configurable

AC_CONFIG_FILES([
Makefile
])

AC_OUTPUT

#####################################################################
## Info

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo
echo "Configuration Options Summary:"
echo
echo "Compilation.................: make (or gmake)"
echo "  CPPFLAGS..................: $CPPFLAGS"
echo "  CFLAGS....................: $CFLAGS"
echo "  LDFLAGS...................: $LDFLAGS"
echo
echo "Backends:"
echo "  JavaScript SpiderMonkey...: ${build_js_sm}"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo
