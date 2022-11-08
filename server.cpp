#include "iostream"
#include "string.h"
#include "queue"
#include "unistd.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>      // inet_aton, htonl, htons
#include <sys/stat.h>
#include "bits/stdc++.h"
#include "header.h"
using namespace std;

int lstn_soc, conn_soc, opt = 1;
struct sockaddr_in address; 


/**
 * This function is used to set up a connection with Client.
 * It is highly inspired from author's CMPUT275 Winter 2021 Assignment-2 code. 
 * This following code is the courtesy of CMPUT 275 Winter 2021 Professors and TAs.
 */
void serverConnection(int argc, char* argv[]) {
	//reading from command line
	if (argc != 2) {
		cout << "Invalid command line input!\n";
		exit(1);
	}
	//geting port number
	int PORT = atoi (argv[1]);

	if (PORT < 5000 || PORT > 64000) {
		cout << "Invalid Connection Port. Should be between 5000 and 64000\n";
		exit(1);
	}
	
	// creating a socket
	lstn_soc = socket(AF_INET, SOCK_STREAM, 0);
	if (lstn_soc == -1) { 
		//error
		cout << "Socket creation failed\n";
		exit(1);
	}

  	// set up the socket (initialize socket parameters)
  	//AF_INET for IPv4
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT); 

  	// binding to port
	if (bind(lstn_soc, (struct sockaddr *)&address, sizeof address) == -1) { 
  		//error
		cout << "Unable to bind to port address\n"; 
		exit(1);
	}

  	// listen for connection requests with Backlog = 10
	if (listen(lstn_soc, 10) == -1) {
  		//error 
		cout << "Listen failed\n";
		exit(1);
	}

  	// accept a connection request
	conn_soc = accept(lstn_soc, NULL, NULL);
	if (conn_soc == -1) {
		cout << "Accept Error\n";
		close(conn_soc);
		exit(1);
	}
	cout << "Using port " << ntohs(address.sin_port) << "\n";
}


int main (int argc, char *argv[]) {
	serverConnection(argc, argv);
	return 0;
}
