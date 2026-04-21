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