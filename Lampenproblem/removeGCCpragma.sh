#!/bin/bash

# Name der Datei, die bearbeitet werden soll
input_file=$1
output_file="gefilterte_datei.txt"

# Benutzt grep, um Zeilen, die diese Muster enthalten, zu ignorieren
grep -v -e '#pragma GCC diagnostic push' \
        -e '#pragma GCC diagnostic ignored "-Wformat"' \
        -e '#pragma GCC diagnostic pop' \
        "$input_file" > "$output_file"

# Optional: Die Originaldatei durch die gefilterte Datei ersetzen
mv "$output_file" "$input_file"
