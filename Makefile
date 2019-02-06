CC=gcc
CFLAGS=-g3 
LIBS=-lGL -lm -lglfw

all: boidsSim

boidsSim: graphics.o boid.o quadtree.o utils.o main.o
	$(CC) $(CFLAGS) $(LIBS) utils.o quadtree.o graphics.o boid.o main.o -o boids


main.o: main.c
	$(CC) -c main.c

graphics.o: graphics.c
	$(CC) -c graphics.c

boids.o: boid.c
	$(CC) -c boid.c


quadtree.o: quadtree.c
	$(CC) -c quadtree.c


utils.o: utils.c
	$(CC) -c utils.c

