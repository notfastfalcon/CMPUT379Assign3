CC = g++ -O -W

# All Targets
all: server client manServer manClient

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

# man Files
manClient:
	groff -man -Tpdf < client.man >  client.pdf

manServer:
	groff -man -Tpdf < server.man >  server.pdf

# Clean
clean: 
	rm -f *.o *.log server.pdf client.pdf server client