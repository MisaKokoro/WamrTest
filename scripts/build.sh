#!/bin/bash

set -e

CC=${CC:-cc}
echo $CC
# assume we are in a subdirectory of the project to build
HERE=$(dirname $0)/..
echo $HERE
cd $HERE
ROOT=$(pwd)
echo $ROOT
NAME=$(basename $ROOT)
echo $NAME

mkdir -p build
mkdir -p generated

cd build

if [[ "$FLATCC_PORTABLE" = "yes" ]]; then
    CFLAGS="$CFLAGS -DFLATCC_PORTABLE"
fi

CFLAGS="$CFLAGS -I ${ROOT}/include -I ${ROOT}/generated -I ${ROOT}/wamr/**"
CFLAGS_DEBUG=${CFLAGS_DEBUG:--g}
CFLAGS_RELEASE=${CFLAGS_RELEASE:--O2 -DNDEBUG}

${ROOT}/bin/flatcc -a -o ${ROOT}/generated ${ROOT}/src/*.fbs

cd ${ROOT}/build
cmake ..
make -j

# echo "building '$NAME' for debug"
# $CC $CFLAGS $CFLAGS_DEBUG ${ROOT}/src/*.c ${ROOT}/lib/libflatccrt_d.a -o ${NAME}_d

# echo "building '$NAME' for release"
# $CC $CFLAGS $CFLAGS_RELEASE ${ROOT}/src/*.c ${ROOT}/lib/libflatccrt.a -o ${NAME}
