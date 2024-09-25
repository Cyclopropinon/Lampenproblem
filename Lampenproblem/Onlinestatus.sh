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

# Wenn keine IPv4-Adresse gefunden wird, auf IPv6 zurückgreifen
if [ -z "$LOCAL_IP" ]; then
  LOCAL_IP=$(ip -6 addr show scope global | grep inet6 | awk '{print $2}' | cut -d/ -f1 | head -n 1)
fi

# Wenn keine IP-Adresse gefunden wird, auf localhost zurückgreifen
if [ -z "$LOCAL_IP" ]; then
  LOCAL_IP="localhost"
fi

# Ermittele die öffentliche IP-Adresse
PUBLIC_IP=$(curl -s https://ifconfig.me/ip)

# Funktion zum Prüfen, ob eine IP IPv6 ist und in eckige Klammern setzen
function format_ip() {
  if [[ $1 == *:* ]]; then
    echo "[$1]"
  else
    echo "$1"
  fi
}

# Formatiere die IP-Adressen
LOCAL_IP=$(format_ip $LOCAL_IP)
PUBLIC_IP=$(format_ip $PUBLIC_IP)

echo ""

# Zeige, wie der Server von einem anderen Gerät aus aufgerufen werden kann
echo "Die Datei kann von einem anderen Gerät im lokalen Netzwerk über folgende URL aufgerufen werden:"
echo "http://$LOCAL_IP:$PORT"
echo "Bzw über folgenden Kommando:"
echo "links2 http://$LOCAL_IP:$PORT"
echo ""
echo "Die Datei kann von außerhalb (Internet) über folgende öffentliche IP aufgerufen werden:"
echo "http://$PUBLIC_IP:$PORT"
echo "Bzw über folgenden Kommando:"
echo "links2 http://$PUBLIC_IP:$PORT"
echo "Achtung: Die Ports könnten vom Router blockiert sein!"
echo ""

while true; do 
  echo -e "HTTP/1.1 200 OK\n\n $(cat $file)" | nc -l -p $PORT -q 1
done
