AC_PREREQ(2.61)

# package version number (not shared library version)
# odd micro numbers indicate in-progress development
# even micro numbers indicate released versions
m4_define(Hello_Yocto_version_major, 0)
m4_define(Hello_Yocto_version_minor, 1)
m4_define(Hello_Yocto_version_micro, 0)

m4_define([Hello_Yocto_version],
          [Hello_Yocto_version_major.Hello_Yocto_version_minor.Hello_Yocto_version_micro])
m4_define([Hello_Yocto_api_version],
          [Hello_Yocto_version_major.Hello_Yocto_version_minor])

AC_INIT(Hello_Yocto, Hello_Yocto_version)
AM_INIT_AUTOMAKE(Hello_Yocto, Hello_Yocto_version)
AC_CONFIG_HEADERS(config.h)

# Checks for programs.
AC_PROG_LIBTOOL
AM_PROG_CC_C_O
AC_PROG_INSTALL

AC_SUBST(Hello_Yocto_CFLAGS)
AC_SUBST(Hello_Yocto_LIBS)

AC_OUTPUT([
Makefile
src/Makefile
])

echo ""
echo "   Hello_Yocto $VERSION"
echo "   ====================="
echo ""
echo "   To build the project, run \"make\""
echo ""
