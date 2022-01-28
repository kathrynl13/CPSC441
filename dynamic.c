//CPSC 441: Assignment 1
//Kathryn Lepine 30044629
//This program allows the user to enter commands in the terminal. A client socket is created to pass commands onto 
//the proxy.c to dynamically change which websites get blocked and unblocked. 

//References 
//Code heavily references tutorial material of TA Amirhossein Sefati
//Code references Carey Williamson's sample client and server Assignment 0 code

//importing libararies 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>

//initializing variable for socket 
int sock;

//will close socket before ending program if needed 
void catacher(int sig){
    close(sock);
    exit(0);
}

int main(int argc, char *argv[]){
    //address initialization for socket 
    struct sockaddr_in address;
    //initializing address to zero
    memset(&address, 0, sizeof(address));
    //IPv4 protocol 
    address.sin_family = AF_INET;
    //port 8638
    address.sin_port = htons(8638);
    
    //printing opening message 
    printf("Hello!\nTry blocking or unblocking content with the following commands\n");
    printf("blockfloppy\nblockspongebob\nblockcurling\nblockall\nfreefloppy\nfreespongebob\nfreecurling\nfreeall\nblockmsgall\nfreemsgall\n");

    //while loop waits for user to type message then send message to server
    while(1){
        //initialing message 
        char message[15];
        printf("Enter a command to block or unblock content: ");
        //reading message from user 
        scanf("%s", &message);

        //if the user inputs a message then create a socket, send message, and close socket 
        if(strlen(message) > 0){
            //create a socket with IPv4 and TCP protocol
            sock = socket(AF_INET, SOCK_STREAM, 0);
            if(sock == -1){
                printf("Could not create socket\n");
            }
            //connect socket to server
            if(connect(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) > 0){
                printf("Failed connection\n");
            }
            //send message to server
            if(send(sock, message, 100, 0) < 0){
                    puts("Sending message failed\n");
            }
            //return success message to user 
            puts("socket was created and connected. Message was sent.\n");
            //close socket 
            close(sock);
        }
    }
}
