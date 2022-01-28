/*
CPSC 441: Assignment 2 Kathryn Lepine 

This voting program connects with the indirection program and makes a UDP connection. Voting allows the client to see the 
candiadates, vote as many times as they want, and see the results of voting. 
To compile gcc -o voting voting.c 
To run ./voting

Code strongly references prof Carey Williamson class examples of madlib-server.c, madlib-client.c, wordlen-server.c, word-client.c
Code strongly references TA Soroosh Emaeilian tutorial exmaples of UDP_client.cpp and UDP_server.cpp
Code references C How to Program (Seventh Edition) - Paul Deitel and Havey Deitel 
*/
//import esstential libariries 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

//define IPs and ports
#define SERVER_IP "136.159.5.27"  /* csx3.cpsc.ucalgary.ca */
#define SERVER_PORT 8638
#define VOTE_PORT 8635

int main(int argc, char *argv[]){
    //intialize all candiadates, their Ids and current votes 
    char name1[] = "Alex";
    int id1 = 2;
    int vote1 = 4;
    char name2[] = "Liam";
    int id2 = 4;
    int vote2 = 4;
    char name3[] = "Noah";
    int id3 = 6;
    int vote3 = 4;
    char name4[] = "Jack";
    int id4 = 8;
    int vote4 = 4;
    //initializing socker addresses
    struct sockaddr_in si_server, si_client;
    struct sockaddr *server, *client;
    //initialzing variables 
    int server_socket, len=sizeof(si_server);
    //clearing address before use
    memset((char *) &si_server, 0, sizeof(si_server));
    //IPv4 protocol 
    si_server.sin_family = AF_INET;
    //port
    si_server.sin_port = htons(VOTE_PORT);
    //si_server.sin_addr.s_addr = inet_addr(SERVER_IP);
    //IP address of local host 
    si_server.sin_addr.s_addr = htonl(INADDR_ANY);
    server = (struct sockaddr *) &si_server;
    client = (struct sockaddr *) &si_client;
    //initialzing variables 
    int voted = 0;
    int readBytes;
    char buf[100];
    //creating socket 
    if((server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        printf("socket was not made\n");
        return 1;
    }
    printf("server socket made\n");
    //binding socket
    if(bind(server_socket, server, sizeof(si_server)) == -1){
        printf("error when binding\n");
        return 1;
    }
    printf("waiting for messages...\n"); 
    //while loop to get request and send back to client 
    while(1){
        //get request from client and store in buf
        if((readBytes = recvfrom(server_socket, buf, 100, 0, client, &len)) == -1){
            printf("Could not read\n");
        }
        printf("message in:%s \n", buf);
        //padding end of buf
        buf[readBytes] = '\0'; 
        //if request is A then show the list of candidates 
        if(strncmp(buf, "A", 1) == 0){
            //Show candidates
            //intialize and make message with all candidates and their IDs to send to client 
            char tosend[300];
            sprintf(tosend, "Name of candidate with ID\n%s %i\n%s %i\n%s %i\n%s %i\n",name1, id1, name2, id2, name3, id3, name4, id4);
            sendto(server_socket, tosend, strlen(tosend), 0, client, len);
            printf("Sending back: %s\n", tosend);	
        }else if(strncmp(buf, "B", 1) == 0){
            //request is B the client would like to vote 
            //now client will be able to see voting results 
            voted = 1;
            //get random key inbetween 0-9
            int key = rand() % 10;
            //initialize and clear tosend 
            char tosend[300];
            bzero(tosend, 300);
            //send the candidate options with their ids and the key at very end of string 
            sprintf(tosend, "Type ID of your desired candidate\n%s %i\n%s %i\n%s %i\n%s %i\n%i",name1, id1, name2, id2, name3, id3, name4, id4, key);
            sendto(server_socket, tosend, strlen(tosend), 0, client, len);
            printf("Sending back: %s\n", tosend);	
            printf("Key is: %i\n", key);
            //intilize and clear thevote before use
            char thevote[20];
            memset(thevote, 0, sizeof(thevote));
            //get message back from the client containing the id of the clients vote 
            if((readBytes = recvfrom(server_socket, thevote, 100, 0, client, &len)) == -1){
                printf("Could not read\n");
            }
            printf("message in:%s \n", thevote);
            //convert the vote from string to int
            int n = atoi(thevote);
            //unenrycpt the vote with the key
            int k = n / key; 
            printf("id number: %i\n", k);
            //check which id the vote matches to and get vote to that candidate 
            if(k == id1){
                vote1 = vote1 +1;
                printf("vote to alex\n");
            }if(k == id2){
                vote2 = vote2 +1;
                printf("vote to liam\n");
            }if(k == id3){
                vote3 = vote3 +1;
                printf("vote to noah\n");
            }if(k == id4){
                vote4 = vote4 +1;
                printf("vote to jack\n");
            }
            //intilize and make message to thank the client for voting 
            char thanks[100];
            sprintf(thanks, "Thanks for voting, you can now see results");
            //send thank you message to client 
            sendto(server_socket, thanks, strlen(thanks), 0, client, len);	
            printf("Sending back: %s \n", thanks);
        }else if((strncmp(buf, "C", 1) == 0) && (voted == 1)){
            //client would like to see voting results - note they can onyl see this if have voted
            //intitlize and make message to send with the current votes 
            char tosend[300];
            sprintf(tosend, "Name of candidate with votes\n%s %i\n%s %i\n%s %i\n%s %i\n",name1, vote1, name2, vote2, name3, vote3, name4, vote4);
            //send list of candidates with current votes to client 
            sendto(server_socket, tosend, strlen(tosend), 0, client, len);	
            printf("Sending back: %s\n", tosend);
        }else{
            //if client does not choose A B C then send invalid selection
            char tosend[] = "not valid selction";
            sendto(server_socket, tosend, strlen(tosend), 0, client, len);	
            printf("Sending back: %s \n", tosend);
        }
        //clear buf before use next while loop 
        memset(buf, '\0', strlen(buf));
    }
    //close socket 
    close(server_socket);
    return 0; 
}