#! /bin/bash

set -euo pipefail

WHEREAMI=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)

compile() {
    cd $WHEREAMI

    generate_qrc
    qmake

    # extract the include flags made by qmake into .clangd
    INCFLAGS=$(grep 'INCPATH *=' Makefile | cut -f 2- -d '-' | tr ' ' ',')
    echo -e "CompileFlags:\n\tAdd: [${INCFLAGS},-std=c++17]" > .clangd

    make

    cd $WHEREAMI/server
    ./build.sh
}

generate_qrc() {
    cd $WHEREAMI

    for item in assets/img/items/*.png; do
        convert $item -fill "#000000" -colorize 100 assets/img/items/sil/$(basename $item)
    done

    echo "<!DOCTYPE RCC><RCC version=\"1.0\"><qresource>" > .images.qrc
    for item in assets/img/items/*.png; do
        echo "<file>$item</file>" >> .images.qrc
    done

    for item in assets/img/items/sil/*.png; do
        echo "<file>$item</file>" >> .images.qrc
    done

    for frame in assets/img/lk/*.png; do
        echo "<file>$frame</file>" >> .images.qrc
    done

    for frame in assets/img/icons/*.png; do
        echo "<file>$frame</file>" >> .images.qrc
    done

    echo "</qresource></RCC>" >> .images.qrc

    # I use QT Designer.
    # QT Designer has this UNSUPPRESSABLE feature where it whines at you if the
    # resource file it's using changes externally.
    # This causes QT Designer to complain with a stop-the-world, focus-grabbing dialog
    # every.
    # fucking.
    # time.
    # you compile this shit.
    # It makes me desire the sweet icy release of death.
    # This is why the images.qrc is written to a temporary file,
    # and then checked to see if it's the same as any existing images.qrc
    # before actually changing it!!!!
    #
    if [[ $(sha256sum .images.qrc | cut -f 1 -d " ") != $(sha256sum images.qrc | cut -f 1 -d " ") ]]; then
        cp .images.qrc images.qrc
    fi

    rm .images.qrc
}

compile
