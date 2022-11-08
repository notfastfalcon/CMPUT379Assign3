#ifndef _HEADER_H_
#define _HEADER_H_

#include "string"
#include "vector"

// CLIENT FUNCTIONS
void clientConnection(int, char*);
std::string getHostName(void);
void loggedToFile(std::string);
void clientOperations(void);
void closeConnection(void);
void finalSummary(void);

// SERVER FUNCTIONS
void serverConnection(int, char*);


#endif