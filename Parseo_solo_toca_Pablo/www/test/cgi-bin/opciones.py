#!/usr/bin/env python3

print("Content-Type: text/html")
print()
import time
time.sleep(2)

print("""
<html>
<body style="text-align:center;">

<h1>         🚪 Elige una puerta</h1>

<div style="display:flex; justify-content:center; gap:20px;">

<h2>  <!-- PUERTA 1 --></h2>
<a href="/cgi-bin/test.py">
  <img src="https://media3.giphy.com/media/v1.Y2lkPTc5MGI3NjExcDA1M285NnZpa2oxZG5la3N0eGRtM2UwZXpmOWo4bWtzaG53YWRwMyZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/aKI9wHGUpIvRtIglEO/giphy.gif" width="250">
</a>

  <!-- PUERTA 2 -->
<a href="/cgi-bin/slow.py">
  <img src="https://media3.giphy.com/media/v1.Y2lkPTc5MGI3NjExcDA1M285NnZpa2oxZG5la3N0eGRtM2UwZXpmOWo4bWtzaG53YWRwMyZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/aKI9wHGUpIvRtIglEO/giphy.gif" width="250">
</a>

</div>

</body>
</html>
""")
