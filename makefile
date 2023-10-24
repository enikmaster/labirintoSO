all:
	gcc -o motor motor.c -lpthread

motor:
	gcc -o motor motor.c -lpthread

clean:
	rm -f motor