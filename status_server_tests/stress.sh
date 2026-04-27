#!/bin/bash

URL=${1:-http://localhost:8080/}
REQUESTS=${2:-100}
CONCURRENCY=${3:-10}

TMP_FILE=$(mktemp)

echo "Testing $URL"
echo "Requests: $REQUESTS | Concurrency: $CONCURRENCY"
echo "-----------------------------"

START=$(date +%s.%N)

seq $REQUESTS | xargs -n1 -P$CONCURRENCY -I{} \
  curl -o /dev/null -s -w "%{http_code} %{time_total}\n" "$URL" >> "$TMP_FILE"

END=$(date +%s.%N)

DURATION=$(echo "$END - $START" | bc)

TOTAL=$(wc -l < "$TMP_FILE")
SUCCESS=$(grep "^200" "$TMP_FILE" | wc -l)
FAILED=$(grep -v "^200" "$TMP_FILE" | wc -l)

REQS_PER_SEC=$(echo "$TOTAL / $DURATION" | bc -l)

# latencias
AVG=$(awk '{sum+=$2} END {print sum/NR}' "$TMP_FILE")

# porcentajes de lentitud
SLOW_100MS=$(awk '$2 > 0.1 {count++} END {print count+0}' "$TMP_FILE")
SLOW_500MS=$(awk '$2 > 0.5 {count++} END {print count+0}' "$TMP_FILE")
SLOW_1S=$(awk '$2 > 1 {count++} END {print count+0}' "$TMP_FILE")

PCT_100MS=$(echo "$SLOW_100MS * 100 / $TOTAL" | bc -l)
PCT_500MS=$(echo "$SLOW_500MS * 100 / $TOTAL" | bc -l)
PCT_1S=$(echo "$SLOW_1S * 100 / $TOTAL" | bc -l)

echo ""
echo "-------- RESULTS --------"
echo "Total time: $DURATION sec"
echo "Requests/sec: $REQS_PER_SEC"
echo "Success: $SUCCESS"
echo "Failed: $FAILED"
echo "Avg latency: $AVG sec"
echo ""
echo "-------- SLOW REQUESTS --------"
echo ">100ms : $SLOW_100MS ($PCT_100MS%)"
echo ">500ms : $SLOW_500MS ($PCT_500MS%)"
echo ">1s    : $SLOW_1S ($PCT_1S%)"

rm "$TMP_FILE"