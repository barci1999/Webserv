#!/usr/bin/env python3
import sys

body = sys.stdin.read()

# Primera cabecera: Status
print("Status: 200 OK")                  
print("Content-Type: text/plain")        
print()                                  

print("CGI funcionando!")
print("Body recibido:")
print(body)