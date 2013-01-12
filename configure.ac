AC_INIT([par2query],[0.1.0],[arnstene@gmail.com],[par2query],[https://github.com/arnstene/par2query])
AC_PREREQ([2.59])

AM_INIT_AUTOMAKE([1.10 no-define])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_LANG_CPLUSPLUS
AC_PROG_INSTALL

dnl Checks for libraries.
AC_PROG_LIBTOOL
AM_PROG_LIBTOOL
AC_CHECK_LIB(stdc++, main,,AC_MSG_ERROR(You need libstdc++))
AC_CHECK_LIB(sigc-2.0, main,,AC_MSG_ERROR(You need libsigc++))

dnl Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDBOOL
AC_HEADER_STDC
AC_CHECK_HEADERS([stdio.h] [endian.h] [getopt.h])
PKG_CHECK_MODULES([SIGC], [sigc++-2.0] )
AC_CHECK_HEADERS([libpar2/par2repairer.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_C_BIGENDIAN
AC_C_CONST
AC_C_INLINE
AC_SYS_LARGEFILE
AC_FUNC_FSEEKO

dnl Checks for library functions.
AC_FUNC_MEMCMP
AC_CHECK_FUNCS([stricmp] [strcasecmp])
AC_CHECK_FUNCS([strchr] [memcpy])
AC_CHECK_FUNCS([getopt] [getopt_long])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
