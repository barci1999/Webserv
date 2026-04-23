#!/usr/bin/env python3
import time
import sys

body = sys.stdin.read()

print("Content-Type: text/plain")
print()

print("Empiezo...")
time.sleep(5)

print("Termino")
print(body)

print("Content-Type: text/html")
print()

print("<html><body>")
print("<h1>🦜 Parrot CGI</h1>")

print("<h2>Method:</h2>")
print("<p>%s</p>" % os.environ.get("REQUEST_METHOD", ""))

print("<h2>Query:</h2>")
print("<p>%s</p>" % os.environ.get("QUERY_STRING", ""))

print("<h2>Body:</h2>")
body = sys.stdin.read()
print("<pre>%s</pre>" % body)

print("</body></html>")