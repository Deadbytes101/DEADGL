#!/bin/sh
set -eu

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
    ./build/deadgl hash "$scene" | awk '{print $1}'
}

demo_hash() {
    name=$1
    ./build/deadgl demo "$name" -o "build/golden_$name.ppm" | awk '/^hash / {print $2}'
}

if [ ! -f ./build/deadgl ]; then
    echo "missing build/deadgl; run make first" >&2
    exit 1
fi

need_hash cube "$(demo_hash cube)" 95689452fd3b5017
need_hash command_machine "$(scene_hash examples/command_machine.dgl)" 7dd8eb60b1510af7
need_hash near_clip "$(scene_hash examples/near_clip.dgl)" 33dd8e01461d1f5c

echo "GOLDEN proof hashes locked."
