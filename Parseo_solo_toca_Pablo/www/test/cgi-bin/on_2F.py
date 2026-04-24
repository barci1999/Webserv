#!/usr/bin/env python3

print("Content-Type: text/html")
print()

print("""
<html>
<body style="text-align:center; font-family:Arial;">

<h1> Mala respuesta</h1>
<h2>Te dare una oportunidad mas </h2>

<h2>El manga sigue en emision</h2>

<div style="position:relative; display:inline-block;">

  <!-- GIF -->
  <img src="https://media2.giphy.com/media/v1.Y2lkPTc5MGI3NjExdDQ3Y3hyamRmb3FqMnE4ZTFxb3g4dWVrcTFtaHM5N2s5c25oajZheSZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/5iWX6XFTndU0YP0Yut/giphy.gif"
       width="500">

  <!-- ZONA IZQUIERDA (SI) -->
  <a href="/cgi-bin/on_3T.py"
     style="position:absolute; left:0; top:0; width:50%; height:100%; display:block;">
  </a>

  <!-- ZONA DERECHA (NO) -->
  <a href="/cgi-bin/on_3F.py"
     style="position:absolute; right:0; top:0; width:50%; height:100%; display:block;">
  </a>

</div>

<h2> SI_____________________NO</h2>

</body>
</html>
""")