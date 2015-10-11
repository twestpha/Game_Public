#!/bin/bash

NMAP_CHECK=$(which nmap)
if [ -z "$NMAP_CHECK" ]
then
    echo -e "\033[1;31m[Error]\033[0m Please install nmap for the build count check."
    exit 125
fi

CURL_CHECK=$(which curl)
if [ -z "$CURL_CHECK" ]
then
    echo -e "\033[1;31m[Error]\033[0m Please install curl for the build count check."
    exit 125
fi

# Check if you are on the local network
# If you are, then set the host to yakkio's local
# ip, otherwise use the domain name.
YAKKIO_LOCAL="192.168.1.53"
YAKKIO_GLOBAL="yakkio.com"

PORT_CHECK=$(nmap 192.168.1.53 -p 8080 | grep "open")
if [ -z "$PORT_CHECK" ]
then
    HOST="$YAKKIO_GLOBAL"
else
    HOST="$YAKKIO_LOCAL"
fi

FULL_HOSTNAME="http://$HOST:8080"

# printf "Sending POST request to $FULL_HOSTNAME\n"
NUM=$(curl -X POST -s http://$HOST:8080)
printf "Build Number: \033[94m%s\033[39m\n" "$NUM"
