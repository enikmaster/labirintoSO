all:
	gcc -o motor BACKEND/motor.c BACKEND/motorFunctions.c -lpthread -pthread -lncurses
	gcc -o jogoUI FRONTEND/jogoUI.c FRONTEND/jogoUIFunctions.c -lpthread -pthread -lncurses
	chmod +x backend.sh

clean:
	rm -f motor jogoUI
