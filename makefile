all: motor jogoUI

motor: motor.c motorFunctions.c
	gcc -o motor motor.c motorFunctions.c -lpthread -pthread -lncurses

jogoUI: jogoUI.c jogoUIFunctions.c
	gcc -o jogoUI jogoUI.c jogoUIFunctions.c -lpthread -pthread -lncurses

clean:
	rm -f motor jogoUI
