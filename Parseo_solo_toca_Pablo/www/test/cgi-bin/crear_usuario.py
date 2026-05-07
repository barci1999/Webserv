#!/usr/bin/python3 -u
import sys
import os
import urllib.parse

# Headers
print("Content-Type: text/html\r\n\r\n")

method = os.environ.get('REQUEST_METHOD', 'GET')
content_length = os.environ.get('CONTENT_LENGTH', '0')

if method == "POST":
    try:
        length = int(content_length)
        body = sys.stdin.read(length)
        
        params = urllib.parse.parse_qs(body.strip())
        
        nombre = params.get('nombre', ['Desconocido'])[0]
        apellido = params.get('apellido', ['SinApellido'])[0]
        
        filename = f"{nombre}_{apellido}.txt"

        # 🔴 Ruta al directorio ../upload relativa al script
        script_dir = os.path.dirname(os.path.abspath(__file__))
        upload_dir = os.path.join(script_dir, "../../upload")

        # Crear la carpeta si no existe
        os.makedirs(upload_dir, exist_ok=True)

        file_path = os.path.join(upload_dir, filename)

        with open(file_path, "w") as f:
            f.write(f"Registro: {nombre} {apellido}\n")
        
        print(f"<html><body><h1>✅ Creado: {file_path}</h1>")
        
    except Exception as e:
        print(f"<h1>❌ Error: {str(e)}</h1>")
else:
    print("<h1>Usa POST</h1>")