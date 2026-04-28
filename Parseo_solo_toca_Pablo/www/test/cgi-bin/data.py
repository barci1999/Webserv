# data.py

IMAGES = {
    "homer_bailando": "https://media1.giphy.com/media/v1.Y2lkPTc5MGI3NjExbTJvYzEwYXgyM21id3IyanIzbXM5YjZ6MHkyYzY3aXdvemo5ODVjdiZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/2nNI9yM6bxPxBhSCcA/giphy.gif",
    
    "homer_arbusto" : "https://media3.giphy.com/media/v1.Y2lkPTc5MGI3NjExZmV2a3Fhb2w0MTI2cWxqaGowa3NzcXRodDNpYXM4cWx4ZWoxN3h1eSZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/GKjO1Ej9uH9rG/giphy.gif",

    "denji_bailando": "https://media.giphy.com/media/ppsHFdRAAlG8UapZXc/giphy.gif",

    "onepiece": "https://media1.giphy.com/media/v1.Y2lkPTc5MGI3NjExOTA2Ym84ZDhqOHQxajh1a3RqaWF6MHg5ZmZic3NzZG1kcGlqdDJ0cyZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/C3brYLms1bhv2/giphy.gif",

    "chainsaw": "https://media1.giphy.com/media/v1.Y2lkPTc5MGI3NjExdXZvbXZ3d3E3dmV3eXQ2bXZ2aXQzcnI2NWsxanBiaHk1MzIxcnFsdiZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/uU42RblvcIOLWwxfEx/giphy.gif",

    "door" : "https://media3.giphy.com/media/v1.Y2lkPTc5MGI3NjExcDA1M285NnZpa2oxZG5la3N0eGRtM2UwZXpmOWo4bWtzaG53YWRwMyZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/aKI9wHGUpIvRtIglEO/giphy.gif",

    "yes" : "https://media4.giphy.com/media/v1.Y2lkPTc5MGI3NjExYnRiMmxzOXN0YXpkaWk1dmlmemc3eXBvNXFzZ2t6YjFodGV0MjdzbSZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9cw/qXJsxIBYlQmEYSVYQi/giphy.gif",

    "no" : "https://media2.giphy.com/media/v1.Y2lkPTc5MGI3NjExMm1tdGJmcGtjcXkxMGp0bGFpdWthY2V6aGYxcTNxNWUzc2V0djcxOCZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9cw/iB1FbEBfilsmhLKAf9/giphy.gif"
}


DATA = {

    "onepiece": {
        "name": "One Piece",
        "start_img": "onepiece",

        "steps": {

            "start": {
                "message": "🏴‍☠️ Bienvenido a One Piece",
                "options": [
                    {"text": "Empezar", "next": "q1"}
                ]
            },

            "q1": {
                "type": "image_options",
                "question": "¿Cómo se llama el creador?",

                "options": [
                    {
                        "text": "Eiichiro Oda",
                        "next": "q2_ok",
                        "image": "door",
                        "style": "width:80%"
                    },
                    {
                        "text": "Akira Toriyama",
                        "next": "q2_fail",
                        "image": "door",
                        "style": "width:80%"
                    }
                ]
            },

            "q2_ok": {
                "type": "image_options",
                "messages": [
                    "🔥 Buena respuesta",
                    "<h2>Ahora una pregunta mas... solo una</h2>",
                    "<h2>El manga sigue en emision</h2>"
                ],
                "options": [
                    {
                        "text": "SI",
                        "next": "win",
                        "image": "yes",
                        "style": "width:80%"
                    },
                    {
                        "text": "NO",
                        "next": "lose",
                        "image": "no",
                        "style": "width:80%"
                    }

                ]
            },

            "q2_fail": {
                "type": "image_options",
                "messages": [
                    "Mala respuesta",
                    "<h2>Te dare una oportunidad mas </h2>",
                    "<h2>El manga sigue en emision</h2>"
                ],

                "options": [
                    {
                        "text": "SI",
                        "next": "win",
                        "image": "yes",
                        "style": "width:80%"
                    },
                    {
                        "text": "NO",
                        "next": "lose",
                        "image": "no",
                        "style": "width:80%"
                    }

                ]
            },

            "win": {
                "messages": [
                    "🎉 Felicidades eres un Nakama",
                    "Pero no tengo nada para ti asi que disfruta..."
                ],
                "image": "homer_bailando"
            },

            "lose": {
                "messages": [
                    "💀 Game Over",
                    "<h2>Como dijeron tus padres:</h2>",
                    "<h3>No esperaba nada y aun asi me decepcionas T.T</h3>"
                ],
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
                "options": [
                    {"text": "Empezar", "next": "q1"}
                ]
            },

            "q1": {
                "type": "image_options",
                "question": "¿Quién es el protagonista?",
                "options": [
                    {
                        "text": "Denji",
                        "next": "q2_ok",
                        "image": "denji_bailando",
                        "style": "width:80%"
                    },
                    {
                        "text": "Pochita",
                        "next": "q2_fail",
                        "image": "denji_bailando",
                        "style": "width:80%"
                    }
                ]
            },

            "q2_ok": {
                "type": "image_options",
                "messages": [
                    "<h1> No me lo esperaba, pero buena respuesta</h1>",
                    "<h2> Veamos si ha sido suerte o es que sabes</h2>",
                    "<h2>El manga sigue en emision</h2>"
                ],
                "options": [
                    {
                        "text": "SI",
                        "next": "win",
                        "image": "yes",
                        "style": "width:80%"
                    },
                    {
                        "text": "NO",
                        "next": "lose",
                        "image": "no",
                        "style": "width:80%"
                    }
                ]
            },

            "q2_fail": {
                "type": "image_options",
                "messages": [
                    "<h1> Puedo entender que falles, pero no era muy dificil</h1>",
                    "<h2> Veamos si en la siguiente tienes suerte</h2>",
                    "<h2> El manga sigue en emision</h2>"
                ],

                "options": [
                    {
                        "text": "SI",
                        "next": "win",
                        "image": "yes",
                        "style": "width:80%"
                    },
                    {
                        "text": "NO",
                        "next": "lose",
                        "image": "no",
                        "style": "width:80%"
                    }
                ]
            },

            "win": {
                "messages": [
                    "<h1>Felicidades por haber acertado</h1>",
                    "<h2>No se si ha sido suerte o te leiste el manga.</h2>",
                    "<h2>Si es así, lo siento</h2>",
                    "<h2>No tengo nada para ti, asi que te dejo esto como recompensa</h2>"
                ],
                "image": "homer_bailando"
            },

            "lose": {
                "messages": [
                    "💀 Game Over",
                    "<h2>No se si es mejor que siguiese:</h2>",
                    "<h3>O que terminase tal y como lo hizo</3>"
                ],
                "image": "homer_arbusto"
            }
        }
    }
}