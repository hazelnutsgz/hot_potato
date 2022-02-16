DEPS = util.h potato.h

all: player ringmaster

player:
	g++ -g -o player player.cpp util.cpp potato.h
ringmaster:
	g++ -g -o ringmaster ringmaster.cpp util.cpp potato.h
clean:
	rm -rf *.o
	rm -rf ringmaster
	rm -rf player