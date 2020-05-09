/**
 server.c
 Chase Larocque
 AUCSC 380 - Programming Assignment 4
 April 15
 Code adapted from server.c on eClass.
 Requires A4Header.h
 
 This c program will act as a server for the generation of random numbers
 in a specified range. The program will connect via sockets to a client and
 fork a child process for each client connected. The server will then
 generate N random numbers from 1...M inclusive, depending on the client's
 request.
 
 File Contains 2 methods:
 -main(int, char**)
 Function handles creation of the socket, and the connection to a client.
 Function will also fork a child process for each client and call generateRandomNumbers(int)
 to send random numbers back to the client.
 
 -generateRandomNumbers(int)
 Function reads data from the socket (number of numbers to generate and range) and
 sends numbers back to the client, one at a time.
 **/

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include "A4Header.h"
#include <time.h>
#include <stdbool.h>

/**
 -main(int, char**)
 Function handles creation of the socket, and the connection to a client.
 Function will also fork a child process for each client and call generateRandomNumbers(int)
 to send random numbers back to the client.
**/
int main(int argc, char** argv){
    
    if (argc != 2) {
        fprintf(stderr, "Incorrect Number Of Arguments. Only Argument Should Be Port Number");
        exit(EXIT_FAILURE);
    }//if
    
    int server_sockfd, client_sockfd;
    socklen_t server_len, client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    
    int portNumber = atoi(argv[1]);

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(server_sockfd == -1){
        perror("SOCKET CREATION ERROR!\n");
        exit(EXIT_FAILURE);
    }//if
    
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(portNumber);
    server_len = sizeof(server_address);
    bind(server_sockfd, (struct sockaddr *) &server_address, server_len);
    
    printf("Server Started!\n\n");

    listen(server_sockfd, 5);
    signal(SIGCHLD, SIG_IGN);
    
    while(true) {
        client_len = sizeof(client_address);
        client_sockfd = accept(server_sockfd,
                               (struct sockaddr *)&client_address, &client_len);
        printf("New Client Connected!\n\n");
        
        //fork for each new client
        switch (fork()) {
            case -1:
                perror("FORK ERROR!");
                exit(EXIT_FAILURE);
                break;
                
            case 0:
                //child
                generateRandomNumbers(client_sockfd);
                if(close(client_sockfd) == -1){
                    perror("CLOSE ERROR!\n");
                    exit(EXIT_FAILURE);
                }//if
                printf("Client Disconnected!\n\n");
                exit(EXIT_SUCCESS);
                
            default:
                //parent
                if(close(client_sockfd) == -1){
                    perror("CLOSE ERROR!\n");
                    exit(EXIT_FAILURE);
                }//if
                break;
        }//switch
    }//while
}//main

/**
 -generateRandomNumbers(int)
 Function reads data from the socket (number of numbers to generate and range) and
 sends numbers back to the client, one at a time.
**/
void generateRandomNumbers(int clientFD){
    srand((unsigned) time(NULL));//called once per child
    struct lotteryInfo currentInfo;
    ssize_t retVal;
    
    printf("Waiting for info from client...\n");
    
    while((retVal = read(clientFD, &currentInfo, sizeof(currentInfo))) != 0){
        if (retVal == -1) {
            perror("Read Failure\n");
            exit(EXIT_FAILURE);
        }//if
        
        printf("Numbers To Draw: %d\n", currentInfo.numbersToDraw);
        printf("Max Number: %d\n", currentInfo.maximumNumber);

        int remainingNumbers = currentInfo.numbersToDraw;
        int maximumNumber = currentInfo.maximumNumber;
        int N = currentInfo.maximumNumber;

        //Random number generation from pseudocode in assignment specification
        for(int i = 1; i <= N; i++){
            if((rand() / ((double) RAND_MAX + 1)) < ((double)remainingNumbers / maximumNumber)){
                write(clientFD, &i, sizeof(int));
                remainingNumbers --;
            }//if
            maximumNumber--;
        }//for
        printf("Random Numbers Sent To The Client!\n\n");
        printf("Waiting for info from client...\n\n");
    }//while
}//generateRandomNumbers
