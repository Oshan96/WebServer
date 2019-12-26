all: compile run

compile:
	gcc WebServer.c -o WebServer

run:
	./WebServer
