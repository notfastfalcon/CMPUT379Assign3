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
	if (argc != 3) {
		cout << "Invalid command line input!\n";
		return 0;
	}

	//server port
	int SERVER_PORT = atoi(argv[1]);
	//IP address
	char const *SERVER_IP = argv[2];

	// Declare structure variables that store local and peer socket addresses
	struct sockaddr_in my_addr, peer_addr;

	// zero out the structor variable because it has an unused part
	memset(&my_addr, '\0', sizeof my_addr);

	// Declare socket descriptor
	int serv_soc = 0;
	serv_soc = socket(AF_INET, SOCK_STREAM, 0);
	//error handling
	if (serv_soc == -1) {
		cerr << "Socket creation failed!\n";
		return 1;
	}
	// Prepare sockaddr_in structure variable
	//AF_INET for IPv4
	peer_addr.sin_family = AF_INET;
	peer_addr.sin_port = htons(SERVER_PORT);

	// internet address (4 bytes). INADDR_LOOPBACK is localhost address
	peer_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	// connecting to the server socket
	if (connect(serv_soc, (struct sockaddr *) &peer_addr, sizeof peer_addr) == -1) {
		//cerror if connection not established
		cerr << "Cannot connect to the Server!\n";
		close(serv_soc);
		return 1;
	}

	//if connection established
	cout << "Connection established with " << inet_ntoa(peer_addr.sin_addr) << ":" << ntohs(peer_addr.sin_port) << "\n";


	return 0;
}