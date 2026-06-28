#!/bin/sh
set -eu

hash_scene() {
    scene=$1
    ./build/deadgl hash "$scene" | awk '{print $1}'
}

check_dgb_parity() {
    name=$1
    scene=$2
    mkdir -p build/dgb_parity
    dgb="build/dgb_parity/$name.dgb"
    unpacked="build/dgb_parity/$name.unpack.dgl"
    ./build/deadgl pack "$scene" -o "$dgb"
    ./build/deadgl disasm "$dgb" > "build/dgb_parity/$name.disasm"
    ./build/deadgl unpack "$dgb" -o "$unpacked"
    a=$(hash_scene "$scene")
    b=$(hash_scene "$unpacked")
    if [ "$a" != "$b" ]; then
        echo "dgb parity hash moved: $name $a $b" >&2
        exit 1
    fi
    echo "DGB PARITY $name $a"
}

if [ ! -x ./build/deadgl ]; then
    echo "missing build/deadgl; run make first" >&2
    exit 1
fi

check_dgb_parity near_clip examples/near_clip.dgl
check_dgb_parity command_machine examples/command_machine.dgl

echo "DGB proof parity locked."
