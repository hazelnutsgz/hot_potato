all: player ringmaster

player:
	g++ -g -o player player.cpp
ringmaster:
	g++ -g -o ringmaster ringmaster.cpp 
clean:
	rm -rf *.o
	rm -rf ringmaster
	rm -rf player