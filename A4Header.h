/**
 Chase Larocque
 AUCSC 380 - Programming Assignment 4
 April 15
 
 Header file has function prototypes and struct lotterInfo definition
**/

#ifndef A4Header_h
#define A4Header_h

//struct for reading and writing in socket
struct lotteryInfo{
    int numbersToDraw;
    int maximumNumber;
};
//================Prototypes for client.c================
/**
-grabIntegersFromUser(int)
Function continuously asks the user for 2 integers, and validates that
the integers follow the correct guidelines. Function then calls
sendNumbersToServer(int, int, int) to send the numbers to the server
for random number generation.
 **/
void grabIntegersFromUser(int socketFD);

/**
 -flushLine()
 Function taken from eClass. Function will flush the buffer in case
 of an incorrect character is entered or more than 2 are entered at
 a single time.
 **/
void flushLine(void);

/**
 -sendNumbersToServer(int, int, int)
 Function writes the number of required numbers and the range to the
 server to create randomNumbers.
 **/
void sendNumbersToServer(int numberToDraw, int maximumNumber, int socketFD);

/**
-printNumbers(int, struct lotteryInfo)
Function prints the numbers from the server one at a time.
 **/
void printNumbers(int, struct lotteryInfo);

//=================prototypes for server.c==================

/**
 -generateRandomNumbers(int)
 Function reads data from the socket (number of numbers to generate and range) and
 sends numbers back to the client, one at a time.
 **/
void generateRandomNumbers(int clientFD);

#endif
