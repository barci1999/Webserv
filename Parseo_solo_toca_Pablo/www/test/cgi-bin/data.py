# data.py

IMAGES = {
    "homer_bailando": "https://media.giphy.com/media/2nNI9yM6bxPxBhSCcA/giphy.gif",
    "homer_arbusto" : "https://media.giphy.com/media/GKjO1Ej9uH9rG/giphy.gif",
    "denji_bailando": "https://media.giphy.com/media/ppsHFdRAAlG8UapZXc/giphy.gif",
    "onepiece": "https://media.giphy.com/media/C3brYLms1bhv2/giphy.gif",
    "chainsaw": "https://media.giphy.com/media/uU42RblvcIOLWwxfEx/giphy.gif",
    "door" : "https://media.giphy.com/media/aKI9wHGUpIvRtIglEO/giphy.gif",
    "yes" : "https://media.giphy.com/media/qXJsxIBYlQmEYSVYQi/giphy.gif",
    "no" : "https://media.giphy.com/media/iB1FbEBfilsmhLKAf9/giphy.gif"
}

# El resto del DATA se mantiene igual, ya que las llaves coinciden perfectamente.
DATA = {
    "onepiece": {
        "name": "One Piece",
        "start_img": "onepiece",
        "steps": {
            "start": {
                "message": "🏴‍☠️ Bienvenido a One Piece",
                "options": [{"text": "Empezar", "next": "q1"}]
            },
            "q1": {
                "type": "image_options",
                "question": "¿Cómo se llama el creador?",
                "options": [
                    {"text": "Eiichiro Oda", "next": "q2_ok", "image": "door"},
                    {"text": "Akira Toriyama", "next": "q2_fail", "image": "door"}
                ]
            },
            "q2_ok": {
                "messages": ["🔥 Buena respuesta", "<h2>Ahora una pregunta mas... solo una</h2>"],
				"question": "¿El manga sige en emision o no?",
                "options": [
                    {"text": "SI", "next": "win", "image": "yes"},
                    {"text": "NO", "next": "lose", "image": "no"}
                ]
            },
            "q2_fail": {
                "messages": ["Mala respuesta", "<h2>Te dare una oportunidad mas</h2>"],
				"question": "¿El manga sige en emision o no?",
                "options": [
                    {"text": "SI", "next": "win", "image": "yes"},
                    {"text": "NO", "next": "lose", "image": "no"}
                ]
            },
            "win": {
                "messages": ["🎉 Felicidades eres un Nakama", "Pero no tengo nada para ti asi que disfruta ..."],
                "image": "homer_bailando"
            },
            "lose": {
                "messages": ["💀 Game Over", "Como dijeron tus padres:", "No esperaba nada de ti y aun asi me decepcionaste"],
                "image": "homer_arbusto"
            }
        }
    },
    "chainsaw": {
        "name": "Chainsaw Man",
        "start_img": "chainsaw",
        "steps": {
            "start": {
                "message": "🔪 Bienvenido a Chainsaw Man",
                "options": [{"text": "Empezar", "next": "q1"}]
            },
            "q1": {
                "question": "¿Quién es el protagonista?",
                "options": [
                    {"text": "Denji", "next": "q2_ok", "image": "denji_bailando"},
                    {"text": "Pochita", "next": "q2_fail", "image": "denji_bailando"}
                ]
            },
            "q2_ok": {
                "messages": ["<h1>No me lo esperaba, pero buena respuesta</h1>", "<h2> Veamos si ha sido suerte o es que sabes</h2>"],
				"question": "¿El manga sige en emision o no?",
                "options": [
                    {"text": "SI", "next": "lose", "image": "yes"},
                    {"text": "NO", "next": "win", "image": "no"}
                ]
            },
            "q2_fail": {
                "messages": ["<h1>Puedo entender que falles, pero no era tan dificil</h1>", "<h2>Veamos si en la siguiente tienes suerte</h2>"],
				"question": "¿El manga sige en emision o no?",
                "options": [
                    {"text": "SI", "next": "lose", "image": "yes"},
                    {"text": "NO", "next": "win", "image": "no"}
                ]
            },
            "win": {"messages": ["<h1>Ganaste</h1>","<h2>No se si ha sido suerte o te leiste el manga.</h2>", "<h2>Si es así lo siento</h2>", "<h2>Pero sea como sea toma esto de recompensa</h2>"], "image": "homer_bailando"},
            "lose": {"messages": ["💀 Game over", "<h2>No se si es mejor que siguiese</h2>", "<h2>o que terminase tal y como lo hizo</2>"], "image": "homer_arbusto"}
        }
    }
}