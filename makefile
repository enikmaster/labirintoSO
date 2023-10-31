all: motor jogoUI
	chmod +x labirinto.sh

motor: BACKEND/motor.c BACKEND/motorFunctions.c
	gcc -o motor BACKEND/motor.c BACKEND/motorFunctions.c -lpthread -pthread -lncurses

jogoUI: FRONTEND/jogoUI.c FRONTEND/jogoUIFunctions.c
	gcc -o jogoUI FRONTEND/jogoUI.c FRONTEND/jogoUIFunctions.c -lpthread -pthread -lncurses

clean:
	rm -f motor jogoUI
