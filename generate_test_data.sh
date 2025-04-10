#!/bin/bash

URL="http://localhost:4080/process_report"
SERVERS="8"
STARTDATE="2025-01-01"
DAYS="30"
MAXSECONDS="600"

# server name array
declare -a SERVERNAMES
for ((i=0; i<${SERVERS}; i++))
do
    SERVERNAMES+=("t-${RANDOM}")
done
echo Servers: ${SERVERNAMES[*]}

# data insertion loops
echo -n "Generating data ('.'=success, '!'=failure): "
for ((DAY=0; DAY<${DAYS}; DAY++))
do
    for SERVERNAME in "${SERVERNAMES[@]}"
    do
        DURATION="$((RANDOM % MAXSECONDS))"
        STARTSECONDS="${RANDOM}"
        STARTTIME="$(date -u -d "${STARTDATE} + ${DAY} days ${STARTSECONDS} seconds" +"%Y-%m-%dT%H:%M:%SZ")"
        ENDTIME="$(date -u -d "${STARTDATE} + ${DAY} days $((STARTSECONDS+DURATION)) seconds" +"%Y-%m-%dT%H:%M:%SZ")"
        POSTDATA="{\"server_name\":\"${SERVERNAME}\",\"start_time\":\"${STARTTIME}\",\"end_time\":\"${ENDTIME}\"}"

        curl -s -d "${POSTDATA}" "${URL}" && echo -n "." || echo -n '!'
    done
done
echo
