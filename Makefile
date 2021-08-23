snake: snake.o
	gcc -o snake snake.o

snake.o: snake.c snake.h screen.h
	gcc -c snake.c
