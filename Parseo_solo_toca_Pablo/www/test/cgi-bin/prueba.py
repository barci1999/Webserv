#!/usr/bin/env python3

print("Content-Type: text/html")
print()
import time
time.sleep(2)

print("""
<html>
<body style="text-align:center;">

<h1>Elige tu destino 😎</h1>

<form action="/cgi-bin/test.py" method="get">
    <button type="submit">🔥 Opción 1</button>
</form>

<br>

<form action="/cgi-bin/slow.py" method="get">
    <button type="submit">💧 Opción 2</button>
</form>

</body>
</html>
""")