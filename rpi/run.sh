#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

stdbuf -oL -eL $DIR/badgy | \
while read i
do
    echo "${i}" >> ${DIR}/badge.log
    echo "${i}" | (espeak -v en+f3 -s 130 --stdout && sleep 1) | ${DIR}/pifm/pifm  - 103.3 1>/dev/null
done
