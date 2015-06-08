all:	$(DIR)obiden

obiden: main.cpp
	g++ -std=c++0x -Wall -o obiden main.cpp host.cpp networking.cpp timer.cpp timer.h packets.h networking.h host.h

clean:
	rm -f *.o *~ obiden
