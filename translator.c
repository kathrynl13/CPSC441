/*
CPSC 441: Assignment 2 Kathryn Lepine 

This translator program makes a UDP connection with the program indirection. The translator program translator five english 
words to french words: hello, goodbye, no, yes, thank you
To compile gcc -o translator translator.c 
To run ./translator

Code strongly references prof Carey Williamson class examples of madlib-server.c, madlib-client.c, wordlen-server.c, word-client.c
Code strongly references TA Soroosh Emaeilian tutorial exmaples of UDP_client.cpp and UDP_server.cpp
Code references C How to Program (Seventh Edition) - Paul Deitel and Havey Deitel 
*/
//import esstential libariries 
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>

//define IPs and ports
#define SERVER_IP "136.159.5.27"  /* csx3.cpsc.ucalgary.ca */
#define SERVER_PORT 8638
#define TRANS_PORT 8637

int main(int argc, char *argv[]){
    //initializing socket addresses
    struct sockaddr_in si_server, si_client;
    struct sockaddr *server, *client;
    //initialzing variables 
    int server_socket, len=sizeof(si_server);
    int readBytes;
    char buf[100];
    //create server socket 
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        printf("error setting up socket \n");
        return 1;
    }
    printf("socket made\n");
    //initalizing to address to zero
    memset((char *) &si_server, 0, sizeof(si_server));
    //IPv4 protocol 
    si_server.sin_family = AF_INET;
    //port
    si_server.sin_port = htons(TRANS_PORT);
    //si_server.sin_addr.s_addr = inet_addr(SERVER_IP);
    //IP address of local host 
    si_server.sin_addr.s_addr = htonl(INADDR_ANY);
    server = (struct sockaddr *) &si_server;
    client = (struct sockaddr *) &si_client;
    
    //binding server socket
    if(bind(server_socket, server, sizeof(si_server)) == -1){
        printf("error binding socket \n");
    }
    printf("waiting for messages...\n"); 
    //main loop to reply to clients requests 
    while(1){
        //get message from client, put in buf
        if((readBytes = recvfrom(server_socket, buf, 100, 0, client, &len)) == -1){
            printf("error receiving message\n");
        }
        printf("message in:%s \n", buf);
        //padding end of string 
        buf[readBytes] = '\0'; 
        //compare the message against the 5 key words hello, yes, no, goodbye, thank you or instructions
        //if the string matches then create new message with the french translation and send back to client 
        if(strncmp(buf, "instructions", 12) == 0){
            char tosend[] = "Hello!\n Welcome to the english to french microservice.\nPlease type hello, goodbye, yes, no, thank you";
            sendto(server_socket, tosend, strlen(tosend), 0, client, len);
            printf("Sending back: %s\n", tosend);	
        }else if(strncmp(buf, "hello", 5) == 0){
            char tosend[] = "bonjour\n";
            sendto(server_socket, tosend, strlen(tosend), 0, client, len);	
            printf("Sending back: %s\n", tosend);
        }else if(strncmp(buf, "goodbye", 7) == 0){
            char tosend[] = "au revior";
            sendto(server_socket, tosend, strlen(tosend), 0, client, len);	
            printf("Sending back: %s\n", tosend);
        }else if(strncmp(buf, "yes", 3) == 0){
            char tosend[] = "oui";
            sendto(server_socket, tosend, strlen(tosend), 0, client, len);	
            printf("Sending back: %s\n", tosend);
        }else if(strncmp(buf, "no", 2) == 0){
            char tosend[] = "non";
            sendto(server_socket, tosend, strlen(tosend), 0, client, len);	
            printf("Sending back: %s\n", tosend);
        }else if(strncmp(buf, "thank you", 9) == 0){
            char tosend[] = "merci";
            sendto(server_socket, tosend, strlen(tosend), 0, client, len);	
            printf("Sending back: %s\n", tosend);
        }else{
            //if string does not match any of the require send back that is is invalid entry
            char tosend[] = "not valid word";
            sendto(server_socket, tosend, strlen(tosend), 0, client, len);	
            printf("Sending back: %s \n", tosend);
        }
        //clear the buf to be used in next while loop 
        memset(buf, '\0', strlen(buf));
    }
    //close the socket 
    close(server_socket);
    return 0; 
}