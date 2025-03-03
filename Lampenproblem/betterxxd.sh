#!/bin/bash

# Überprüfen, ob zwei Dateien als Argumente übergeben wurden
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <inputfile.tar> <outputfile.h>"
    exit 1
fi

# Datei in einer Variablen speichern
inp=$1
oup=$2

xxd -i $inp $oup || (echo 'the "xxd" command is not installed lmao'; exit 2)

# Die Ersetzungen vornehmen und die Datei direkt aktualisieren
# Alle Vorkommen von ", " und ",\n  " werden durch "," ersetzt:
temp_file=$(mktemp) # Temporäre Datei erstellen
tr -d '\n' < "$oup" > "$temp_file"
sed -i -e 's/, /,/g' "$temp_file"
sed -i -e 's/, /,/g' "$temp_file"
#sed -i    's/,0x//g' "$temp_file"
mv "$temp_file" "$oup"
