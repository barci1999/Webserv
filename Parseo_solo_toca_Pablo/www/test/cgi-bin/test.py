#!/usr/bin/env python3

import time
import sys

body = sys.stdin.read()

# Primera cabecera: Status
print("Status: 200 OK")
print("Content-Type: text/plain")
print()
time.sleep(5)
print("CGI funcionando!")
print("Body recibido:")
print(body)
print("hola me llamo ramon y soy super listo")
