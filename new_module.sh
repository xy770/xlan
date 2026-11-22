#!/usr/bin/env bash
set -euo pipefail

if [ $# -lt 1 ]; then
    echo "usage: $0 <module_name>"
    exit 1
fi

NAME="$1"
DATE="$(date +%F)"
AUTHOR="xy770"

HEADER="include/${NAME}.h"
SOURCE="src/${NAME}.c"

if [ -e "$HEADER" ] || [ -e "$SOURCE" ]; then
    echo "already exists: $HEADER or $SOURCE"
    exit 1
fi

GUARD="${NAME^^}_H"
GUARD="${GUARD//-/_}"

cat > "$HEADER" <<EOF
/*
 * ${NAME}.h
 * author: ${AUTHOR}
 * date:   ${DATE}
 */

#ifndef ${GUARD}
#define ${GUARD}

#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif

#endif
EOF

cat > "$SOURCE" <<EOF
/*
 * ${NAME}.c
 * author: ${AUTHOR}
 * date:   ${DATE}
 */

#include "${NAME}.h"
EOF

echo "created $HEADER"
echo "created $SOURCE"
