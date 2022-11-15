#! /bin/bash

set -euo pipefail

WHEREAMI=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)

compile() {
    cd $WHEREAMI

    generate_qrc
    qmake

    # extract the include flags made by qmake into .clangd
    INCFLAGS=$(grep 'INCPATH *=' Makefile | cut -f 2- -d '-' | tr ' ' ',')
    echo -e "CompileFlags:\n\tAdd: [${INCFLAGS}]" > .clangd

    make
}

test() {
    compile

    cd $WHEREAMI/test
    qmake
    make

    ./test
}

generate_qrc() {
    cd $WHEREAMI

    echo "<!DOCTYPE RCC><RCC version=\"1.0\"><qresource>" > images.qrc
    for item in assets/img/items/*.png; do
        echo "<file>$item</file>" >> images.qrc
    done

    for frame in assets/img/lk/*.png; do
        echo "<file>$frame</file>" >> images.qrc
    done

    for frame in assets/img/icons/*.png; do
        echo "<file>$frame</file>" >> images.qrc
    done

    echo "</qresource></RCC>" >> images.qrc
}

if [[ $# -eq 0 || $1 == "compile" ]]; then
    compile
elif [[ $1 == "test" ]]; then
    test
else
    echo "please specify either \"compile\" or \"test\""
    exit 1
fi
