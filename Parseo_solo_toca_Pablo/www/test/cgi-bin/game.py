#!/usr/bin/env python3
import os
from urllib.parse import parse_qs
from data import DATA, IMAGES

print("Content-Type: text/html; charset=utf-8\n")

def img(key):
    url = IMAGES.get(key, "")
    return f'<img src="{url}" alt="">' if url else ""

def get_params():
    params = parse_qs(os.environ.get("QUERY_STRING", ""))
    anime = params.get("anime", [""])[0]
    step  = params.get("step", ["menu"])[0]
    return anime, step

anime, step = get_params()

# -------- MENU --------
if step == "menu":
    print("<h1>🎌 Elige un anime</h1>")
    print("<div class='game-menu'>")
    for key, anime_data in DATA.items():
        print(f"""
        <div class="option" data-anime="{key}" data-step="start">
            {img(anime_data['start_img'])}
            <h2>{anime_data['name']}</h2>
        </div>
        """)
    print("</div>")

# -------- GAME --------
else:
    data = DATA[anime]["steps"][step]
    print("<div class='game-main'>")

    if "messages" in data:
        for msg in data["messages"]:
            print(f"<h2>{msg}</h2>")
    elif "message" in data:
        print(f"<h2>{data['message']}</h2>")

    if "question" in data:
        print(f"<h1>{data['question']}</h1>")

    if "image" in data:
        print(img(data["image"]))

    if "options" in data:
        print("<div class='game-menu'>")
        for opt in data["options"]:
            img_html = img(opt["image"]) if "image" in opt else ""
            print(f"""
            <div class="option" data-anime="{anime}" data-step="{opt['next']}">
                {img_html}
                <h3>{opt['text']}</h3>
            </div>
            """)
        print("</div>")

    print("</div>")
