/**
 client.c
 Chase Larocque
 AUCSC 380 -  Programming Assignment 4
 Requires A4Header.h
 
 Program adapted from code on eClass
 
 This program will use sockets to send data to a server program to generate random
 numbers. The data sent will be 2 integers, one representing the amount of
 random numbers to generate, the second is the maximum value. The client will
 then read the socket for the random numbers created by the server and print
 them to stdout. The client handles all integer validation before sending the
 data to the server.
 
 Program contains five methods:
 -main(int, char**)
 Function handles creation of the socket and connecting to the server's socket.
 Function calls grabIntegersFromUser(int)
 
 -grabIntegersFromUser(int)
 Function continuously asks the user for 2 integers, and validates that
 the integers follow the correct guidelines. Function then calls
 sendNumbersToServer(int, int, int) to send the numbers to the server
 for random number generation
 
 -flushLine()
 Function taken from eClass. Function will flush the buffer in case
 of an incorrect character is entered or more than 2 are entered at
 a single time.
 
 -sendNumbersToServer(int, int, int)
 Function writes the number of required numbers and the range to the
 server to create randomNumbers
 
 -printNumbers(int, struct lotteryInfo)
 Function prints the numbers from the server one at a time.

 **/
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include "A4Header.h"
#include <stdbool.h>

/**
 -main(int, char**)
 Function handles creation of the socket and connecting to the server's socket.
 Function calls grabIntegersFromUser(int). Returns EXIT_SUCCESS on success and
 EXIT_FAILURE on failure.
**/
int main(int argc, char **argv){
    
    if (argc != 3) {
        fprintf(stderr, "Incorrect Number Of Arguments. Arguments Should Be Address and Port Number, In That Order");
        exit(EXIT_FAILURE);
    }//if
    
    int socketFD;
    int lengthOfAddress;
    struct sockaddr_in address;

    //assign command line arguments to variables
    int portNumber = atoi(argv[2]);
    char *addressOfServer = argv[1];
    
    //create socket for client
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD == -1) {
        perror("SOCKET ERROR!\n");
        exit(EXIT_FAILURE);
    }//if
    
    //name socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(addressOfServer);
    address.sin_port = htons(portNumber);
    lengthOfAddress = sizeof(address);
    
    //connect socket with server's socket
    if(connect(socketFD, (struct sockaddr *)&address, lengthOfAddress) == -1){
        perror("Socket Connection Failed");
        exit(EXIT_FAILURE);
    }//if
    
    printf("Enter two positive integers, separated by one or more spaces.\nThe first integer indicates how many numbers are to be\nrandomly selected and the second integer specifies the\nsize of the range from which the numbers should be drawn. It is\nassumed that 1 is the start of the range; the maximum\nsize of the range is 65535.\n");
    
    grabIntegersFromUser(socketFD);

    close(socketFD);
    exit(EXIT_SUCCESS);
    
}//main()

/**
 -grabIntegersFromUser(int)
 Function continuously asks the user for 2 integers, and validates that
 the integers follow the correct guidelines. Function then calls
 sendNumbersToServer(int, int, int) to send the numbers to the server
 for random number generation
**/
void grabIntegersFromUser(int socketFD){
    int numbersToDraw;
    int maximumNumber;
    int scanfRetVal;
    
    while(true){
        printf("\nEnter Two Numbers OR CTRL+D to exti:\n");
        scanfRetVal = scanf("%d %d", &numbersToDraw, &maximumNumber);
        
        //break the while loop when user enters CTRL+D
        if (scanfRetVal == EOF) {
            break;
        }//if
        
        //if we were unable to read a character
        if(scanfRetVal == 0 || scanfRetVal == 1){
            printf("Invalid Character Entered. Enter Only Positive Integers!\n");
        }//if
        
        //we were able to read both integers
        if (scanfRetVal == 2) {
            
            //verify integers are within our parameters
            if(numbersToDraw <= 0 || maximumNumber <= 0){
                printf("Both Numbers MUST Be Positive!\n");
            }else if(numbersToDraw > 65535 || maximumNumber > 65535){
                printf("Both Numbers Must Be Less Than Or Equal to 65535!\n");
            }else if(numbersToDraw >= maximumNumber){
                printf("The Second Number MUST be Greater Than The First!\n");
            }else{ //numbers are vaild
                sendNumbersToServer(numbersToDraw, maximumNumber, socketFD);
            }//else
            
        }//if
        
        //clear buffer if invalid character entered or if more than 2 numbers eneter
        flushLine();
        
    }//while
}//grabIntegersFromUser

/**
 Function taken from eClass.
 Discard any characters (e.g., newline) left on the input line.
 From http://c-faq.com/stdio/gets_flush2.html
**/
void flushLine(){
    int flushedChar;
    while((flushedChar = getchar()) != '\n' && flushedChar != EOF)
        ;//discard
}//flushLine

/**
 -sendNumbersToServer(int, int, int)
 Function writes the number of required numbers and the range to the
 server to create randomNumbers
**/
void sendNumbersToServer(int numbersToDraw, int maximumNumber, int socketFD){
    struct lotteryInfo currentInfo = {numbersToDraw, maximumNumber};
    
    if(write(socketFD, &currentInfo, sizeof(currentInfo)) == -1){
        perror("WRITE ERROR!\n");
        exit(EXIT_FAILURE);
    }//if
    
    printNumbers(socketFD, currentInfo);
    
}//sendNumberToServer

/**
 Function prints the numbers as they are read from the socket connected
 to the server. 
**/
void printNumbers(int socketFD, struct lotteryInfo currentInfo){
    int randomNumber;

    printf("\nYour %d random numbers from 1 to %d are:\n", currentInfo.numbersToDraw, currentInfo.maximumNumber );
    
    for(int i = 0; i < currentInfo.numbersToDraw; i++){
        
        if(read(socketFD, &randomNumber, sizeof(int)) == -1){
            perror("READ ERROR!\n");
            exit(EXIT_FAILURE);
        }//if
        printf("%d\n", randomNumber);
        
    }//for
}//printNumbers
