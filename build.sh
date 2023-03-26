#! /bin/bash

set -euo pipefail

WHEREAMI=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)

compile() {
    cd $WHEREAMI

    generate_qrc
    qmake

    # extract the include flags made by qmake into .clangd
    INCFLAGS=$(grep 'INCPATH *=' Makefile.Debug | cut -f 2- -d '-' | tr ' ' ',')
    echo -e "CompileFlags:\n\tAdd: [${INCFLAGS},-std=c++17]" > .clangd

    make debug

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

    for icon in assets/img/icons/*.png; do
        echo "<file>$icon</file>" >> .images.qrc
    done

    for image in assets/img/style/*.png; do
        echo "<file>$image</file>" >> .images.qrc
    done

    for image in assets/img/map/*.png; do
        echo "<file>$image</file>" >> .images.qrc
    done

    for wav in assets/wav/*.wav; do
        echo "<file>$wav</file>" >> .images.qrc
    done

    echo "</qresource></RCC>" >> .images.qrc

    if [[ $(sha256sum .images.qrc | cut -f 1 -d " ") != $(sha256sum images.qrc | cut -f 1 -d " ") ]]; then
        cp .images.qrc images.qrc
    fi

    rm .images.qrc
}

compile
