#!/bin/bash

# Überprüfen, ob der Dateiname als Argument übergeben wurde
if [ -z "$1" ]; then
  echo "Bitte geben Sie den Dateinamen als Argument an."
  exit 1
fi

# Speichere den Dateinamen
file=$1

# Überprüfen, ob die Datei existiert
if [ ! -f "$file" ]; then
  echo "Die Datei '$file' existiert nicht."
  exit 1
fi

# Port
PORT=1729

# Lausche auf Port 1729 und antworte mit dem Inhalt der Textdatei
while true; do
  echo -e "Listening on port $PORT. Press [CTRL+C] to stop."
  # Warte auf eine Verbindung und antworte mit dem Inhalt der Datei
  cat << EOF | nc -l -p $PORT -q 1
HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: $(wc -c < "$file")

$(cat "$file")
EOF
done
