#!/usr/bin/env python3

print("Content-Type: text/html")
print()
import time
time.sleep(2)

print("""
<html>
<style>
	.my-img {
		width: 90%;
	}
</style>
<body style="text-align:center;">

<h1>           Elige un anime</h1>

<div style="display:flex; justify-content:center; gap:20px;">

  <!-- ONE PIECE -->
<a href="/cgi-bin/on_1.py">
  <img class="my-img" src="https://media1.giphy.com/media/v1.Y2lkPTc5MGI3NjExOTA2Ym84ZDhqOHQxajh1a3RqaWF6MHg5ZmZic3NzZG1kcGlqdDJ0cyZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/C3brYLms1bhv2/giphy.gif" >
</a>

  <!-- chainsawman -->
<a href="/cgi-bin/cm_1.py">
  <img class="my-img" src="https://media1.giphy.com/media/v1.Y2lkPTc5MGI3NjExdXZvbXZ3d3E3dmV3eXQ2bXZ2aXQzcnI2NWsxanBiaHk1MzIxcnFsdiZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/uU42RblvcIOLWwxfEx/giphy.gif" >
</a>

</div>

</body>
</html>
""")