#!/usr/bin/env python3

print("Content-Type: text/html")
print()
import time
time.sleep(2)

print("""
<html>
<body style="text-align:center;">

<h1>           Elige un anime</h1>

<div style="display:flex; justify-content:center; gap:20px;">

  <!-- ONE PIECE -->
<a href="/cgi-bin/on_1.py">
  <img src="https://media4.giphy.com/media/v1.Y2lkPTc5MGI3NjExMHlteWV5aWtocHR2OGttc2F4NXlzaXkxbHdjYWp2c2FkejY1MGdyNiZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/dmqnEU3LCyOkM/giphy.gif" width="250">
</a>

  <!-- chainsawman -->
<a href="/cgi-bin/slow.py">
  <img src="https://media3.giphy.com/media/v1.Y2lkPTc5MGI3NjExZXU4OG5odnl4azFwbTlwaGczcHhrdTYwYWhpZTNoMWY4azV1NHF3MCZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/ppsHFdRAAlG8UapZXc/giphy.gif" width="250">
</a>

</div>

</body>
</html>
""")