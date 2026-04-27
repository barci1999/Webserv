#!/bin/bash

URL=${1:-http://localhost:8080/}
REQUESTS=${2:-1000}
CONCURRENCY=${3:-20}

TMP=$(mktemp)

echo "Testing $URL"
echo "Requests: $REQUESTS | Concurrency: $CONCURRENCY"
echo "-----------------------------"

seq $REQUESTS | xargs -n1 -P$CONCURRENCY -I{} \
curl -o /dev/null -s -w "%{http_code}\n" "$URL" >> "$TMP"

TOTAL=$(wc -l < "$TMP")
OK=$(grep "^200$" "$TMP" | wc -l)
FAIL=$((TOTAL - OK))

AVAIL=$(echo "$OK * 100 / $TOTAL" | bc -l)

echo ""
echo "-------- RESULTS --------"
echo "Total: $TOTAL"
echo "OK: $OK"
echo "Failed: $FAIL"
echo "Availability: $AVAIL %"

rm "$TMP"