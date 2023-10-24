all:
	gcc -o motor motor.c motorFunctions.c -lpthread -pthread

motor:
	gcc -o motor motor.c motorFunctions.c -lpthread -pthread

clean:
	rm -f motor