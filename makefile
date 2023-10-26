all:
	gcc -o motor motor.c motorFunctions.c -lpthread -pthread -lncurses

motor:
	gcc -o motor motor.c motorFunctions.c -lpthread -pthread -lncurses

clean:
	rm -f motor