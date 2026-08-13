#!/bin/bash
seg=0
# Check number of arguments
if [[ $# -eq 1 ]]; then
    start=$1
    end=$1
elif [[ $# -eq 2 ]]; then
    start=$1
    end=$2
elif [[ $# -eq 3 ]]; then
    start=$1
    end=$2
    seg=$3
else
    echo "Usage: $0 RUN"
    echo "   or: $0 START_RUN END_RUN"
    exit 1
fi

# Loop over runs
for (( run=start; run<=end; run++ )); do
    echo "Processing run $run"
    analyzer "replay.C(${run},${seg})"
done
