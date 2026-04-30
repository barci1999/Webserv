#!/usr/bin/env python3
import time
import sys

body = sys.stdin.read()

print("Status: 200 OK")
print("Content-Type: text/html")
print()
time.sleep(2)

print("<h2>⌛ Procesando muy muy fuerte...</h2>")
time.sleep(2)

print("<h2>💡 Ahora sí. Ya lo tengo!</h2>")
print()
time.sleep(1)

print("<h1>Como fan de Pokemon, te lo dedico Pablo<h1>")
print("<img src='https://media3.giphy.com/media/v1.Y2lkPTc5MGI3NjExeG1zbTdhdnEzbG44aDZhNzQ3cjhsczliOWxzNmhia2E3eTNpdXE2OCZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/frPnnZv9NJYfeldxr2/giphy.gif' width='730'>")
