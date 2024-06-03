CPPFLAGS = -std=c++20 -Wall -Werror -pedantic -ggdb -pthread
HDRS = TCPClient.h TCPServer.h 2PC_Participant.h
OBJS = TCPClient.o TCPServer.o 2PC_Participant.o

%.o : %.cpp $(HDRS)
	g++ $(CPPFLAGS) -c $< -o $@

coordinator : $(OBJS) coordinator.o
	g++ -lcurses -lpthread $^ -o $@

participant : $(OBJS) participant.o
	g++ -lcurses -lpthread $^ -o $@

all : coordinator participant

clean :
	rm -f *.o coordinator participant
