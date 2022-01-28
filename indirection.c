/*
CPSC 441: Assignment 2 Kathryn Lepine 

This indirection program makes a server socket to create a TCP connection with client.c. The indirection then based upon the clients 
commands will make a UDP conncetion with one of three mircoservices: currency, translator, voting. 
To compile gcc -o indirection indirection.c 
To run ./indirection

Code strongly references TA Amirhossien Sefati tutorial examples of exampleClient.c and exampleServer.C
Code strongly references prof Carey Williamson class examples of madlib-server.c, madlib-client.c, wordlen-server.c, word-client.c
Code strongly references TA Soroosh Emaeilian tutorial exmaples of UDP_client.cpp and UDP_server.cpp
Code strongly references https://inst.eecs.berkeley.edu//~ee122/sp05/sockets-select_function.pdf to create timeouts when working 
with UDP connections (this is referenced again below where was used)
Code references C How to Program (Seventh Edition) - Paul Deitel and Havey Deitel 
*/
//import esstential libariries 
#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include<time.h>

//define IPs and ports
#define SERVER_IP "136.159.5.27"  /* csx3.cpsc.ucalgary.ca */
#define SERVER_PORT 8638
#define TRANS_PORT 8637
#define CURR_PORT 8636
#define VOTE_PORT 8635

