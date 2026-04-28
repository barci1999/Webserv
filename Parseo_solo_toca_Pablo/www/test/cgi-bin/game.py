#!/usr/bin/env python3
import os
from urllib.parse import parse_qs
from data import DATA, IMAGES

print("Content-Type: text/html\n")

# -------- helpers --------

def render_image(key, size="60%"):
    url = IMAGES.get(key, "")
    return f"<img src='{url}' style='width:{size}'>"


def get_params():
    params = parse_qs(os.environ.get("QUERY_STRING", ""))
    anime = params.get("anime", [""])[0]
    step = params.get("step", ["menu"])[0]
    return anime, step


# -------- HTML base --------

print("""
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">

<style>
html, body {
    margin: 0;
    padding: 0;
}

body {
    font-family: Arial;
    background: #111;
    color: white;
    text-align: center;
}

/* contenedor principal */
.page {
    max-width: 1000px;
    margin: 0 auto;
    padding: 40px 20px;

    min-width: 700px; 
}

/* links blancos */
a {
    color: white;
    text-decoration: none;
}

/* opciones */
.container {
    display: flex;
    justify-content: center;
    flex-wrap: wrap;
    gap: 30px;
    margin-top: 30px;
}

/* tarjetas */
.option {
    width: 250px;
}

/* imágenes */
img {
    width: 100%;
    border-radius: 10px;
}

/* texto */
h1, h2, h3 {
    max-width: 800px;
    margin-left: auto;
    margin-right: auto;
}
</style>

</head>
<body>
<div class="page">
""")

anime, step = get_params()

# -------- MENU --------

if step == "menu":
    print("<h1>🎌 Elige un anime</h1>")
    print("<div class='container'>")

    for key, anime_data in DATA.items():
        print(f"""
        <div class="option">
            <a href="/cgi-bin/game.py?anime={key}&step=start">
                {render_image(anime_data['start_img'], "80%")}
                <h2>{anime_data['name']}</h2>
            </a>
        </div>
        """)

    print("</div>")


# -------- GAME --------

else:
    data = DATA[anime]["steps"][step]

    if "messages" in data:
        for msg in data["messages"]:
            print(f"<h2>{msg}</h2>")

    elif "message" in data:
        print(f"<h2>{data['message']}</h2>")

    if "question" in data:
        print(f"<h1>{data['question']}</h1>")

    if "image" in data:
        print(render_image(data["image"]))

    if "options" in data:
        print("<div class='container'>")

        for opt in data["options"]:
            img_html = ""
            if "image" in opt:
                img_html = render_image(opt["image"], "70%")

            print(f"""
            <div class="option">
                <a href="/cgi-bin/game.py?anime={anime}&step={opt['next']}">
                    {img_html}
                    <h3>{opt['text']}</h3>
                </a>
            </div>
            """)

        print("</div>")


print("</div></body></html>")