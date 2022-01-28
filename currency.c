/*
CPSC 441: Assignment 2 Kathryn Lepine 

This currency program connects with the indirection program and makes a UDP connection. The program returns the exhange of CAN 
to EUR, BIT, US, GBP. 
To compile gcc -o currency currency.c 
To run ./currency

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
#define CURR_PORT 8636

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
    si_server.sin_port = htons(CURR_PORT);
    //IP address of local hos
    si_server.sin_addr.s_addr = htonl(INADDR_ANY);
    server = (struct sockaddr *) &si_server;
    client = (struct sockaddr *) &si_client;
    //binding socket
    if(bind(server_socket, server, sizeof(si_server)) == -1){
        printf("Could not bind to port\n");
        return 1;
    }
    printf("waiting for messages...\n"); 
    //while loop to get client request and send answers back 
    while(1){
        //get request from client and store in buf
        if ((readBytes = recvfrom(server_socket, buf, 100, 0, client, &len)) == -1){
            printf("error getting message from client\n");
        }
        printf("input is: %s", buf);
        char token[10];
        //copy buf into tocken
        //strcpy(token, buf);
        //intialize amt as float
        float amt;
        //convert string in float 
        //we know based on format dolar amount is a position 1 in string
        amt = atof(&buf[1]);
        printf("the num is: %f\n",amt);
        printf("input now is: %s", buf);
        //compare string against all different currency types 
        //if string matches then convert amt to that currency 
        //process is the same for each will only comment the first 
        if(strstr(buf, "US") != NULL){
            //exchange rate us to ca 
            float us = 0.8;
            //calcuate the amount in us dollars
            float can = amt * us;
            char tosend[20];
            //convert to string with two decical points
            sprintf(tosend, "%.2f", can);
            strcat(tosend, " US");
            //send result back to client 
            sendto(server_socket, tosend, strlen(tosend), 0, client, len);
            printf("Sending back: %s\n", tosend);	
        }else if(strstr(buf, "EUR") != NULL){
            float eur = 0.69;
            float can = amt * eur;
            char tosend[20];
            sprintf(tosend, "%.2f", can);
            strcat(tosend, " EUR");
            sendto(server_socket, tosend, strlen(tosend), 0, client, len);
            printf("Sending back: %s\n", tosend);	
        }else if(strstr(buf, "GBP") != NULL){
            float gbp = 0.59;
            float can = amt * gbp;
            char tosend[20];
            sprintf(tosend, "%.2f", can);
            strcat(tosend, " GBP");
            sendto(server_socket, tosend, strlen(tosend), 0, client, len);
            printf("Sending back: %s\n", tosend);	
        }else if(strstr(buf, "BIT") != NULL){
            float bit = 0.000014;
            float can = amt * bit;
            char tosend[20];
            sprintf(tosend, "%f", can);
            strcat(tosend, " BIT");
            sendto(server_socket, tosend, strlen(tosend), 0, client, len);
            printf("Sending back: %s\n", tosend);	
        }else{
            //if string does not match above send the instructions back 
            char tosend[] = "Hello!\nWelcome to the currency microservice.\nSupporting CAN to US, EUR, GBP, BIT\n Use format $x.xx CAN US\n";
            sendto(server_socket, tosend, strlen(tosend), 0, client, len);	
            printf("Sending back %s\n", tosend);
        } 
        //clear buf before use on next while loop
        memset(buf, '\0', sizeof(buf));
    }
    //close socket 
    close(server_socket);
    return 0; 
}