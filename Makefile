CC = g++ -O -W

# All Targets
all: server client

# Server Files 
server: server.o tands.o 
	$(CC)  -o server server.o tands.o 

server.o: server.cpp tands.o
	$(CC) -c server.cpp 

# Client Files
client: client.o tands.o
	$(CC) -o client client.o tands.o

client.o: client.cpp tands.o
	$(CC) -c client.cpp 

# tANDs File
tands.o: tands.cpp
	$(CC) -c tands.cpp 

# Clean
clean: 
	rm -f *.o *.log server client