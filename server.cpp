#include "iostream"
#include "string.h"
#include "cmath"
#include "unistd.h"
#include "fcntl.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>      // inet_aton, htonl, htons
#include <sys/stat.h>
#include "bits/stdc++.h"
#include "sys/time.h"
#include "unordered_map"
#include "header.h"
using namespace std;

int lstn_soc, conn_soc, opt = 1;
struct sockaddr_in address; 
struct timeval tv, start;
unordered_map<string, int> workForClient;

//tands.cpp function
extern void Trans(int n);

/**
 * Prints the current EPOCH time 
 */
void printEpochTime() {
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
 * This function is used to set up a connection with Client.
 * It is highly inspired from author's CMPUT275 Winter 2021 Assignment-2 code. 
 * This following code is the courtesy of CMPUT 275 Winter 2021 Professors and TAs.
 * @param: argc: count of arguments
 * @param: argv: input buffer (command arguments)
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

	cout << "Using port " << ntohs(address.sin_port) << "\n";

  	// accept a connection request
	conn_soc = accept(lstn_soc, NULL, NULL);
	if (conn_soc == -1) {
		cout << "Accept Error\n";
		close(conn_soc);
		exit(1);
	}

	/***TEST CODE START***/

	char inBuffer[1024] = {};
	int valread = read(conn_soc, inBuffer, sizeof inBuffer);

	if (valread < 0) {
		cout << "Error!\n";
	}

	cout << string(inBuffer) << "\n";

	int count = 1;
	string outString = "D" + to_string(count);
	char outBuffer[3] = {};
	strcpy(outBuffer, outString.c_str());
	send(conn_soc, outBuffer, sizeof outBuffer, 0);

	/****TEST CODE END ***/
}

/**
 * Handles main server operations
 */
void serverOperations() {
	while(true) {
		//start timer after first transaction is recived
		gettimeofday(&start, NULL);
		break;
	}

}

/**
 * Close all sockets
 */
void closeConnection() {
	close(conn_soc);
	close(lstn_soc);
}

/**
 * Print the Final Client Summary
 */
void finalSummary() {
	int totalTrans = 0;
	cout << "SUMMARY\n";
	for (auto client : workForClient) {
		cout << "\t" << client.second << " transactions from " << client.first << "\n";
		totalTrans += client.second;
	}
	gettimeofday(&tv, NULL);
	float execTime = (float) (tv.tv_sec - start.tv_sec) + (pow((tv.tv_usec - start.tv_usec), -6));
	float transPerSecond = 0.0;
	if (execTime > 0.0) {
		transPerSecond = totalTrans/execTime;
	}
	printf("%.1f", transPerSecond);
	cout << " transactions/sec\t(" << totalTrans << "/";
	printf("%.2f)\n", execTime); 
}

/**
 * Driver Code
 */
int main (int argc, char *argv[]) {
	string filename = "server.log";
	loggedToFile(filename);
	serverConnection(argc, argv);
	serverOperations();
	closeConnection();
	finalSummary();
	return 0;
}
