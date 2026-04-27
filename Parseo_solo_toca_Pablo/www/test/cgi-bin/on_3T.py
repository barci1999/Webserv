#!/usr/bin/env python3
import time
import sys
import os

body = sys.stdin.read()

print("Content-Type: text/html")
print()

print("""<html><style>
	#my-img {
		width: 30%;
	}
</style><body style='font-family: Arial; text-align:center;'>""")


print("<h1>Felicidades erer un Nakama</h1>")

print("<h2>Pero no tengo nada para ti asi que disfruta de este Homer Bailando</h2>")
print()
time.sleep(3)

# GIF estilo Homer bailando (puedes cambiar la URL)

print("<img id='my-img' src='https://media1.giphy.com/media/v1.Y2lkPTc5MGI3NjExbTJvYzEwYXgyM21id3IyanIzbXM5YjZ6MHkyYzY3aXdvemo5ODVjdiZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/2nNI9yM6bxPxBhSCcA/giphy.gif'>")

print("</body></html>")