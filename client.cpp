#include "iostream"
#include "string.h"
#include "queue"
#include "unistd.h"
#include "fcntl.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>      // inet_aton, htonl, htons
#include <sys/stat.h>
#include "bits/stdc++.h"
#include "sys/time.h"
#include "header.h"
using namespace std;

struct sockaddr_in address;
int serv_soc = 0;
int totalTransactions = 0;
bool clientSleeping = false;

//tands.cpp function
extern void Sleep(int n);


/**
 * Gets the client ID i.e. machineName.pid
 * @return Client Name
 */
string getHostName() {
	string hostName = "";
	char host[32];
	if (gethostname(host, 32) < 0) {
		perror("gethostname() failed");
	}
	hostName += string(host) + "." + to_string(getpid());
	return hostName;
}

/**
 * Prints the current EPOCH time 
 */
void printEpochTime() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	//by default tv_usec gives 6 digits but we just want 2, so we divide by 10000
	cout << tv.tv_sec << "." << (tv.tv_usec/10000);
}


/**
 * Redirect STDOUT to given Filename
 * @param: filename: Log file where output is redirected.
 */
void loggedToFile(string filename) {
	int outfd;
	// 0666 was included to give rw permission
	if ((outfd = open(filename.c_str(), O_CREAT | O_WRONLY, 0666)) < 0) {
		perror("Open outfd failed!");
	}
	dup2(outfd, STDOUT_FILENO);
	close(outfd);
}

/**
 * This function is used to set up a connection with Server.
 * It is highly inspired from author's CMPUT275 Winter 2021 Assignment-2 code. 
 * This following code is the courtesy of CMPUT 275 Winter 2021 Professors and TAs.
 * @param: argc: count of arguments
 * @param: argv: input buffer (command arguments)
 */
void clientConnection(int argc, char* argv[]) {
	//reading from command line
	if (argc != 3) {
		cout << "Invalid command line input!\n";
		exit(1);
	}

	//server port
	int SERVER_PORT = atoi(argv[1]);

	if (SERVER_PORT < 5000 || SERVER_PORT > 64000) {
		cout << "Invalid Connection Port. Should be between 5000 and 64000\n";
		exit(1);
	}

	//IP address
	char const *SERVER_IP = argv[2];

	serv_soc = socket(AF_INET, SOCK_STREAM, 0);
	//error handling
	if (serv_soc == -1) {
		cout << "Socket creation failed!\n";
		exit(1);
	}
	// Prepare sockaddr_in structure variable
	//AF_INET for IPv4
	address.sin_family = AF_INET;
	address.sin_port = htons(SERVER_PORT);

	//converts custom IP address to binary form
	if (inet_pton(AF_INET, SERVER_IP, &address.sin_addr) == -1) {
		cout << "Convertion of IPv4 address from text to binary failed\n";
		exit(1);
	}

	// connecting to the server socket
	if (connect(serv_soc, (struct sockaddr *) &address, sizeof address) == -1) {
		//cerror if connection not established
		cout << "Cannot connect to the Server!\n";
		close(serv_soc);
		exit(1);
	}

	//if connection established
	cout << "Using port " << ntohs(address.sin_port) << "\n";
	cout << "Using server address " << inet_ntoa(address.sin_addr) << "\n";
	cout << "Host " << getHostName() << "\n";
}

/**
 * Main Client Write Operations
 */
void clientWriteOperations() {
	string input = "";

	//input is guaranteed to be correct. So no try catch block implemented.
	while (cin >> input) {
		if (input[0] == 'T') {
			clientSleeping = false;
			string outString = input.substr(1) + "." + getHostName();
			char outBuffer[outString.length()] = {};
			strcpy(outBuffer, outString.c_str());

			//write this command to socket
			send(serv_soc, outBuffer, sizeof outBuffer, 0);

			//write to log file
			printEpochTime();
			cout << ": Send (" << input << ")\n";

			//update transactions sent 
			totalTransactions++;
		}
		else if (input[0] == 'S') {
			clientSleeping = true;
			//put client to sleep
			int units = stoi(input.substr(1));
			cout << "Sleep " << units << " units\n";
			Sleep(units);
		}
		if (!clientSleeping) {
			clientReadOperations();
		}
	}
}

/**
 * Main Client Read Operations
 */
void clientReadOperations() {
	char inBuffer[3] = {};
	string inString = "";

	//read from socket
	if (read(serv_soc, inBuffer, sizeof inBuffer) == -1) {
		cout << "Error reading from Server\n";
		exit(1);
	}

	//convert from cstring to string
	for (unsigned int i = 0; i < strlen(inBuffer); i++) {
		inString += inBuffer[i];
	}
	if (!inString.empty()) {
		//write to log file
		printEpochTime();
		cout << ": Recv (" << inString << ")\n";
	}
}

/**
 * Close all sockets and send terminating message to server
 */
void closeConnection() {
	//send that all work is done to server
	char outBuffer[2] = {'Q'};
	send(serv_soc, outBuffer, sizeof outBuffer, 0);
	close(serv_soc);
}

/**
 * Print the Final Client Summary
 */
void finalSummary() {
	cout << "Sent " << totalTransactions << " transactions\n";
}

/**
 * Driver Code
 */
int main (int argc, char *argv[]) {
	string filename = getHostName() + ".log";
	loggedToFile(filename);
	clientConnection(argc, argv);
	clientWriteOperations();
	closeConnection();
	finalSummary();
	return 0;
}