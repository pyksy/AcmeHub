#!/bin/bash

URL="http://localhost:4080/process_report"
SERVERS="24"
STARTDATE="2025-01-01"
DAYS="30"
DURATION="600"
AVGVARIANCE="10"
OUTLIERSVARIANCE="600"
OUTLIERSRATIO="64"

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
        DURATION="600"
        [ $((RANDOM % OUTLIERSRATIO)) -eq 0 ] && DURATION="$((RANDOM % OUTLIERSVARIANCE + DURATION))" || DURATION="$((RANDOM % AVGVARIANCE + DURATION))"
        STARTSECONDS="${RANDOM}"
        STARTTIME="$(date -u -d "${STARTDATE} + ${DAY} days ${STARTSECONDS} seconds" +"%Y-%m-%dT%H:%M:%SZ")"
        ENDTIME="$(date -u -d "${STARTDATE} + ${DAY} days $((STARTSECONDS+DURATION)) seconds" +"%Y-%m-%dT%H:%M:%SZ")"
        POSTDATA="{\"server_name\":\"${SERVERNAME}\",\"start_time\":\"${STARTTIME}\",\"end_time\":\"${ENDTIME}\"}"

        curl -s -o /dev/null -d "${POSTDATA}" "${URL}" && echo -n "." || echo -n '!'
    done
done
echo
