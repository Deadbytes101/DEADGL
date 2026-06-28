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

need_hash() {
    name=$1
    actual=$2
    expected=$3
    if [ "$actual" != "$expected" ]; then
        echo "golden hash moved: $name expected $expected got $actual" >&2
        exit 1
    fi
    echo "GOLDEN $name $actual"
}

scene_hash() {
    scene=$1
    "$DEADGL" hash "$scene" | awk '{print $1}'
}

demo_hash() {
    name=$1
    "$DEADGL" demo "$name" -o "build/golden_$name.ppm" | awk '/^hash / {print $2}'
}

need_hash cube "$(demo_hash cube)" d710810073eeec70
need_hash command_machine "$(scene_hash examples/command_machine.dgl)" ddcf9658d3e00f08
need_hash near_clip "$(scene_hash examples/near_clip.dgl)" c84cf0589c219386

echo "GOLDEN proof hashes locked."
