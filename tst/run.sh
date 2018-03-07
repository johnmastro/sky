#!/usr/bin/env bash

shopt -s nullglob

cd $(dirname ${BASH_SOURCE[0]})

for file in data/*.lisp; do
    out="${file}.out"
    ../src/sky "$file" > "$out"
    if [[ ! -e "$out" ]]; then
        echo "Expected file not created: $out"
        exit 1
    elif ! diff -q "$file" "$out" >/dev/null; then
        echo "Files differ: $file $out"
        exit 1
    fi
done
