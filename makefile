CPPFLAGS = -std=c++20 -Wall -Werror -pedantic -ggdb -pthread
OBJS = TCPClient.o TCPServer.o

%.o : %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

coordinator : $(OBJS) coordinator.o
	g++ -lcurses -lpthread $^ -o $@

participant : $(OBJS) participant.o

all : coordinator participant

clean :
	rm -f *.o coordinator participant
