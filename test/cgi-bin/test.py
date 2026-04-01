#!/usr/bin/env python3
import sys

body = sys.stdin.read()

print("Content-Type: text/plain")
print("Status: 200 OK")
print("")
print("CGI funcionando!")
print("Body recibido:")
print(body)