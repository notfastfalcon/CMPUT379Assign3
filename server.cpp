#include "iostream"
#include "string.h"
#include "queue"
#include "unistd.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>      // inet_aton, htonl, htons
#include <sys/stat.h>
#include "bits/stdc++.h"
using namespace std;


int main (int argc, char *argv[]) {
	//reading from command line
	if (argc != 2) {
		cout << "Invalid command line input!\n";
		return 0;
	}
	//geting port number
	int PORT = atoi (argv[1]);  

	//listening socket and connection socket
	int lstn_soc, conn_soc;

	// declare structure variables that store local and peer socket addresses
	struct sockaddr_in address, client_address; 
	// adress length
	int addrlen = sizeof(struct sockaddr_in);
	
	// creating a socket
	lstn_soc = socket(AF_INET, SOCK_STREAM, 0);
	if (lstn_soc == -1) { 
		//error
		cout << "socket creation failed\n";
		return 1;
	}

  	// set up the socket (initialize socket parameters)
  	//AF_INET for IPv4
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
	address.sin_port = htons(PORT); 

  	// binding to port
	if (bind(lstn_soc, (struct sockaddr *)&address, addrlen) == -1) { 
  		//error
		cout << "Unable to bind to port address\n"; 
		return 1;
	}

  	// listen for connection requests with Backlog = 10
	if (listen(lstn_soc, 10) == -1) {
  		//error 
		cout << "Listen failed\n";
		return 1;
	}
  	//listening message
	cout << "Listening to " << inet_ntoa(address.sin_addr) << ":" << ntohs(address.sin_port) << endl;

  	// accept a connection request
	conn_soc = accept(lstn_soc, (struct sockaddr *)&client_address, (socklen_t*)&addrlen);
	cout << "Connection request accepted from " << inet_ntoa(client_address.sin_addr) << ":" << ntohs(client_address.sin_port) << endl;
	
	return 0;
}
