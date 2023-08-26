all:
	gcc src/main.c -lGL -lraylib -ggdb -o weeds -lpthread -Iinclude -Llib -lm -fsanitize=undefined -fsanitize=address

run: all
	./weeds