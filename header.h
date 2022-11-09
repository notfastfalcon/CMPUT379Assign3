#ifndef _HEADER_H_
#define _HEADER_H_

#include "string"
#include "vector"

// CLIENT FUNCTIONS
void clientConnection(int, char*);
std::string getHostName(void);
void printEpochTime(void);
void loggedToFile(std::string);
void clientWriteOperations(void);
void clientReadOperations(void);
void closeConnection(void);
void finalSummary(void);

// SERVER FUNCTIONS
void printEpochTime(void);
void loggedToFile(std::string);
void serverConnection(int, char*);
void serverOperations(void);
void closeConnection(void);
void finalSummary(void);

#endif