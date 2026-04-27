#!/bin/bash

HOST=${1:-localhost}
PORT=${2:-8080}
ITERATIONS=${3:-100}
MAX_CONCURRENCY=${4:-20}

methods=(
"GET" "POST" "DELETE" "PUT"
"HEAD" "OPTIONS"
"PATCH"
"TRACE"
"CONNECT"
"HACK" "INVALID" ""
"GE T" "PO ST"
"get" "post"
)
paths=(
"/"
"/index.html"
"/doesnotexist"
"/test"
"/random$RANDOM"

# traversal
"/../../etc/passwd"
"/../../../"
"/..%2f..%2fetc/passwd"

# raros
"/./././"
"//"
"////"
"/%00"
"/%ff"

# largos
"/$(head -c 100 </dev/urandom | base64 | tr -d /=+ | head -c 50)"

# query strings
"/?a=1"
"/?a=$(head -c 20 </dev/urandom | base64)"
"/test?param=value"

# encoding
"/%2e%2e/"
"/%2F%2F"

# edge
""
"/ "
"/\t"
)
headers=(
"Host: localhost"
"Host: bad"
"Host:"

# duplicados
"Host: localhost\r\nHost: evil"

# largos
"X-Fuzz: $(head -c 50 </dev/urandom | base64)"

# inválidos
"Invalid-Header"
"X:"

# raros
"Content-Length: -1"
"Content-Length: 999999999"
"Content-Length: abc"

# encoding raro
"X-Test: %00%01%02"

# múltiples headers
"User-Agent: fuzz\r\nX-Test: 123"

# vacío
""
)
bodies=(
""
"abc"
"12345"
"$(head -c 20 </dev/urandom | base64)"
)

COUNT=0
TMP=$(mktemp)

echo "Starting fuzz test on $HOST:$PORT"
echo "Iterations: $ITERATIONS | Concurrency: $MAX_CONCURRENCY"
echo "-----------------------------"

for i in $(seq 1 $ITERATIONS); do

  METHOD=${methods[$RANDOM % ${#methods[@]}]}
  PTH=${paths[$RANDOM % ${#paths[@]}]}
  HEADER=${headers[$RANDOM % ${#headers[@]}]}
  BODY=${bodies[$RANDOM % ${#bodies[@]}]}

REQUEST="${METHOD} ${PTH} HTTP/1.1\r\nHost: localhost\r\n${HEADER}\r\nContent-Length: ${#BODY}\r\nConnection: close\r\n\r\n${BODY}"
  {

    # ---- SEND REQUEST ----
    RESPONSE=$(echo -e "$REQUEST" | netcat -w 1 $HOST $PORT 2>&1)
    NC_EXIT=$?

    STATUS_LINE=$(echo "$RESPONSE" | head -n 1)
    CODE=$(echo "$STATUS_LINE" | cut -d' ' -f2)

    # ---- CLASSIFICATION ----
    TYPE="OK"

    if [ $NC_EXIT -ne 0 ]; then
      TYPE="CRASH_OR_CONN_FAIL"
      CODE="000"
    elif [ -z "$RESPONSE" ]; then
      TYPE="TIMEOUT"
      CODE="000"
    elif ! echo "$STATUS_LINE" | grep -q "HTTP/"; then
      TYPE="PARSE_ERROR"
      CODE="000"
    fi

    # ---- SAVE STATS ----
    echo "$CODE" >> "$TMP"

    # ---- PRINT ----
    echo "[ $METHOD $PTH ] -> $CODE ($TYPE)"

    # ---- LOG ONLY PROBLEMS ----
    if [ "$CODE" = "000" ]; then
      {
        echo "-----------------------------"
        echo "[TYPE] $TYPE"
        echo "[REQUEST]"
        echo -e "$REQUEST"
        echo "[RESPONSE]"
        echo "$RESPONSE"
        echo "-----------------------------"
      } >> error.txt
    fi

  } &

  COUNT=$((COUNT+1))

  if [ $COUNT -ge $MAX_CONCURRENCY ]; then
    wait
    COUNT=0
  fi

done

wait

echo "-----------------------------"
echo "RESULTS SUMMARY"

TOTAL=$(wc -l < "$TMP")

for c in 200 201 204 301 302 400 403 404 405 408 411 413 414 500 501 000; do
  COUNT_CODE=$(grep "^$c$" "$TMP" | wc -l)
  PCT=$(echo "$COUNT_CODE * 100 / $TOTAL" | bc -l 2>/dev/null)
  echo "$c : $COUNT_CODE ($PCT%)"
done

AVAIL=$(grep -v "^000$" "$TMP" | wc -l)
AVAIL_PCT=$(echo "$AVAIL * 100 / $TOTAL" | bc -l 2>/dev/null)

echo "-----------------------------"
echo "Availability (non-failure responses): $AVAIL_PCT %"

rm "$TMP"

echo "Fuzzing finished."