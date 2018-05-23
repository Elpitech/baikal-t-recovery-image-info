AC_PREREQ(2.69)
AC_INIT([Recovery Image Information parser], [1.0.0], [fancer.lancer@gmail.com])

AC_ARG_WITH(static, [  --with-static            enable static linking])

if test "$with_static" = "yes"
then
  AC_MSG_NOTICE([Compiling with staticaly linked libraries.])
  LIBS="$LIBS -static"
else
  AC_MSG_NOTICE([Compiling with dynamically linked libraries.])
fi

AC_ARG_VAR(RII_DEFAULT_NAME,  [ default Baikal-T ROM-image file name (/dev/mtd2 if not specified)])
if test "$RII_DEFAULT_NAME" == ""
then
  RII_DEFAULT_NAME="/dev/mtd2"
fi
AC_DEFINE_UNQUOTED([RII_DEFAULT_NAME], ["$RII_DEFAULT_NAME"],
                   [Default Baikal-T ROM-image file name])

AC_ARG_VAR(RII_DEFAULT_ADDR, [ default info-section offset (0 if not specified)])
if test "$RII_DEFAULT_ADDR" == ""
then
  RII_DEFAULT_ADDR="0x0U"
fi
AC_DEFINE_UNQUOTED([RII_DEFAULT_ADDR], [$RII_DEFAULT_ADDR],
                   [Default info-section offset])

AC_ARG_VAR(RII_MAGIC, [ info-section magic number (default 0xDEADBEEFBAADF00D)])
if test "$RII_MAGIC" == ""
then
  RII_MAGIC="0xDEADBEEFBAADF00DULL"
fi
AC_DEFINE_UNQUOTED([RII_MAGIC], [$RII_MAGIC],
                   [Info-section magic number])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/])
AC_CONFIG_HEADER([src/config.h])

AC_PROG_CC

#Getting user and host info
username=$(whoami)
AC_MSG_CHECKING([user ID])
AC_MSG_RESULT([$username])

hostname=$(uname -n)
AC_MSG_CHECKING([host name])
AC_MSG_RESULT([$hostname])

timestamp=$(date)
AC_MSG_CHECKING([current timestamp])
AC_MSG_RESULT([$timestamp])

AC_DEFINE_UNQUOTED([RII_TIMESTAMP],
                   "$username @ $hostname on $timestamp",
                   [Timestamp when ./configure was executed])

# Checks for header files.
AC_HEADER_STDC
# Skip malloc.h to prevent redefinition of HAVE_MALLOC_H on some platforms
AC_CHECK_HEADERS([stdint.h stdarg.h unistd.h stdlib.h getopt.h stdio.h errno.h sys/types.h sys/stat.h fcntl.h endian.h time.h], [], [AC_MSG_FAILURE([Missing some header files.])])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_HEADER_TIME
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([open lseek read close malloc free])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
