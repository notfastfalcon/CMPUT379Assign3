#ifndef _HEADER_H_
#define _HEADER_H_

#include "string"
#include "vector"

// CLIENT FUNCTIONS
void clientConnection(int, char*);
std::string getHostName(void);
std::string getTime(void);
void loggedToFile(std::string);
void clientWriteOperations(void);
void clientReadOperations(void);
void closeConnection(void);
void finalSummary(void);

// SERVER FUNCTIONS
std::string getTime(void);
void loggedToFile(std::string);
void printOutput(std::string, int, int);
void addToUmap(std::string);
void serverConnection(int, char*);
void serverOperations(void);
void closeConnection(void);
void finalSummary(void);

#endif