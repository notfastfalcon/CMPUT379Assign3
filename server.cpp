#include "iostream"
#include "string.h"
#include "cmath"
#include "unistd.h"
#include "fcntl.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>      // inet_aton, htonl, htons
#include <sys/stat.h>
#include "sys/ioctl.h"      // set non-blocking socket
#include "bits/stdc++.h"
#include "sys/time.h"
#include "unordered_map"
#include "vector"
#include "header.h"
using namespace std;

int lstn_soc, conn_soc, opt = 1, transNumber = 0;
fd_set fds; 

struct sockaddr_in address; 
struct timeval tv, startTime, endTime;

unordered_map<string, int> workForClient;
vector<int> clientSockets;

//tands.cpp function
extern void Trans(int n);

/**
 * Prints the current EPOCH time 
 */
void printEpochTime() {
	gettimeofday(&tv, NULL);
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
 * Logs the operation output
 * @param: hostName: Name of the client 
 * @param: work: The work to be done by the server
 * @param: flag: Flag to differentiate work and done outputs
 */
 void printOutput(string hostName, int work, int flag) {
 	printEpochTime();
 	cout << ":\t# " << transNumber << " (";
 	if(flag == 0) {
 		cout << "T" << work;
 	}
 	else {
 		cout << "Done";
 	}
 	cout << ") from " << hostName << "\n";
 } 

/**
 * Checks if hostName already exists or not and updates work done accordingly
 * @param: hostName: Name of the client
 */
void addToUMap(string hostName) {
	auto i = workForClient.find(hostName);
	//if its a new client set work done as 1
	if(i == workForClient.end()) {
		workForClient.insert(make_pair(hostName, 1));
	}
	else {
		i->second++;
	}
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
	memset(&address, '0', sizeof address);
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(PORT);  

  	// binding to port
	if (bind(lstn_soc, (struct sockaddr *)&address, sizeof address) == -1) { 
  		//error
		cout << "Unable to bind to port address\n"; 
		exit(1);
	}

  	// listen for connection requests with Backlog = 30 (reasonable limit -> Following Forum Post)
	if (listen(lstn_soc, 30) == -1) {
  		//error 
		cout << "Listen failed\n";
		exit(1);
	}

	cout << "Using port " << ntohs(address.sin_port) << "\n";
}

/**
 * Handles main server operations
 */
void serverOperations() {
	int numberOfSockets = 1; //number of clients that are communicating
	int maxSocketDesc = -1, socketDesc = -1;
	clientSockets.push_back(lstn_soc);

	//for timeout to work in select we need to set the socket as non-blocking
	//Implementation highly inspired from stack overflow code.
	u_long n = 1;
	ioctl(lstn_soc, FIONBIO, &n);

	while(true) {
		//The following code for select() is inspired from a GeeksForGeek Code 
		//clear the socket set 
		FD_ZERO(&fds);
		//add listening socket to set 
		FD_SET(lstn_soc, &fds);
		//track the maxSocketDesc
		maxSocketDesc = lstn_soc;
		
		//add new sockets to set, ignore 0 as lstn_soc is at pos 0
		for(int i = 1; i < numberOfSockets; i++) {
			//get the socket descriptors from the vector
			socketDesc = clientSockets[i];
			//add socket decriptor to list
			FD_SET(socketDesc, &fds);
			//update the highest file descriptor
			if(socketDesc > maxSocketDesc) {
				maxSocketDesc = socketDesc;
			}
		}

		struct timeval timeout = {30, 0}; //timeout after 30seconds of inactivity
		int activity = select(maxSocketDesc + 1, &fds, NULL, NULL, &timeout);
		
		if(activity == -1) {
			cout << "Select Error!\n";
			exit(1);
		}

		else if(activity == 0) {
			//this is when the timer ran out
			break;
		}

		//If something happened on the listening socket, then its an incoming connection 
		if (FD_ISSET(lstn_soc, &fds)){
			// accept a connection request
			int addressLen = sizeof address;
			conn_soc = accept(lstn_soc, (struct sockaddr *)&address, (socklen_t*)&addressLen);
			if(conn_soc == -1) {
				cout << "Accept Error\n";
				close(conn_soc);
				exit(1);
			}
			//setting new socket as non blocking as well.
			n = 1;
			ioctl(lstn_soc, FIONBIO, &n);
			//update number of clients after a connection was accepted
			numberOfSockets++;
			//add new socket to the vector
			clientSockets.push_back(conn_soc);
		}

		//else its some work on the other socket
		else {
			char inBuffer[1024] = {};
			//we ignore clientSocket.begin() as there we have lstn_soc
			for(auto i = clientSockets.begin() + 1; i < clientSockets.end(); i++) {
				socketDesc = *i;
				if (FD_ISSET(socketDesc, &fds)) {

					int valread = read(socketDesc, inBuffer, sizeof inBuffer);
					if(valread == -1) {
						cout << "Error reading from Client with Socket Description: " << socketDesc <<"\n";
						exit(1);
					}

					//do the transaction
					else {
						if(inBuffer[0] == 'Q') {
							clientSockets.erase(i);
							numberOfSockets--;
							close(socketDesc);
						}

						else {
							string inString = "";
							string outString = "D";
							//convert from cstring to string
							for (unsigned int i = 0; i < strlen(inBuffer); i++) {
								inString += inBuffer[i];
							}
							//start timer after first transaction is received
							//but do not reset it everytime
							if(transNumber == 0) {
								gettimeofday(&startTime, NULL);
							}
							size_t posOfFirstDot = inString.find('.');
							int work = stoi(inString.substr(0, posOfFirstDot));
							string hostName = inString.substr(posOfFirstDot + 1); 
							transNumber++;

							printOutput(hostName, work, 0);
							Trans(work);

							outString += to_string(transNumber);
							char outBuffer[outString.length()] = {};
							strcpy(outBuffer, outString.c_str());
							//write done command to socket
							send(conn_soc, outBuffer, sizeof outBuffer, 0);

							printOutput(hostName, work, 1);
							addToUMap(hostName);
							//end time after last transaction is over (keep resetting in each iteration)
							gettimeofday(&endTime, NULL);
						}
					}
				}
            }
		}
	}

}

/**
 * Close all sockets and clear the map and vector
 */
void closeConnection() {
	close(lstn_soc);
	clientSockets.clear();
}

/**
 * Print the Final Client Summary
 */
void finalSummary() {
	int totalTrans = 0;
	cout << "SUMMARY\n";
	for (auto client : workForClient) {
		cout << "  " << client.second << " transactions from " << client.first << "\n";
		totalTrans += client.second;
	}
	float execTime = 0.0;
	if(startTime.tv_usec != endTime.tv_usec) {
		execTime = (float) (endTime.tv_sec - startTime.tv_sec) + (float) (1e-6*(endTime.tv_usec - startTime.tv_usec));
	}
	float transPerSecond = 0.0;
	if (execTime > 0.0) {
		transPerSecond = totalTrans/execTime;
	}
	printf("%.1f", transPerSecond);
	cout << " transactions/sec\t(" << totalTrans << "/";
	printf("%.2f)\n", execTime); 
	workForClient.clear();
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
