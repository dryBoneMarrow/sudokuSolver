all: main.c
	gcc main.c -o main
run: main.c
	gcc main.c -o main
	./main
clean:
	rm main

