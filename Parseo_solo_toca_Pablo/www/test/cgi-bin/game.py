#!/usr/bin/env python3
import os

print("Content-Type: text/html")
print()

# 📦 DATOS DEL JUEGO (AQUÍ ESTÁ LA MAGIA)
DATA = {
    "onepiece": {
        "name": "One Piece",
        "gif": "https://media1.giphy.com/media/v1.Y2lkPTc5MGI3NjExOTA2Ym84ZDhqOHQxajh1a3RqaWF6MHg5ZmZic3NzZG1kcGlqdDJ0cyZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/C3brYLms1bhv2/giphy.gif",
        "q1": {
            "question": "¿Cómo se llama el creador?",
            "correct": "Eiichiro Oda",
            "wrong": "Akira Toriyama"
        },
        "final_win": "Eres un Nakama 😎",
        "final_gif": "https://media1.giphy.com/media/2nNI9yM6bxPxBhSCcA/giphy.gif"
    },

    "chainsaw": {
        "name": "Chainsaw Man",
        "gif": "https://media1.giphy.com/media/v1.Y2lkPTc5MGI3NjExdXZvbXZ3d3E3dmV3eXQ2bXZ2aXQzcnI2NWsxanBiaHk1MzIxcnFsdiZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/uU42RblvcIOLWwxfEx/giphy.gif",
        "q1": {
            "question": "¿Quién es el protagonista?",
            "correct": "Denji",
            "wrong": "Naruto"
        },
        "final_win": "Eres un cazador de demonios 🔪",
        "final_gif": "https://media.giphy.com/media/ppsHFdRAAlG8UapZXc/giphy.gif"
    }
}

# 📥 PARÁMETROS
query = os.environ.get("QUERY_STRING", "")
params = {}

for p in query.split("&"):
    if "=" in p:
        k, v = p.split("=")
        params[k] = v

anime = params.get("anime", "")
step = params.get("step", "0")
res = params.get("res", "")

print("""
<html>
<style>
body { text-align:center; font-family:Arial; }
.myimg { width: 80%; }
img:hover { transform: scale(1.1); }
</style>
<body>
""")

# 🟢 STEP 0 → elegir anime
if step == "0":
    print("<h1>Elige un anime</h1><div style='display:flex; justify-content:center;'>")

    for key, data in DATA.items():
        print(f"""
        <a href="/cgi-bin/game.py?anime={key}&step=1">
            <img class="myimg" src="{data['gif']}">
        </a>
        """)

    print("</div>")

# 🟡 STEP 1 → pregunta 1
elif step == "1":

    data = DATA[anime]

    print(f"""
    <h1>{data['q1']['question']}</h1>

    <a href="/cgi-bin/game.py?anime={anime}&step=2&res=ok">
        <h2>{data['q1']['correct']} ✅</h2>
    </a>

    <a href="/cgi-bin/game.py?anime={anime}&step=2&res=fail">
        <h2>{data['q1']['wrong']} ❌</h2>
    </a>
    """)

# 🔵 STEP 2 → pregunta común (Matrix)
elif step == "2":

    titulo = "✅ Buena respuesta" if res == "ok" else "❌ Mala respuesta, última oportunidad"

    print(f"""
    <h1>{titulo}</h1>
    <h2>¿El manga sigue en emisión?</h2>

    <div style="position:relative; display:inline-block;">
        <img src="https://media2.giphy.com/media/v1.Y2lkPTc5MGI3NjExdDQ3Y3hyamRmb3FqMnE4ZTFxb3g4dWVrcTFtaHM5N2s5c25oajZheSZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/5iWX6XFTndU0YP0Yut/giphy.gif" width="500">

        <a href="/cgi-bin/game.py?anime={anime}&step=3&res=ok"
           style="position:absolute; left:0; top:0; width:50%; height:100%;"></a>

        <a href="/cgi-bin/game.py?anime={anime}&step=3&res=fail"
           style="position:absolute; right:0; top:0; width:50%; height:100%;"></a>
    </div>
    """)

# 🔴 FINAL
elif step == "3":

    data = DATA[anime]

    if res == "ok":
        print(f"""
        <h1>🎉 {data['final_win']}</h1>
        <img class="myimg" src="{data['final_gif']}">
        """)
    else:
        print("""
        <h1>💀 Game Over</h1>
        <img class="myimg" src="https://media3.giphy.com/media/GKjO1Ej9uH9rG/giphy.gif">
        """)

print("</body></html>")