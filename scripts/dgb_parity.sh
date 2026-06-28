#!/bin/sh
set -eu

DEADGL=${DEADGL:-}
if [ -z "$DEADGL" ]; then
    if [ -f ./build/deadgl ]; then
        DEADGL=./build/deadgl
    elif [ -f ./build/deadgl.exe ]; then
        DEADGL=./build/deadgl.exe
    else
        echo "missing build/deadgl; run make first" >&2
        exit 1
    fi
fi

hash_scene() {
    scene=$1
    "$DEADGL" hash "$scene" | awk '{print $1}'
}

check_dgb_parity() {
    name=$1
    scene=$2
    mkdir -p build/dgb_parity
    dgb="build/dgb_parity/$name.dgb"
    unpacked="build/dgb_parity/$name.unpack.dgl"
    "$DEADGL" pack "$scene" -o "$dgb"
    "$DEADGL" disasm "$dgb" > "build/dgb_parity/$name.disasm"
    "$DEADGL" unpack "$dgb" -o "$unpacked"
    a=$(hash_scene "$scene")
    b=$(hash_scene "$unpacked")
    if [ "$a" != "$b" ]; then
        echo "dgb parity hash moved: $name $a $b" >&2
        exit 1
    fi
    echo "DGB PARITY $name $a"
}

check_dgb_parity near_clip examples/near_clip.dgl
check_dgb_parity command_machine examples/command_machine.dgl

echo "DGB proof parity locked."
