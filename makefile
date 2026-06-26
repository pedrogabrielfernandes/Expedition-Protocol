CC = gcc
CFLAGS = -Wall
LIBS = -lallegro -lallegro_main -lallegro_font -lallegro_ttf -lallegro_image -lallegro_primitives -lallegro_audio -lallegro_acodec -lm

SRCS = main.c \
       modulos/sistemas/colisao/colisao.c \
       modulos/sistemas/sanidade/sanidade.c \
       modulos/sistemas/sons/sons.c \
       modulos/sistemas/vida/vida.c \
       modulos/entidades/zumbis/zumbis.c \
       modulos/interface/interface.c \
       modulos/sistemas/pocao/pocao.c \
       modulos/interface/particulas/particulas.c \
       modulos/entidades/samurai/samurai.c \
       modulos/interface/ranking/ranking.c \
       modulos/interface/game_over/game_over.c \
       modulos/cutciene/cutciene.c \
       modulos/mapa/mapa.c \
       modulos/interface/selecao_fase/selecao_fase.c \
       modulos/entidades/boss/boss.c

teste: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o teste $(LIBS)

clean:
	rm -f teste