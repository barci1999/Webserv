#!/usr/bin/env python3

print("Content-Type: text/html")
print()

print("""
<html>
<style>
.myimg{
	width : 80%;
}
</style>
<body style="text-align:center; font-family:Arial;">

<h1>¿Quién es el protagonista?</h1>

<div style="display:flex; justify-content:center; gap:40px;">

  <!-- CORRECTA -->
  <div>
    <a href="/cgi-bin/cm_2T.py">
      <img class="myimg" src="https://media.giphy.com/media/ppsHFdRAAlG8UapZXc/giphy.gif">
    </a>
    <h3>Denji </h3>
  </div>

  <!-- INCORRECTA -->
  <div>
    <a href="/cgi-bin/cm_2F.py">
      <img class="myimg" src="https://media.giphy.com/media/ppsHFdRAAlG8UapZXc/giphy.gif">
    </a>
    <h3>Pochita </h3>
  </div>

</div>

</body>
</html>
""")