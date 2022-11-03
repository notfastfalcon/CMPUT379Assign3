CC = g++ -O

# All Targets
all: server client

# Server Files 
server: server.o tands.o 
	$(CC)  -o server server.o tands.o 

server.o: server.cpp tands.o
	$(CC) -c server.cpp 

tands.o: tands.cpp
	$(CC) -c tands.cpp 

# Client Files
client: client.o
	$(CC) -o client client.o

client.o: client.cpp 
	$(CC) -c client.cpp 

# Clean
clean: 
	rm -f *.o *.log server client