RAYLIB ?= ./external/raylib-5.5/src/

SOURCE = ./raylife.c

all:
	gcc $(SOURCE) -I $(RAYLIB) -L $(RAYLIB) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o raylife

clean:
	rm -f raylife
