#!/usr/bin/env python3

print("Content-Type: text/html")
print()
import time
time.sleep(2)

print("""
<html>
<body style="text-align:center; font-family:Arial;">

<h1>          Como se llama el creador</h1>

<div style="display:flex; justify-content:center; gap:40px;">

  <!-- OPCIÓN 1 -->
  <a href="/cgi-bin/on_2T.py" style="text-decoration:none; color:black;">
    <div>
      <img src="https://media3.giphy.com/media/v1.Y2lkPTc5MGI3NjExcDA1M285NnZpa2oxZG5la3N0eGRtM2UwZXpmOWo4bWtzaG53YWRwMyZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/aKI9wHGUpIvRtIglEO/giphy.gif" width="250">
      <h3>Eiichiro Oda </h3>
    </div>
  </a>

  <!-- OPCIÓN 2 -->
  <a href="/cgi-bin/on_2F.py" style="text-decoration:none; color:black;">
    <div>
      <img src="https://media3.giphy.com/media/v1.Y2lkPTc5MGI3NjExcDA1M285NnZpa2oxZG5la3N0eGRtM2UwZXpmOWo4bWtzaG53YWRwMyZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/aKI9wHGUpIvRtIglEO/giphy.gif" width="250">
      <h3>Akira Toriyama </h3>
    </div>
  </a>

</div>

</body>
</html>
""")