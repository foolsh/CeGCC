#! /usr/bin/env bash

BASE_DIRECTORY=`dirname $0`
BASE_DIRECTORY=`(cd ${BASE_DIRECTORY}; cd ..; pwd)`
ME=`basename $0`

# FIXME: some components need this (mingwdll), but they shouldn't.
export BASE_DIRECTORY

#
# Initializations.
#
export BUILD_DIR=`pwd`

ac_default_prefix="/opt/mingw32ce"

if test -z "${gcc_src}"; then
    export gcc_src=gcc-4.4.0
fi

# The list of components, in build order.  There's a build_FOO
# function for each of these components
COMPONENTS=( binutils bootstrap_gcc mingw w32api gcc profile dlls docs )
COMPONENTS_NUM=${#COMPONENTS[*]}

# Build comma separated list of components, for user display.
for ((i=0;i<$COMPONENTS_NUM;i++)); do
    if [ $i = 0 ]; then
	COMPONENTS_COMMA_LIST=${COMPONENTS[${i}]}
    else
	COMPONENTS_COMMA_LIST="${COMPONENTS_COMMA_LIST}, ${COMPONENTS[${i}]}"
    fi
done

usage()
{
    cat << _ACEOF

$ME builds the mingw32ce toolchain.

Usage: $0 [OPTIONS] ...

  -h, --help              print this help, then exit
  --prefix=PREFIX         install toolchain in PREFIX
			  [$ac_default_prefix]
  --host=HOST             host on which the toolchain will run [BUILD]
  -j, --parallelism PARALLELISM  Pass PARALLELISM as -jPARALLELISM
                          to make invocations.
  --components=LIST       specify which components to build
                          valid components are: ${COMPONENTS_COMMA_LIST}
			  [all]

Report bugs to <cegcc-devel@lists.sourceforge.net>.
_ACEOF

}

ac_prev=
for ac_option
do
  # If the previous option needs an argument, assign it.
  if test -n "$ac_prev"; then
    eval "$ac_prev=\$ac_option"
    ac_prev=
    continue
  fi

  ac_optarg=`expr "x$ac_option" : 'x[^=]*=\(.*\)'`

  case $ac_option in

  -help | --help | --hel | --he | -h)
    usage; exit 0 ;;

  -j | --parallelism | --parallelis | --paralleli | --parallel \
      | --paralle | --parall | --paral | --para | --par \
      | --pa)
    ac_prev=parallelism ;;
  -j=* | --parallelism=* | --parallelis=* | --paralleli=* | --parallel=* \
      | --paralle=* | --parall=* | --paral=* | --para=* | --par=* \
      | --pa=*)
    parallelism=$ac_optarg ;;
  -j*)
    parallelism=`echo $ac_option | sed 's/-j//'` ;;

  -prefix | --prefix | --prefi | --pref | --pre | --pr | --p)
    ac_prev=prefix ;;
  -prefix=* | --prefix=* | --prefi=* | --pref=* | --pre=* | --pr=* | --p=*)
    prefix=$ac_optarg ;;

  -components | --components | --component | --componen | \
      --compone | --compon | --compo | --comp | --com \
      | --co | --c)
    ac_prev=components ;;
  -components=* | --components=* | --component=* | --componen=* \
      | --compone=* | --compon=* | --compo=* | --comp=* | --com=* \
      | --co=* | --c=*)
    components=$ac_optarg ;;

  --host)
    ac_prev=host ;;
  --host=*)
    host=$ac_optarg ;;

  -*) { echo "$as_me: error: unrecognized option: $ac_option
Try \`$0 --help' for more information." >&2
   { (exit 1); exit 1; }; }
    ;;

  *=*)
    ac_envvar=`expr "x$ac_option" : 'x\([^=]*\)='`
    # Reject names that are not valid shell variable names.
    expr "x$ac_envvar" : ".*[^_$as_cr_alnum]" >/dev/null &&
      { echo "$as_me: error: invalid variable name: $ac_envvar" >&2
   { (exit 1); exit 1; }; }
    ac_optarg=`echo "$ac_optarg" | sed "s/'/'\\\\\\\\''/g"`
    eval "$ac_envvar='$ac_optarg'"
    export $ac_envvar ;;

  *)
    ;;
  esac
done

# We don't want no errors from here on.
set -e

if test -n "$ac_prev"; then
  ac_option=--`echo $ac_prev | sed 's/_/-/g'`
  { echo "$as_me: error: missing argument to $ac_option" >&2
   { (exit 1); exit 1; }; }
fi

# Be sure to have absolute paths.
for ac_var in prefix
do
  eval ac_val=$`echo $ac_var`
  case $ac_val in
    [\\/$]* | ?:[\\/]* | NONE | '' ) ;;
    *)  { echo "$as_me: error: expected an absolute directory name for --$ac_var: $ac_val" >&2
   { (exit 1); exit 1; }; };;
  esac
done

if [ "x${prefix}" != "x" ]; then
    export PREFIX="${prefix}"
else
    export PREFIX=${ac_default_prefix}
fi

# Figure out what components where requested to be built.
if test x"${components+set}" != xset; then
    components=all
else
    if test x"${components}" = x ||
	test x"${components}" = xyes;
	then
	echo --components needs at least one argument 1>&2
	exit 1
    fi
fi

if [ "x${parallelism}" != "x" ]; then
    PARALLELISM="-j${parallelism}"
else
    PARALLELISM=
fi

# embedded tabs in the sed below -- do not untabify
components=`echo "${components}" | sed -e 's/[ 	,][ 	,]*/,/g' -e 's/,$//'`

# Don't build in source directory, it will clobber things and cleanup is hard.
if [ -d ${BUILD_DIR}/.svn ]; then
	echo "Don't build in a source directory, please create an empty directory and build there."
	echo "Example :"
	echo "  mkdir build-mingw32ce"
	echo "  cd build-mingw32ce"
	echo "  sh ../build-mingw32ce.sh $@"
	exit 1
fi

build_binutils()
{
    echo ""
    echo "BUILDING BINUTILS --------------------------"
    echo ""
    echo ""
    mkdir -p binutils
    cd binutils
    ${BASE_DIRECTORY}/binutils/configure \
	--prefix=${PREFIX}      \
	--host=${HOST}          \
	--target=${TARGET}      \
	--disable-nls		\
	--disable-werror

    make ${PARALLELISM}
    make install

    cd ${BUILD_DIR}
}    

build_bootstrap_gcc()
{
    mkdir -p gcc-bootstrap
    cd gcc-bootstrap

    ${BASE_DIRECTORY}/${gcc_src}/configure	       \
	--with-gcc                     \
	--with-gnu-ld                  \
	--with-gnu-as                  \
	--target=${TARGET}             \
	--build=${BUILD}               \
	--host=${HOST}                 \
	--prefix=${PREFIX}             \
	--disable-threads              \
	--disable-nls                  \
	--enable-languages=c           \
	--disable-win32-registry       \
	--disable-multilib             \
	--disable-shared               \
	--disable-interwork            \
	--without-newlib               \
	--enable-checking	|| exit 1
    
    case ${gcc_src} in
        gcc)
            make ${PARALLELISM} all-gcc	|| exit 1
            make install-gcc	|| exit 1
        ;;
        gcc-*)   
            make ${PARALLELISM} all-gcc	|| exit 1
            make configure-target-libgcc || exit 1
            make install-gcc	|| exit 1
            cd ${TARGET}/libgcc	|| exit 1
            make ${PARALLELLISM} libgcc.a	|| exit 1
            /usr/bin/install -c -m 644 libgcc.a ${PREFIX}/lib/gcc/${TARGET}/4.4.0 || exit 1
        ;;
    esac
    
    cd ${BUILD_DIR}
}

build_w32api()
{
    mkdir -p w32api
    cd w32api

    ${BASE_DIRECTORY}/w32api/configure \
	--host=${TARGET}               \
	--prefix=${PREFIX}

    make ${PARALLELISM}
    make install

    #
    # Create a cegcc.h file with some sensible input
    # Code is copied from the scripts/make_release.sh script.
    # This will probably always have "old" numbers
    #
    # CEGCC_VERSION_MAJOR=`echo $VERSION | awk -F. '{print $1}'`
    # CEGCC_VERSION_MINOR=`echo $VERSION | awk -F. '{print $2}'`
    # CEGCC_VERSION_PATCHLEVEL=`echo $VERSION | awk -F. '{print $3}'`
    #
    # Version patchlevel 999 refers to SVN from now on :-)
    #
    CEGCC_VERSION_MAJOR=0
    CEGCC_VERSION_MINOR=55
    CEGCC_VERSION_PATCHLEVEL=999
    #
    INCFILE=${BASE_DIRECTORY}/w32api/include/cegcc.h.in
    DESTFILE=${PREFIX}/${TARGET}/include/cegcc.h
    #
    L1=`grep -s -n "Automatic changes below" ${INCFILE} | awk -F: '{print $1}'`
    L2=`grep -s -n "Automatic changes above" ${INCFILE} | awk -F: '{print $1}'`
    head -$L1 ${INCFILE} >${DESTFILE}
    echo "#define   __CEGCC_VERSION_MAJOR__ " $CEGCC_VERSION_MAJOR >> ${DESTFILE}
    echo "#define   __CEGCC_VERSION_MINOR__ " $CEGCC_VERSION_MINOR >> ${DESTFILE}
    echo "#define   __CEGCC_VERSION_PATCHLEVEL__ " $CEGCC_VERSION_PATCHLEVEL >> ${DESTFILE}
    echo "#define   __CEGCC_BUILD_DATE__" `date +%Y%m%d` >> ${DESTFILE}
    tail -n +$L2 ${INCFILE} >>${DESTFILE}

    cd ${BUILD_DIR}
}

build_mingw()
{
    mkdir -p mingw
    cd mingw
    ${BASE_DIRECTORY}/mingw/configure \
	--build=${BUILD}              \
	--host=${TARGET}              \
	--target=${TARGET}            \
	--prefix=${PREFIX}

    make ${PARALLELISM}
    make install

    cd ${BUILD_DIR}
}

build_gcc()
{
    mkdir -p gcc
    cd gcc

    case ${gcc_src} in
        gcc)
            ${BASE_DIRECTORY}/${gcc_src}/configure	\
            --with-gcc                     \
            --with-gnu-ld                  \
            --with-gnu-as                  \
            --build=${BUILD}               \
            --target=${TARGET}             \
            --host=${HOST}                 \
            --prefix=${PREFIX}             \
            --enable-threads=win32         \
            --disable-nls                  \
            --enable-languages=c,c++       \
            --disable-win32-registry       \
            --disable-multilib             \
            --disable-interwork            \
            --without-newlib               \
            --enable-checking              \
            --with-headers
        ;;
        gcc-*)   
           ${BASE_DIRECTORY}/${gcc_src}/configure	\
            --with-gcc                     \
            --with-gnu-ld                  \
            --with-gnu-as                  \
            --build=${BUILD}               \
            --target=${TARGET}             \
            --host=${HOST}                 \
            --prefix=${PREFIX}             \
            --enable-threads=win32         \
            --disable-nls                  \
            --enable-languages=c,c++       \
            --disable-win32-registry       \
            --disable-multilib             \
            --disable-interwork            \
            --without-newlib               \
            --enable-checking              \
            --with-headers			\
            --disable-__cxa_atexit
        ;;
    esac
    

# we build libstdc++ as dll, so we don't need this.    
#  --enable-fully-dynamic-string  \
#	--enable-sjlj-exceptions	\
#  --disable-clocale              \

    make ${PARALLELISM}
    make install

    #
    # Clean up one file
    #

    cd ${BUILD_DIR}
}

build_gdb()
{
    echo ""
    echo "BUILDING GDB --------------------------"
    echo ""
    echo ""

    mkdir -p gdb
    cd gdb

    ${BASE_DIRECTORY}/gdb/configure  \
	--with-gcc                     \
	--with-gnu-ld                  \
	--with-gnu-as                  \
	--target=${TARGET}             \
	--prefix=${PREFIX}             \
	--disable-nls                  \
	--disable-win32-registry       \
	--disable-multilib             \
	--disable-interwork            \
	--enable-checking

    export CFLAGS=${PREV_CFLAGS}

    make ${PARALLELISM}
    make install

    cd ${BUILD_DIR}
}

build_gdbserver()
{
    echo ""
    echo "BUILDING gdbserver --------------------------"
    echo ""
    echo ""

    mkdir -p gdbserver
    cd gdbserver

    ${BASE_DIRECTORY}/gdb/gdbserver/configure  \
	--target=${TARGET}             \
	--host=${TARGET}               \
	--prefix=${PREFIX}             \

    make ${PARALLELISM}
    make install

    cd ${BUILD_DIR}
}

build_docs()
{
    echo ""
    echo "INSTALLING documentation --------------------------"
    echo ""
    echo ""

    mkdir -p ${PREFIX}/share/docs
    mkdir -p ${PREFIX}/share/images

    cd ${BASE_DIRECTORY}/../docs
    tar cf - . | (cd ${PREFIX}/share/docs; tar xf -)
    cd ${BASE_DIRECTORY}/../website
    tar cf - images | (cd ${PREFIX}/share; tar xf -)

    cd ${BASE_DIRECTORY}/..
    cp NEWS README ${PREFIX}
    cp src/binutils/COPYING ${PREFIX}
    cp src/binutils/COPYING.LIB ${PREFIX}
    cp src/newlib/COPYING.NEWLIB ${PREFIX}

    cd ${BUILD_DIR}
}

build_profile()
{
    echo ""
    echo "BUILDING profiling libraries --------------------------"
    echo ""
    echo ""

    mkdir -p profile
    cd profile

    ${BASE_DIRECTORY}/profile/configure  \
	--build=${BUILD}              \
	--host=${TARGET}              \
	--target=${TARGET}            \
	--prefix=${PREFIX}            \
	|| exit

    make ${PARALLELISM}
    make install

    cd ${BUILD_DIR}
}

obuild_dlls()
{
    echo ""
    echo "BUILDING DLL libraries --------------------------"
    echo ""
    echo ""

    cd ${BUILD_DIR}

    mkdir -p dll
    cd dll

    cd ${BASE_DIRECTORY}
    ${BASE_DIRECTORY}/build-mingw32ce-dlls.sh

    cd ${BUILD_DIR}
}

build_dlls()
{
    echo ""
    echo "BUILDING DLL libraries --------------------------"
    echo ""
    echo ""

    cd ${BUILD_DIR}

    cd ${BASE_DIRECTORY}/mingwdll
    make
    make install

    cd ${BUILD_DIR}
}

build_all()
{
    for ((i=0;i<$COMPONENTS_NUM;i++)); do
	comp=${COMPONENTS[${i}]}
	build_$comp
    done
}

split_components=`echo "${components}" | sed -e 's/,/ /g'`

# check for valid options before trying to build them all.
eval "set -- $split_components"
while [ -n "$1" ]; do
    if [ "$1" != "all" ]; then
	found=false
	for ((i=0;i<$COMPONENTS_NUM;i++)); do
	    if [ "${COMPONENTS[${i}]}" = "$1" ]; then
		found=true
	    fi
	done
	if [ $found = false ] ; then
	    echo "Please enter a valid build option."
	    exit 1
	fi
    fi

    shift
done

export TARGET="arm-mingw32ce"
#export TARGET="arm-wince-mingw32ce"
export BUILD=`sh ${BASE_DIRECTORY}/${gcc_src}/config.guess`
if [ "x${host}" != "x" ]; then
    export HOST="${host}"
else
    export HOST=${BUILD}
fi

export PATH=${PREFIX}/bin:${PATH}

echo "Building mingw32ce:"
echo "source: ${BASE_DIRECTORY}"
echo "building in: ${BUILD_DIR}"
echo "prefix: ${PREFIX}"
echo "components: ${components}"

mkdir -p ${BUILD_DIR}
mkdir -p ${PREFIX}

# Now actually build them.
eval "set -- $split_components"
while [ -n "$1" ]; do
    build_${1}
    shift
done

echo ""
echo "DONE --------------------------"
echo ""
echo ""
