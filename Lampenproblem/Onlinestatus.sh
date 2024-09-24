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

# Ermittele die lokale IP-Adresse des Systems (nur für eth0 oder wlan0, je nach Interface)
LOCAL_IP=$(ip -4 addr show scope global | grep inet | awk '{print $2}' | cut -d/ -f1 | head -n 1)

# Wenn keine IP-Adresse gefunden wird, auf localhost zurückgreifen
if [ -z "$LOCAL_IP" ]; then
  LOCAL_IP="localhost"
fi

# Zeige, wie der Server von einem anderen Gerät aus aufgerufen werden kann
echo "Die Datei kann von einem anderen Gerät über folgende URL aufgerufen werden:"
echo "http://$LOCAL_IP:$PORT"
echo "Bzw über folgenden Kommando:"
echo "links2 http://$LOCAL_IP:$PORT"

while true; do 
  echo -e "HTTP/1.1 200 OK\n\n $(cat $file)" | nc -l -p $PORT -q 1
done
