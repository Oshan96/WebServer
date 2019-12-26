all: compile run

compile:
	gcc queue.c -c
	gcc queue.o WebServer.c -pthread -o WebServer

run:
	./WebServer