int main(int argc, char *argv[]){
    //initialize variable for sockets 
	int server_socket, client_socket, trans_socket;
    //server socket address inistialization 
	struct sockaddr_in server;
    //initialze msgin char variable
	char msgin[500];
    //initialize pid for when fork is called 
    pid_t pid;
    //IPv4 protocol 
	server.sin_family = AF_INET;
    //IP address from local machine 
	server.sin_addr.s_addr = INADDR_ANY;
    //port number
	server.sin_port = htons(SERVER_PORT);

	//Create socket with TCP
	if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("could not create socket\n");
    }
	puts("socket created\n");
	
	//bind server socket 
    if((bind(server_socket, (struct sockaddr *)&server, sizeof(server)) < 0)){
        printf("bind failed");
        return 0; 
    }
	printf("Binding done.\n");
	
	//listen on server socket 
	listen(server_socket , 5);
	
    //goto here for the parent of the fork process
    jump:
        printf("waiting for clients...\n");
        
        //server socket accepts connection from client socket 
        client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0){
            perror("connection failed to client");
            return 1;
        }
        printf("connection of client done\n");
        //fork process 
        pid = fork();
        //if pid is less then 0, then there was error forking
        if(pid < 0){
            printf("fork call failed\n");
        //if pid is equal to zero we are in the child process
        }else if(pid ==0){
            //in the child process
            //close the server socket that is for the parent process
            close(server_socket);
            //main while loop to listen and respond to client 
            while(1){
        //voteCrash:
                //clear msgin before use
                memset(msgin, '\0', strlen(msgin));
                printf("top of while loop\n");
                //get message from client
                if((recv(client_socket, msgin, 5000, 0)) < 0){
                    printf("Error in receiving!");
                    break;
                }
                printf("Client says: %s\n", msgin);
                //if message is 1 the client desires the translator microservice 
                if(strncmp(msgin, "1", 1) == 0){
                    //TRANSLATOR/////////////////////////////////////////////////////////////////////////////////////
                    printf("in translator\n");
                    //create and send intro message to the client about using translator
                    char msgout[500];
                    sprintf(msgout, "Welcome! I am the UDP translation client!\nFor instruction type 'instructions'\n");
                    if((send(client_socket , msgout , strlen(msgout), 0)) < 0){
                        printf("problem sending to client");
                    }
                    //while loop to get information from client and send to translator 
                    while(1){
                        //clear and make request char before use
                        char request[500];
                        bzero(request, 500);
                        //get request from client
                        if((recv(client_socket, request, 500, 0)) < 0){
                            printf("error getting message from client");
                            break;
                        } 
                        printf("Client says: %s\n", request);
                        //if clients message is 0 they want to leave translator
                        //send goodbye message to them and break the while loop 
                        if(strncmp(request, "0", 1) == 0){
                            send(client_socket , "Bye from translator" , 20, 0);
                            break;
                        }
                        //address initialization
                        struct sockaddr_in servaddr;
                        //create socket for translation microservice
                        if((trans_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
                            printf("socket creation failed");
                        }
                        //initializing address to zero
                        memset(&servaddr, 0, sizeof(servaddr));
                        //IPv4 protocol 
                        servaddr.sin_family = AF_INET;
                        //give port of translator
                        servaddr.sin_port = htons(TRANS_PORT);
                        //use local machine IP address
                        servaddr.sin_addr.s_addr = INADDR_ANY;
                        //initialize varaibles for use
                        char answer[500];  
                        bzero(answer, 500);  
                        int bytes = 0;
                        int len;
                        //send clients message to the translator through UDP
                        sendto(trans_socket, (const char *)request, strlen(request),
                            MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
                                sizeof(servaddr));
                        printf("Message sent to translator: %s\n", request);
                        //the implementation of the timeout circuit refrerences this website 
                        //https://inst.eecs.berkeley.edu//~ee122/sp05/sockets-select_function.pdf
                        //intitailizing and setting up for the timeout 
                        struct timeval timer;
                        fd_set readfds, master;
                        FD_ZERO(&readfds);
                        FD_ZERO(&master);
                        FD_SET(trans_socket, &master);
                        //setting time until timeout
                        timer.tv_sec=2;
                        timer.tv_usec=0;
                        readfds=master;
                        //choosing which socket to read from 
                        select(trans_socket+1, &readfds, NULL, NULL, &timer);
                        if(FD_ISSET(trans_socket, &readfds)){
                            //gets message from translator no problem
                            recvfrom(trans_socket, (char *)answer, 500, 
                            MSG_WAITALL, (struct sockaddr *) &servaddr,(socklen_t *)&len);
                            FD_CLR(trans_socket, &readfds);
                            printf("Translator said: %s\n", answer);
                            //send message from translator to client 
                            if((send(client_socket , answer , 500, 0)) < 0){
                                printf("Message received failed");
                            }
                        }else{
                            //there was nothing received from translator
                            char errormsg[] = "Sorry message lost, please send again";
                            //send message to re send request to the client 
                            if((send(client_socket , errormsg , sizeof(errormsg), 0)) < 0){
                                printf("1Message received failed");
                            }
                        }
                    }
                    //close translation socket
                    close(trans_socket); 
                //client say 2 meaning they would like currency microservice
                }else if(strncmp(msgin, "2", 1) == 0){
                    //CURRENCY///////////////////////////////////////////////////////////////////////////////////////
                    printf("in currency\n");
                    //intializing and defining char msgout to send intro to client
                    char msgout[500];
                    sprintf(msgout, "Welcome! I am the UDP currency client!\n");
                    //send opening message to client 
                    if(send(client_socket , msgout , strlen(msgout), 0) < 0){
                        printf("failed to send to client");
                    }
                    //while loop to get requests of client and send back message from currency 
                    while(1){
                        //initializing request and clearing before use
                        char request[500];
                        bzero(request, 500);
                        //get message request from client
                        if((recv(client_socket, request, 500, 0)) < 0){
                            printf("could not get client message");
                            break;
                        }
                        printf("Client says: %s\n", request);
                        //if the message request is 0 the client wants to leave currency microserivce
                        if(strncmp(request, "0", 1) == 0){
                            //send goodbye to client 
                            send(client_socket , "Bye from translator" , 20, 0);
                            break;
                        }
                        //socket address inistialization 
                        struct sockaddr_in servaddr;
                        //create socket for currency microservice 
                        if ((trans_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
                            printf("could not create currency socket");
                        }
                        //clear address before use
                        memset(&servaddr, 0, sizeof(servaddr));
                        //IPv4 protocol 
                        servaddr.sin_family = AF_INET;
                        //port number of currency
                        servaddr.sin_port = htons(CURR_PORT);
                        //IP address from local host
                        servaddr.sin_addr.s_addr = INADDR_ANY;
                        int len;
                        //send request tp currency server 
                        sendto(trans_socket, (const char *)request, strlen(request),
                            MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
                                sizeof(servaddr));
                        printf("Message sent to currency: %s\n", request);
                        //intializing and clearing of answer before use
                        char answer[500];  
                        bzero(answer, 500);  
                        //the implementation of the timeout circuit refrerences this website 
                        //https://inst.eecs.berkeley.edu//~ee122/sp05/sockets-select_function.pdf
                        //intitailizing and setting up for the timeout 
                        struct timeval timer;
                        fd_set readfds, master;
                        FD_ZERO(&readfds);
                        FD_ZERO(&master);
                        FD_SET(trans_socket, &master);
                        //setting time allowance until timeout
                        timer.tv_sec=2;
                        timer.tv_usec=0;
                        readfds=master;
                        //choosing which socket to read from 
                        select(trans_socket+1, &readfds, NULL, NULL, &timer);
                        if(FD_ISSET(trans_socket, &readfds)){
                            //get message from currency server 
                            recvfrom(trans_socket, (char *)answer, 500, 
                            MSG_WAITALL, (struct sockaddr *) &servaddr,(socklen_t *)&len);
                            FD_CLR(trans_socket, &readfds);
                            printf("Currency said: %s\n", answer);
                            //send answer back from currency to client 
                            if((send(client_socket , answer, 500, 0)) < 0){
                                printf("Message received failed");
                            }
                        }else{
                            //currency server did not respond
                            //send error message to client 
                            char errormsg[] = "Sorry message lost, please send again";
                            if((send(client_socket , errormsg , sizeof(errormsg), 0)) < 0){
                                printf("Message received UDP failed");
                            }
                        }
                    }
                //close currency socket 
                close(trans_socket); 
                //client choose 3 they would like the voting microsevice  
                }else if(strncmp(msgin, "3", 1) == 0){
                    //VOTING///////////////////////////////////////////////////////////////////////////////////////
                    printf("in voting\n");
                    //initialize msgout and assign welcoming message 
                    char msgout[500];
                    sprintf(msgout, "Welcome! I am the UDP voting client!\nCandidates: A\nVote: B\nResults after voted: C\n");
                    //send welcoming messge to client from voting
                    if(send(client_socket , msgout , strlen(msgout), 0) < 0){
                        printf("Message not sent");
                    }
                    //main while loop to intract with client while in the voting microservice 
                    while(1){
                        //intitize request and clear before use 
                        printf("top of voting while loop\n");
                        char request[500];
                        bzero(request, 500);
                        //receive request from client 
                        if((recv(client_socket, request, 500, 0)) < 0){
                            printf("could not get message from client");
                            break;
                        }
                        printf("Client says:: %s\n", request);
                        //if the request is 0 then client would like to leave voting 
                        if(strncmp(request, "0", 1) == 0){
                            //send goodbye message to client and break while loop 
                            send(client_socket , "Bye from translator" , 20, 0);
                            break;
                        }
                        //address intitialization 
                        struct sockaddr_in servaddr;
                        //making socket for voting 
                        if((trans_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
                            printf("could not make voting socket");
                        }
                        //clear address before use 
                        memset(&servaddr, 0, sizeof(servaddr));
                        //IPv4 protocol 
                        servaddr.sin_family = AF_INET;
                        //voting port
                        servaddr.sin_port = htons(VOTE_PORT);
                        //IP address from local machine
                        servaddr.sin_addr.s_addr = INADDR_ANY;
                        int len;
                        //send client request to voting server 
                        sendto(trans_socket, (const char *)request, strlen(request),
                            MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
                                sizeof(servaddr));
                        printf("Message sent to voting: %s\n", request);
                        //intilize and clear answer before using
                        char answer[500];  
                        bzero(answer, 500);  
                        //the implementation of the timeout circuit refrerences this website 
                        //https://inst.eecs.berkeley.edu//~ee122/sp05/sockets-select_function.pdf
                        //intitailizing and setting up for the timeout 
                        struct timeval timer;
                        fd_set readfds, master;
                        FD_ZERO(&readfds);
                        FD_ZERO(&master);
                        FD_SET(trans_socket, &master);
                        //how long until timeout
                        timer.tv_sec=2;
                        timer.tv_usec=0;
                        readfds=master;
                        //choosing which socket to read from 
                        select(trans_socket+1, &readfds, NULL, NULL, &timer);
                        if(FD_ISSET(trans_socket, &readfds)){
                            //get answer from voting server 
                            recvfrom(trans_socket, (char *)answer, 500, 
                            MSG_WAITALL, (struct sockaddr *) &servaddr,(socklen_t *)&len);
                            FD_CLR(trans_socket, &readfds);
                            printf("Voting said:: %s\n", answer);
                            //send answer from voting server back to client 
                            if((send(client_socket , answer , 500, 0)) < 0){
                                printf("Message received failed");
                            }
                        }else{
                            //no response from voting server 
                            //send error message back to client 
                            char errormsg[] = "Sorry message lost, taking you back to main menu";
                            if((send(client_socket , errormsg , sizeof(errormsg), 0)) < 0){
                                printf("Message received UDP failed");
                            }
                            break;
                        }
                    }
                //close voting socket 
                close(trans_socket); 
                //if message is x then the client would like to end its connection
                }else if(strncmp(msgin, "x", 1)){
                    //close client socket 
                    printf("leaving client\n");
                    close(client_socket);
                    break;
                }
            }
        }else{
            //the parent goes back to listening 
            //close the client socket that is for the child 
            close(client_socket);
            //go to accept new connections
            goto jump;
        }
	return 0;
}